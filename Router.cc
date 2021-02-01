
#include <omnetpp.h>
#include <typeinfo>
#include "TraceUtils.cc"
#include "Packet_m.h"
#include "MPLSLabel.cc"
#include <vector>

using namespace omnetpp;
using namespace std;

static int label = 20;

class Router: public cSimpleModule{

private:
    int nodeIndex;

    int currentLabelIndex = 4;

    typedef map<int, int> RoutingTable;  // destaddr -> gateindex
    RoutingTable rtable;

    typedef map<int, MPLSLabelByInLabel> CacheByInLabel;
    CacheByInLabel labelCache;

    typedef map<string, vector<int>> CacheByIP;
    CacheByIP ipCache;

    typedef map<int, map<int, int>> TracebackTable;
    TracebackTable tracebackTable;

    typedef map<int, TraceLabelPojo> TracebackHelper;
    TracebackHelper tracebackHelper;

    int buildLsp(string prefix, int inGate, int dstIndex);

    int findTraceLabel(int forwardLabel, int lastLabel);

    cTopology *topo;

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
};
Define_Module(Router)

void Router::finish() {
    delete topo;
}

void Router::initialize() {
    nodeIndex = par("nodeIndex");
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
	
	if (pk->getKind() == 0) {
		string destAddr = pk->getDestAddr();
		//string prefix = destAddr.substr(0, destAddr.size() - 1).append("*");

		int dstIndex = Singleton::get_instance().getIndex(destAddr);

		int inLabel = pk->getForwardLabel();
		// inlabelΪ0�����������·��������δ�����ǩ
		if (inLabel == 0) {
			//CacheByIP::iterator it = ipCache.find(prefix);
			CacheByIP::iterator it = ipCache.find(destAddr);
			if (it != ipCache.end()) {
				for (auto inL : it->second) {
					CacheByInLabel::iterator it = labelCache.find(inL);
					if (inGate == it->second.inIf) {
						EV << "find same lsp" <<endl;
						inLabel = inL;
						break;
					}
				}
			}
			// inLabel��δ0.����ǰlsp��δ���������Ƚ���lsp���ַ���ǩ
			if (inLabel == 0) {
				EV <<"no lsp available, need to set lsp first" << endl;
				//inLabel = buildLsp(prefix, inGate, dstIndex);
				inLabel = buildLsp(destAddr, inGate, dstIndex);

			}
		}

		CacheByInLabel::iterator it = labelCache.find(inLabel);
		int outGateIndex = it->second.outIf;
		pk->setForwardLabel(it->second.outLabel);

		int lastLabel = pk->getTraceLabel();
		if (lastLabel == 0) {
			pk->setTraceLabel(3);
		} else {
			pk->setTraceLabel(findTraceLabel(inLabel, lastLabel));
		}
		EV << "TraceBack Label: " << pk->getTraceLabel()<< endl;

		send(pk, "out", outGateIndex);
	} else {
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
		send(pk, "out", inIntf);
	}
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
int Router::buildLsp(string prefix, int inGate, int dstIndex) {
    int inLabel = 0;
    CacheByIP::iterator ipIt = ipCache.find(prefix);
    // �����ǰcache���ж�Ӧ���棬�����Ѿ������˴ӵ�ǰ·������dst��lsp�����Ժϲ���ǩ
    if (ipIt != ipCache.end()) {
        EV <<"try to find same lsp in buildLsp" << endl;
        vector<int> list = ipIt->second;
        for (auto inL : list) {
            CacheByInLabel::iterator it = labelCache.find(inL);
            if (inGate == it->second.inIf) {
                EV <<"find same lsp in buildLsp" << endl;
                inLabel = inL;
                break;
            }
        }
    } else {
        EV <<"insert new vector"<<endl;
		vector<int> vv;
		ipCache.insert(make_pair(prefix, vv));
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
            outLabel = nextRouter->buildLsp(prefix, nextGate, dstIndex);
        } else {
            //�����һ����Ϊrouter������ǰ·����Ϊ�߽�·�ɣ������ٵݹ飬ֱ������outLabelΪ0
            outLabel = 0;
        }

        inLabel = label++;
        ipCache.find(prefix)->second.push_back(inLabel);

        MPLSLabelByInLabel* mm = new MPLSLabelByInLabel(inGate, outLabel, outGate);
        labelCache.insert(make_pair(inLabel, *mm));
    }
    return inLabel;
}


