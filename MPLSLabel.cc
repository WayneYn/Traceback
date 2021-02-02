
#include <stdlib.h>
#include <string>


using namespace std;


class MPLSLabelByInLabel {
public:
    int inIf;
    int outLabel;
    int outIf;


    MPLSLabelByInLabel(int a, int b, int c) {
        inIf = a;
        outLabel = b;
        outIf = c;

    }

    MPLSLabelByInLabel(const MPLSLabelByInLabel& m) {
        inIf = m.inIf;
        outIf = m.outIf;
        outLabel = m.outLabel;
    }

    string str() {
            string s = " inIf: " + to_string(inIf) + " outLabel: " + to_string(outLabel) + " outIf: " + to_string(outIf);
            return s;
        }
};

class MPLSLabelByIP {
public:
    int inLabel;
    int inIf;
    int outLabel;
    int outIf;


    MPLSLabelByIP(int a, int b, int c, int d) {
        inLabel = a;
        inIf = b;
        outLabel = c;
        outIf = d;
    }

    MPLSLabelByIP(const MPLSLabelByIP& m) {
        inLabel = m.inLabel;
        inIf = m.inIf;
        outIf = m.outIf;
        outLabel = m.outLabel;
    }

    string str() {
        string s = "inLabel: " + to_string(inLabel) + " inIf: " + to_string(inIf) + " outLabel: " + to_string(outLabel) + " outIf: " + to_string(outIf);
        return s;
    }
};

class TraceLabelPojo {
public:
    int inLabel;
    int lastLabel;

    TraceLabelPojo(int a, int b) {
        inLabel = a;
        lastLabel = b;
    }

    TraceLabelPojo(const TraceLabelPojo& m) {
        inLabel = m.inLabel;
        lastLabel = m.lastLabel;
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




