import ned.DatarateChannel;


//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//
//
// TODO documentation
//
simple Host
{
    parameters:
        int nodeIndex;
        string IP;
        volatile double sendIaTime @unit(s) = default(exponential(1s));
        @display("i=device/pc2");

    gates:
        output out[];
        input in[];
}

//
// TODO documentation
//
simple Switch
{
    parameters:
        int nodeIndex;
        @display("i=device/switch");

    gates:
        output out[];
        input in[];
}

//
// TODO documentation
//
simple Router
{
    parameters:
        int nodeIndex;
        bool flag;
        @display("i=old/srouter");

    gates:
        output out[];
        input in[];
}

//
// TODO documentation
//
network Network
{
    @display("bgb=1144.81,610.645");
    submodules:
        r1: Router {
            parameters:
                nodeIndex = 0;
                flag = false;
                @display("p=137.425,376.42502");
        }
        r2: Router {
            parameters:
                nodeIndex = 1;
                flag = false;
                @display("p=278.435,393.155");
        }
        r3: Router {
            parameters:
                nodeIndex = 2;
                flag = false;
                @display("p=225.85501,249.755");
        }
        r4: Router {
            parameters:
                nodeIndex = 3;
                flag = false;
                @display("p=507.875,267.68");
        }
        r5: Router {
            parameters:
                nodeIndex = 4;
                flag = false;
                @display("p=689.515,285.605");
        }

        r6: Router {
            parameters:
                nodeIndex = 5;
                flag = false;
                @display("p=507.875,450.515");
        }

        r7: Router {
            parameters:
                nodeIndex = 6;
                flag = false;
                @display("p=705.05,384.79");
        }

        r8: Router {
            parameters:
                nodeIndex = 7;
                flag = false;
                @display("p=705.05,494.73");
        }

        r9: Router {
            parameters:
                nodeIndex = 8;
                flag = false;
                @display("p=339.38,193.59001");
        }

        r10: Router {
            parameters:
                nodeIndex = 9;
                flag = false;
                @display("p=464.855,142.205");
        }

        r11: Router {
            parameters:
                nodeIndex = 10;
                flag = false;
                @display("p=726.56,184.03");
        }

        r12: Router {
            parameters:
                nodeIndex = 11;
                flag = false;
                @display("p=852.03503,127.865005");
        }

        r13: Router {
            parameters:
                nodeIndex = 12;
                flag = false;
                @display("p=841.28,249.755");
        }

        r14: Router {
            parameters:
                nodeIndex = 13;
                flag = false;
                @display("p=905.81,357.305");
        }

        r15: Router {
            parameters:
                nodeIndex = 14;
                flag = false;
                @display("p=225.85501,114.72");
        }


        h1: Host {
            parameters:
                nodeIndex = 15;
                IP = "10.0.0.1";
                @display("p=90.82,431.39502");
        }
        h2: Host {
            parameters:
                nodeIndex = 16;
                IP = "10.0.0.2";
                @display("p=362.08502,431.39502");
        }
        h3: Host {
            parameters:
                nodeIndex = 17;
                IP = "10.0.1.1";
                @display("p=112.33,230.63501");
        }
        h4: Host {
            parameters:
                nodeIndex = 18;
                IP = "10.0.2.1";
                @display("p=461.27002,531.775");
        }
        h5: Host {
            parameters:
                nodeIndex = 19;
                IP = "10.0.3.1";
                @display("p=805.43,450.515");
        }
        h6: Host {
            parameters:
                nodeIndex = 20;
                IP = "10.0.3.2";
                @display("p=679.955,568.82");
        }
        h7: Host {
            parameters:
                nodeIndex = 21;
                IP = "10.0.4.1";
                @display("p=952.41504,230.63501");
        }
        h8: Host {
            parameters:
                nodeIndex = 22;
                IP = "10.0.4.2";
                @display("p=999.02,460.075");
        }
        h9: Host {
            parameters:
                nodeIndex = 23;
                IP = "10.0.4.3";
                @display("p=970.34,96.795006");
        }
        h10: Host {
            parameters:
                nodeIndex = 24;
                IP = "10.0.4.4";
                @display("p=192.395,40.63");
        }


    connections:


        r3.out++ --> {datarate=100Mbps;} --> r1.in++;
        r1.out++ --> {datarate=100Mbps;} --> r3.in++;

        r3.out++ --> {datarate=100Mbps;}--> r9.in++;
        r9.out++ --> {datarate=100Mbps;} --> r3.in++;

        r9.out++ --> {datarate=100Mbps;}--> r10.in++;
        r10.out++ --> {datarate=100Mbps;}--> r9.in++;

        r10.out++ --> {datarate=100Mbps;}--> r4.in++;
        r4.out++ --> {datarate=100Mbps;}--> r10.in++;

        r4.out++ --> {datarate=100Mbps;}--> r5.in++;
        r5.out++ --> {datarate=100Mbps;}--> r4.in++;

        r5.out++ --> {datarate=100Mbps;}--> r7.in++;
        r7.out++ --> {datarate=100Mbps;}--> r5.in++;

        r7.out++ --> {datarate=100Mbps;}--> r8.in++;
        r8.out++ --> {datarate=100Mbps;}--> r7.in++;

        r3.out++ --> {datarate=100Mbps;}--> r2.in++;
        r2.out++ --> {datarate=100Mbps;}--> r3.in++;

        r4.out++ --> {datarate=100Mbps;}--> r6.in++;
        r6.out++ --> {datarate=100Mbps;}--> r4.in++;

        r5.out++ --> {datarate=100Mbps;}--> r11.in++;
        r11.out++ --> {datarate=100Mbps;}--> r5.in++;

        r11.out++ --> {datarate=100Mbps;}--> r12.in++;
        r12.out++ --> {datarate=100Mbps;}--> r11.in++;

        r11.out++ --> {datarate=100Mbps;}--> r13.in++;
        r13.out++ --> {datarate=100Mbps;}--> r11.in++;

        r13.out++ --> {datarate=100Mbps;}--> r14.in++;
        r14.out++ --> {datarate=100Mbps;}--> r13.in++;

        r15.out++ --> {datarate=100Mbps;}--> r9.in++;
        r9.out++ --> {datarate=100Mbps;}--> r15.in++;

        r1.out++ --> {datarate=100Mbps;}--> h1.in++;
        h1.out++ --> {datarate=100Mbps;}--> r1.in++;

        r2.out++ --> {datarate=100Mbps;}--> h2.in++;
        h2.out++ --> {datarate=100Mbps;}--> r2.in++;

        h3.out++ --> {datarate=100Mbps;}--> r3.in++;
        r3.out++ --> {datarate=100Mbps;}--> h3.in++;

        r6.out++ --> {datarate=100Mbps;}--> h4.in++;
        h4.out++ --> {datarate=100Mbps;}--> r6.in++;

        r8.out++ --> {datarate=100Mbps;}--> h6.in++;
        h6.out++ --> {datarate=100Mbps;}--> r8.in++;

        h5.out++ --> {datarate=100Mbps;}--> r7.in++;
        r7.out++ --> {datarate=100Mbps;}--> h5.in++;

        h8.out++ --> {datarate=100Mbps;}--> r14.in++;
        r14.out++ --> {datarate=100Mbps;}--> h8.in++;

        h7.out++ --> {datarate=100Mbps;}--> r13.in++;
        r13.out++ --> {datarate=100Mbps;}--> h7.in++;

        h9.out++ --> {datarate=100Mbps;}--> r12.in++;
        r12.out++ --> {datarate=100Mbps;}--> h9.in++;

        h10.out++ --> {datarate=100Mbps;}--> r15.in++;
        r15.out++ --> {datarate=100Mbps;}--> h10.in++;
        r10.out++ --> {datarate=100Mbps;}--> r11.in++;
        r11.out++ --> {datarate=100Mbps;}--> r10.in++;
        r9.out++ --> {datarate=100Mbps;}--> r4.in++;
        r4.out++ --> {datarate=100Mbps;}--> r9.in++;
}
