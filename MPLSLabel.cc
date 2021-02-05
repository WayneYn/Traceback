
#include <stdlib.h>
#include <string>


using namespace std;


class MPLSLabelByInLabel {
public:
    int inIf;
    int outLabel;
    int outIf;
    string dst;


    MPLSLabelByInLabel(int a, int b, int c, string s) {
        inIf = a;
        outLabel = b;
        outIf = c;
        dst.assign(s);

    }

    MPLSLabelByInLabel(const MPLSLabelByInLabel& m) {
        inIf = m.inIf;
        outIf = m.outIf;
        outLabel = m.outLabel;
        dst.assign(m.dst);
    }
};

class MPLSLabelByIP {
public:
    int inLabel;
    int outLabel;
    int outIf;


    MPLSLabelByIP(int b, int c, int d) {
        inLabel = b;
        outLabel = c;
        outIf = d;
    }

    MPLSLabelByIP(const MPLSLabelByIP& m) {
        inLabel = m.inLabel;
        outLabel = m.outLabel;
        outIf = m.outIf;
    }

    string str() {
        return "inLabel: " + to_string(inLabel) + "   outLabel: " + to_string(outLabel) + "   outIf: " + to_string(outIf);
    }
};

class TraceLabelPojo {
public:
    int lastLabel;
    int inGate;

    TraceLabelPojo(int a, int b) {
        lastLabel = a;
        inGate = b;
    }

    TraceLabelPojo(const TraceLabelPojo& m) {
        lastLabel = m.lastLabel;
        inGate = m.inGate;
    }
};

class ValidationPojo {
public:
    int intf;
    int hopCount;

    ValidationPojo(int a, int b) {
        intf = a;
        hopCount = b;
    }

    ValidationPojo(const ValidationPojo& m) {
        intf = m.intf;
        hopCount = m.hopCount;
    }
};




