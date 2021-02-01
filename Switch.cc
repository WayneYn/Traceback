
#include <omnetpp.h>

#include "TraceUtils.cc"
#include "Packet_m.h"

using namespace omnetpp;
using namespace std;

class Switch: public cSimpleModule{
private:
    int nodeIndex;
    typedef map<int, int> RoutingTable;  // destaddr -> gateindex
    RoutingTable rtable;
protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};
Define_Module(Switch)

void Switch::initialize() {
    nodeIndex = par("nodeIndex");
    cTopology *topo = new cTopology("topo");
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
    delete topo;
}

void Switch::handleMessage(cMessage *msg) {
    //EV << "arrive gate id: " << msg->getArrivalGate()->getIndex() << endl;

    Packet *pk = check_and_cast<Packet *>(msg);
    string destAddr = pk->getDestAddr();
    int index = Singleton::get_instance().getIndex(destAddr);

    RoutingTable::iterator it = rtable.find(index);
    int outGateIndex = (*it).second;
    send(pk, "out", outGateIndex);
}

