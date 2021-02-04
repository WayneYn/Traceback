
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
    int outLabel;
    int outIf;


    MPLSLabelByIP(int c, int d) {
        outLabel = c;
        outIf = d;
    }

    MPLSLabelByIP(const MPLSLabelByIP& m) {
        outLabel = m.outLabel;
        outIf = m.outIf;
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




