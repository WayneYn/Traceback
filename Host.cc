
#include <omnetpp.h>
#include "TraceUtils.cc"
#include "Packet_m.h"
#include <string>

using namespace omnetpp;
using namespace std;

class Host: public cSimpleModule{
private:
    int nodeIndex;
    const int minus = 9;
    std::string ip;
    cPar *sendIATime;
    int destIndex = 10;
    // state
    cMessage *generatePacket;

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
public:
    Host();
    ~Host();
};

Define_Module(Host)

Host::Host()
{
    generatePacket = nullptr;
}

Host::~Host()
{
    cancelAndDelete(generatePacket);
}

void Host::initialize() {
    nodeIndex = par("nodeIndex");
    cPar *ipPar = &par("IP");
    ip = ipPar->str();
    ip = ip.substr(1, ip.size() - 2);

    Singleton& instance = Singleton::get_instance();
    instance.setIndex(ip, nodeIndex);
    instance.setIp(nodeIndex, ip);
    instance.addHost(nodeIndex);

    generatePacket = new cMessage("nextPacket");
    sendIATime = &par("sendIaTime");  // volatile parameter
    scheduleAt(simTime() + sendIATime->doubleValue(), generatePacket);
}

void Host::handleMessage(cMessage *msg) {
    if (msg == generatePacket) {
        if (destIndex == nodeIndex) {
            destIndex++;
        }
        if (destIndex == 21) {
            return;
        }

        string destAddress = Singleton::get_instance().getIp(destIndex);
//        string destAddress = Singleton::get_instance().getRandomHost();
//        int destIndex = Singleton::get_instance().getIndex(destAddress);
//        while (nodeIndex == destIndex) {
//            destAddress = Singleton::get_instance().getRandomHost();
//            destIndex = Singleton::get_instance().getIndex(destAddress);
//        }

        string srcAddress = Singleton::get_instance().getRandomHost();

        EV <<"****True src address: " << ip <<endl;
        EV <<"****Current src address: " << srcAddress << endl;

        char pkname[40];
        sprintf(pkname, "pk-h%d-to-h%d", nodeIndex - minus, destIndex - minus);

        Packet *pk = new Packet(pkname);
        pk->setKind(0);
        pk->setDestAddr(destAddress.data());
        pk->setDeclaredAddr(ip.data());
        pk->setSrcAddr(srcAddress.data());
        send(pk, "out", 0);
        if (hasGUI()) {
            bubble("Generating packet...");
        }
        destIndex++;
        scheduleAt(simTime() + sendIATime->doubleValue(), generatePacket);
    } else {
        if (hasGUI()) {
            bubble("Arrived");
        }
//        Packet *pk = check_and_cast<Packet *>(msg);
//        pk->setKind(1);
//        send(pk, "out", 0);

    }
}
