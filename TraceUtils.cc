
#include <omnetpp.h>
#include <stdlib.h>
#include <vector>
#include <time.h>

using namespace omnetpp;
using namespace std;


class Singleton {
public:
    ~Singleton(){

    }
    Singleton(const Singleton&)=delete;

    Singleton& operator=(const Singleton&)=delete;

    static Singleton& get_instance(){
        static Singleton instance;
        return instance;
    }

    void setIndex(string ip, int index) {
        ipToIndex[ip] = index;
    }

    int getIndex(string ip) {
        StringToIntTable::iterator it = ipToIndex.find(ip);
        int index = (*it).second;
        return index;
    }

    void setIp(int index, string ip) {
        indexToIp[index] = ip;
    }

    string getIp(int index) {
        IntToStringTable::iterator it = indexToIp.find(index);
        string ip = (*it).second;
        return ip;
    }

    void addHost(int index) {
        hostList.push_back(index);
    }

    string getRandomHost() {
        srand((unsigned)time(NULL));
        int randNum = rand() % hostList.size();
        int index = hostList.at(randNum);
        return getIp(index);
    }

private:
    typedef map<string, int> StringToIntTable;
    StringToIntTable ipToIndex;

    typedef map<int, string> IntToStringTable;
    IntToStringTable indexToIp;

    vector<int> hostList;

    Singleton(){

    }
};



