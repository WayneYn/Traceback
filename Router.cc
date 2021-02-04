
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

    typedef map<string, map<int, int>> CacheByIP;
    CacheByIP ipCache;

    typedef map<int, map<int, int>> TracebackTable;
    TracebackTable tracebackTable;

    typedef map<int, TraceLabelPojo> TracebackHelper;
    TracebackHelper tracebackHelper;

    typedef map<string, ValidationPojo> ValidationHelper;
    ValidationHelper validationHelper;

    int buildLsp(string src, string prefix, int inGate, int dstIndex, int hopCount);

    int findTraceLabel(int forwardLabel, int lastLabel);

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
    cout << setw(20) <<"inGate" << setw(20) << "inLabel" << setw(20) << "dstAddr" << setw(20) << "outGate" << setw(20) << "outLabel" <<endl;
    CacheByInLabel::iterator iter = labelCache.begin();
    while (iter != labelCache.end()) {
        MPLSLabelByInLabel temp = iter->second;
        cout<< setw(20) << temp.inIf << setw(20) << iter->first << setw(20) <<temp.dst << setw(20) <<temp.outIf << setw(20) <<temp.outLabel << endl;
        iter++;
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

    // �õ���ǰ�ڵ�
    cTopology::Node *thisNode = topo->getNode(nodeIndex);

    // ��̬·������
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

        //�õ����·������¼��·�ɱ���
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

		// inlabelΪ0�����������·��������δ�����ǩ
		if (inLabel == 0) {
			//CacheByIP::iterator it = ipCache.find(prefix);
			CacheByIP::iterator it = ipCache.find(destAddr);
			if (it == ipCache.end()) {
			    map<int, int> mm;
			    ipCache.insert(make_pair(destAddr, mm));
			} else {
			    map<int, int> temp = it->second;
			    map<int, int>::iterator ii = temp.find(inGate);
			    if (ii != temp.end()) {
			        EV << "find same lsp" <<endl;
			        inLabel = ii->second;
			    }
			}

			// inLabel��δ0.����ǰlsp��δ���������Ƚ���lsp���ַ���ǩ
			if (inLabel == 0) {
				EV <<"no lsp available, need to set lsp first" << endl;
				//inLabel = buildLsp(prefix, inGate, dstIndex);
				inLabel = buildLsp(declaredAddr, destAddr, inGate, dstIndex, 1);

			}
		}
		EV << "InLabel: " << inLabel << endl;
		CacheByInLabel::iterator it = labelCache.find(inLabel);
		int outGateIndex = it->second.outIf;
		pk->setForwardLabel(it->second.outLabel);
		EV << "OutLabel: " << pk->getForwardLabel() << endl;

		int lastLabel = pk->getTraceLabel();
		if (lastLabel == 0) {
			pk->setTraceLabel(3);
		} else {
			pk->setTraceLabel(findTraceLabel(inLabel, lastLabel));
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
        return;
    }
    TracebackHelper::iterator it = tracebackHelper.find(currentLabel);
    int lastLabel = it->second.lastLabel;
    int inLabel = it->second.inLabel;
    CacheByInLabel::iterator labelIt = labelCache.find(inLabel);
    int inIntf = labelIt->second.inIf;

    pk->setTraceLabel(lastLabel);
    pk->setKind(1);
    send(pk, "out", inIntf);
}

//��ѯ��Ӧ����Դ��ǩ
//���������µ���Դ��ǩ
int Router::findTraceLabel(int inLabel, int lastLabel) {
    int currentLabel = 0;

    TracebackTable::iterator inIt = tracebackTable.find(inLabel);
    if (inIt != tracebackTable.end()) {
        map<int, int> temp = inIt->second;
        map<int,int>::iterator it = temp.find(lastLabel);
        if (it != temp.end()) {
            currentLabel = it->second;
        } else {
            currentLabel = currentLabelIndex++;
            temp.insert(make_pair(lastLabel, currentLabel));
            TraceLabelPojo* tlp = new TraceLabelPojo(inLabel, lastLabel);
            tracebackHelper.insert(make_pair(currentLabel, *tlp));
        }
    } else {
        currentLabel = currentLabelIndex++;
        map<int, int> mm;
        mm.insert(make_pair(lastLabel, currentLabel));
        tracebackTable.insert(make_pair(inLabel, mm));
        TraceLabelPojo* tlp = new TraceLabelPojo(inLabel, lastLabel);
        tracebackHelper.insert(make_pair(currentLabel, *tlp));
    }

    return currentLabel;
}

// lsp��δ����������lsp
int Router::buildLsp(string declaredAddr, string prefix, int inGate, int dstIndex, int hopCount) {

    if (flag) {
        ValidationPojo* vv = new ValidationPojo(inGate, hopCount);
        validationHelper.insert(make_pair(declaredAddr, *vv));
    }

    int inLabel = 0;
    CacheByIP::iterator ipIt = ipCache.find(prefix);
    if (ipIt == ipCache.end()) {
        map<int, int> mm;
        ipCache.insert(make_pair(prefix, mm));
    } else {
        // �����ǰcache���ж�Ӧ���棬�����Ѿ������˴ӵ�ǰ·������dst��lsp�����Ժϲ���ǩ
        EV <<"try to find same lsp in buildLsp" << endl;
        map<int, int> list = ipIt->second;
        map<int, int>::iterator listIt = list.find(inGate);
        if (listIt != list.end()) {
            inLabel = listIt->second;
            EV <<"*************************find same lsp in buildLsp" << endl;
        }
    }


    if (inLabel == 0) {
        EV <<"have to build new lsp" << endl;
        // �ӵ�ǰ·������ʼ������dst��lsp
        //���Ȼ�ȡ���ӿ�outGate
        RoutingTable::iterator it = rtable.find(dstIndex);
        int outGate = (*it).second;

        topo->calculateUnweightedSingleShortestPathsTo(topo->getNode(dstIndex));
        cTopology::Node *thisNode = topo->getNode(nodeIndex);
        cModule* next = thisNode->getPath(0)->getRemoteNode()->getModule();
        Router* nextRouter = dynamic_cast<Router *>(next);
        int outLabel;
        // ��һ����Ϊrouter����Ҫ�ݹ飬����һ����inLabel��Ϊ��·�ɵ�outLabel
        if (nextRouter != NULL) {
            // ������һ����������inGate
            int nextGate = thisNode->getPath(0)->getRemoteGate()->getIndex();
            outLabel = nextRouter->buildLsp(declaredAddr, prefix, nextGate, dstIndex, hopCount + 1);
        } else {
            //�����һ����Ϊrouter������ǰ·����Ϊ�߽�·�ɣ������ٵݹ飬ֱ������outLabelΪ0
            outLabel = 0;
        }

        inLabel = label++;
        ipCache.find(prefix)->second.insert(make_pair(inGate, inLabel));

        MPLSLabelByInLabel* mm = new MPLSLabelByInLabel(inGate, outLabel, outGate, prefix);
        labelCache.insert(make_pair(inLabel, *mm));
    }
    return inLabel;
}


