
#include <omnetpp.h>
#include <typeinfo>
#include "TraceUtils.cc"
#include "Packet_m.h"
#include "MPLSLabel.cc"
#include <vector>
#include <iomanip>

using namespace omnetpp;
using namespace std;

static int label = 20;

class Router: public cSimpleModule{

private:
    int nodeIndex;

    bool flag;

    int currentLabelIndex = 4;

    typedef map<int, int> RoutingTable;  // destaddr -> gateindex
    RoutingTable rtable;

    typedef map<int, MPLSLabelByInLabel> CacheByInLabel;
    CacheByInLabel labelCache;

    typedef map<string, MPLSLabelByIP> CacheByIP;
    CacheByIP ipCache;

    typedef map<int, map<int, int>> TracebackTable;
    TracebackTable tracebackTable;

    typedef map<int, TraceLabelPojo> TracebackHelper;
    TracebackHelper tracebackHelper;

    typedef map<string, ValidationPojo> ValidationHelper;
    ValidationHelper validationHelper;

    int buildLsp(bool isLER, string src, string prefix, int inGate, int dstIndex, int hopCount);

    int findTraceLabel(int inGate, int lastLabel);

    void traceback(Packet *pk);

    cTopology *topo;

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
};
Define_Module(Router)

void Router::finish() {
    cout <<"********R" << nodeIndex << endl;
    cout << setw(15) <<"in Intf" << setw(15) << "inLabel" << setw(15) << "dstAddr" << setw(15) << "out Intf" << setw(15) << "outLabel" <<endl;
    CacheByInLabel::iterator iter = labelCache.begin();
    while (iter != labelCache.end()) {
        MPLSLabelByInLabel temp = iter->second;
        if (iter->first < 0) {
            cout<< setw(15) << "null" << setw(15) << "null";
        }
        else {
            cout<< setw(15) << temp.inIf << setw(15) << iter->first;
        }
        cout<< setw(15) <<temp.dst;
        cout<< setw(15) <<temp.outIf;
        cout<< setw(15) <<temp.outLabel << endl;
        iter++;
    }

    cout << setw(15) <<"Current TL" << setw(15) << "in Intf" << setw(15) << "Last TL" <<endl;
    TracebackHelper::iterator it = tracebackHelper.begin();
    while (it != tracebackHelper.end()) {
        TraceLabelPojo temp = it->second;
        cout << setw(15) <<it->first << setw(15) << temp.inGate << setw(15) << temp.lastLabel <<endl;
        it++;
    }

    delete topo;
}

void Router::initialize() {
    nodeIndex = par("nodeIndex");
    flag = par("flag");

    topo = new cTopology("topo");
    vector<string> nedTypes;
    nedTypes.push_back("Router");
    nedTypes.push_back("Switch");
    nedTypes.push_back("Host");
    topo->extractByNedTypeName(nedTypes);
    EV << "cTopology found " << topo->getNumNodes() << " nodes\n";

    // 得到当前节点
    cTopology::Node *thisNode = topo->getNode(nodeIndex);

    // 动态路由生成
    for (int i = 0; i < topo->getNumNodes(); i++) {
        cTopology::Node* node = topo->getNode(i);
        EV <<"target node type: " << node->getModule()->getClassName() << endl;
        if (strncmp("Host", node->getModule()->getClassName(), 5)) {
            EV <<"target node is not host, skip" << endl;
            continue;
        }

        topo->calculateUnweightedSingleShortestPathsTo(node);
        if (thisNode->getNumPaths() == 0)
            continue;  // not connected

        //得到最短路径并记录在路由表中
        cGate *parentModuleGate = thisNode->getPath(0)->getLocalGate();
        EV << thisNode->getPath(0)->getRemoteNode()->getModule()->getName() << endl;
        int gateIndex = parentModuleGate->getIndex();
        rtable[i] = gateIndex;
        EV << "  towards address " << i << " gateIndex is " << gateIndex << endl;
    }
}

void Router::handleMessage(cMessage *msg) {
    int inGate = msg->getArrivalGate()->getIndex();

    Packet *pk = check_and_cast<Packet *>(msg);
    pk->setHopCount(pk->getHopCount() + 1);
	
	if (pk->getKind() == 0) {
	    string declaredAddr = pk->getDeclaredAddr();
		string destAddr = pk->getDestAddr();
		//string prefix = destAddr.substr(0, destAddr.size() - 1).append("*");

		int dstIndex = Singleton::get_instance().getIndex(destAddr);

		int inLabel = pk->getForwardLabel();
		int outLabel = 0;
		int outGateIndex = 0;

		// inlabel为0，代表是入口路由器，尚未分配标签
		if (inLabel == 0) {
		    pk->setEntrance(nodeIndex);

			//CacheByIP::iterator it = ipCache.find(prefix);
			CacheByIP::iterator mapIter = ipCache.find(destAddr);
			if (mapIter == ipCache.end()) {
			    // no lsp found, build lsp first
			    buildLsp(true, declaredAddr, destAddr, inGate, dstIndex, 1);
			}

			MPLSLabelByIP pojo = ipCache.find(destAddr)->second;
			EV << pojo.str()<<endl;
			outLabel = pojo.outLabel;
			outGateIndex = pojo.outIf;

			EV <<"outLabel: "<< outLabel << endl;
			EV <<"outGateIndex: "<< outGateIndex << endl;
			EV <<"inGateIndex: "<< inGate << endl;
		} else {
		    //inLabel不为0，直接查找对应outLabel即可
		    CacheByInLabel::iterator it = labelCache.find(inLabel);
		    outGateIndex = it->second.outIf;
		    outLabel = it->second.outLabel;
		}

		pk->setForwardLabel(outLabel);

		int lastLabel = pk->getTraceLabel();
		if (lastLabel == 0) {
			pk->setTraceLabel(3);
		} else {
			pk->setTraceLabel(findTraceLabel(inGate, lastLabel));
		}
		EV << "TraceBack Label: " << pk->getTraceLabel()<< endl;

        if (flag) {
            string srcAddr = pk->getSrcAddr();
            ValidationHelper::iterator vit = validationHelper.find(srcAddr);
            if (vit == validationHelper.end()) {
                bubble("find fake src address");
                traceback(pk);
                return;
            }
            else {
                if (inGate != vit->second.intf || pk->getHopCount() != vit->second.hopCount) {
                    bubble("find fake src address");
                    traceback(pk);
                    return;
                }
            }
            bubble("src address validation passed");
        }

		send(pk, "out", outGateIndex);
	} else {
		traceback(pk);
	}
}

void Router::traceback(Packet *pk) {
    int currentLabel = pk->getTraceLabel();
    if (currentLabel == 3) {
        bubble("Find Start Router");
        if (nodeIndex != pk->getEntrance()) {
            EV<<"false"<<endl;
        } else {
            cout<<"true entrance"<<endl;
        }
        return;
    }
    TracebackHelper::iterator it = tracebackHelper.find(currentLabel);
    int lastLabel = it->second.lastLabel;
    int inIntf = it->second.inGate;

    pk->setTraceLabel(lastLabel);
    pk->setKind(1);
    send(pk, "out", inIntf);
}

//查询对应的溯源标签
//或是生成新的溯源标签
int Router::findTraceLabel(int inGate, int lastLabel) {
    int currentLabel = 0;

    TracebackTable::iterator inIt = tracebackTable.find(inGate);
    if (inIt != tracebackTable.end()) {
        map<int, int> temp = inIt->second;
        map<int,int>::iterator it = temp.find(lastLabel);
        if (it != temp.end()) {
            currentLabel = it->second;
        } else {
            // TLT build
            currentLabel = currentLabelIndex++;
            tracebackTable.find(inGate)->second.insert(make_pair(lastLabel, currentLabel));
            // THT build
            TraceLabelPojo* tlp = new TraceLabelPojo(lastLabel, inGate);
            tracebackHelper.insert(make_pair(currentLabel, *tlp));
        }
    } else {
        // TLT build
        currentLabel = currentLabelIndex++;
        map<int, int> mm;
        mm.insert(make_pair(lastLabel, currentLabel));
        tracebackTable.insert(make_pair(inGate, mm));
        // THT build
        TraceLabelPojo* tlp = new TraceLabelPojo(lastLabel, inGate);
        tracebackHelper.insert(make_pair(currentLabel, *tlp));
    }

    return currentLabel;
}

// lsp尚未建立，建立lsp
int Router::buildLsp(bool isLER, string declaredAddr, string prefix, int inGate, int dstIndex, int hopCount) {

    if (flag) {
        ValidationPojo* vv = new ValidationPojo(inGate, hopCount);
        validationHelper.insert(make_pair(declaredAddr, *vv));
    }

    int outLabel = 0;
    if (isLER) {
        EV <<"have to build new lsp" << endl;
        // 从当前路由器开始建立到dst的lsp
        //首先获取出接口outGate
        RoutingTable::iterator it = rtable.find(dstIndex);
        int outGate = (*it).second;

        topo->calculateUnweightedSingleShortestPathsTo(topo->getNode(dstIndex));
        cTopology::Node *thisNode = topo->getNode(nodeIndex);
        cModule* next = thisNode->getPath(0)->getRemoteNode()->getModule();
        Router* nextRouter = dynamic_cast<Router *>(next);

        if (nextRouter != NULL) {
        // 帮助下一跳计算它的inGate
            int nextGate = thisNode->getPath(0)->getRemoteGate()->getIndex();
            outLabel = nextRouter->buildLsp(false, declaredAddr, prefix, nextGate, dstIndex, hopCount + 1);
        } else {
            //如果下一跳不为router，代表当前路由器为边界路由，无须再递归，直接设置outLabel为0
            outLabel = label++;
        }

        MPLSLabelByIP *mm = new MPLSLabelByIP(0, outLabel, outGate);
        ipCache.insert(make_pair(prefix, *mm));

        int inLabel = -outLabel;
        MPLSLabelByInLabel* nn = new MPLSLabelByInLabel(inGate, outLabel, outGate, prefix);
        labelCache.insert(make_pair(inLabel, *nn));
        return outLabel;
    }

    int inLabel = 0;
    CacheByIP::iterator ipIt = ipCache.find(prefix);
    if (ipIt != ipCache.end()) {
        inLabel = ipIt->second.inLabel;
    }
    if (inLabel == 0) {
        EV <<"have to build new lsp" << endl;
        // 从当前路由器开始建立到dst的lsp
        //首先获取出接口outGate
        RoutingTable::iterator it = rtable.find(dstIndex);
        int outGate = (*it).second;

        topo->calculateUnweightedSingleShortestPathsTo(topo->getNode(dstIndex));
        cTopology::Node *thisNode = topo->getNode(nodeIndex);
        cModule* next = thisNode->getPath(0)->getRemoteNode()->getModule();
        Router* nextRouter = dynamic_cast<Router *>(next);
        // 下一跳仍为router，需要递归，以下一跳的inLabel作为本路由的outLabel
        if (nextRouter != NULL) {
            // 帮助下一跳计算它的inGate
            int nextGate = thisNode->getPath(0)->getRemoteGate()->getIndex();
            outLabel = nextRouter->buildLsp(false, declaredAddr, prefix, nextGate, dstIndex, hopCount + 1);
        } else {
            //如果下一跳不为router，代表当前路由器为边界路由，无须再递归，直接设置outLabel为0
            outLabel = 0;
        }

        inLabel = label++;
        MPLSLabelByIP *m = new MPLSLabelByIP(inLabel, outLabel, outGate);
        //EV << m->str() << endl;
        ipCache.insert(make_pair(prefix, *m));

        MPLSLabelByInLabel* mm = new MPLSLabelByInLabel(inGate, outLabel, outGate, prefix);
        labelCache.insert(make_pair(inLabel, *mm));
    }
    return inLabel;
}


