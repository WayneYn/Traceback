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
    @display("bgb=952.6,586.3");
    submodules:
        r1: Router {
            parameters:
                nodeIndex = 0;
                flag = false;
                @display("p=123.2,298.1");
        }
        r2: Router {
            parameters:
                nodeIndex = 1;
                flag = false;
                @display("p=239.79999,292.6");
        }
        r3: Router {
            parameters:
                nodeIndex = 2;
                flag = false;
                @display("p=212.29999,171.59999");
        }
        r4: Router {
            parameters:
                nodeIndex = 3;
                flag = false;
                @display("p=493.9,189.2");
        }
        r5: Router {
            parameters:
                nodeIndex = 4;
                flag = false;
                @display("p=675.39996,206.79999");
        }
        
        r6: Router {
            parameters:
                nodeIndex = 5;
                flag = false;
                @display("p=477.4,306.9");
        }

        r7: Router {
            parameters:
                nodeIndex = 6;
                flag = false;
                @display("p=691.89996,306.9");
        }

        r8: Router {
            parameters:
                nodeIndex = 7;
                flag = false;
                @display("p=691.89996,416.9");
        }

        r9: Router {
            parameters:
                nodeIndex = 8;
                flag = false;
                @display("p=325.59998,115.5");
        }

        r10: Router {
            parameters:
                nodeIndex = 9;
                flag = false;
                @display("p=451,63.8");
        }

        h1: Host {
            parameters:
                nodeIndex = 10;
                IP = "10.0.0.1";
                @display("p=45.1,375.09998");
        }
        h2: Host {
            parameters:
                nodeIndex = 11;
                IP = "10.0.0.2";
                @display("p=123.2,443.3");
        }
        h3: Host {
            parameters:
                nodeIndex = 12;
                IP = "10.0.1.1";
                @display("p=337.69998,306.9");
        }
        h4: Host {
            parameters:
                nodeIndex = 13;
                IP = "10.0.2.1";
                @display("p=282.69998,425.69998");
        }
        h5: Host {
            parameters:
                nodeIndex = 14;
                IP = "10.0.3.1";
                @display("p=451,462");
        }
        h6: Host {
            parameters:
                nodeIndex = 15;
                IP = "10.0.3.2";
                @display("p=567.6,382.8");
        }
        h7: Host {
            parameters:
                nodeIndex = 16;
                IP = "10.0.4.1";
                @display("p=709.5,106.7");
        }
        h8: Host {
            parameters:
                nodeIndex = 17;
                IP = "10.0.4.2";
                @display("p=838.19995,258.5");
        }
        h9: Host {
            parameters:
                nodeIndex = 18;
                IP = "10.0.4.3";
                @display("p=838.19995,332.19998");
        }
        h10: Host {
            parameters:
                nodeIndex = 19;
                IP = "10.0.4.4";
                @display("p=666.6,486.19998");
        }
        h11: Host {
            parameters:
                nodeIndex = 20;
                IP = "10.0.5.1";
                @display("p=782.1,455.4");
        }

    connections:


        r1.out++ --> h1.in++;
        h1.out++ --> r1.in++;
        
        r3.out++ --> r1.in++;
        r1.out++ --> r3.in++;
        
        r2.out++ --> r3.in++;
        r3.out++ --> r2.in++;
        
        r1.out++ --> h2.in++;
        h2.out++ --> r1.in++;
        
        r2.out++ --> h3.in++;
        h3.out++ --> r2.in++;
        
        r2.out++ --> h4.in++;
        h4.out++ --> r2.in++;
        
        r3.out++ --> r9.in++;
        r9.out++ --> r3.in++;
        
        r9.out++ --> r10.in++;
        r10.out++ --> r9.in++;
        
        r10.out++ --> r4.in++;
        r4.out++ --> r10.in++;
        
        r4.out++ --> r6.in++;
        r6.out++ --> r4.in++;
        
        r4.out++ --> r5.in++;
        r5.out++ --> r4.in++;
        
        r5.out++ --> r7.in++;
        r7.out++ --> r5.in++;
        
        r7.out++ --> r8.in++;
        r8.out++ --> r7.in++;
        
        r6.out++ --> h5.in++;
        h5.out++ --> r6.in++;
        
        r6.out++ --> h6.in++;
        h6.out++ --> r6.in++;
        
        r8.out++ --> h10.in++;
        h10.out++ --> r8.in++;
        
        r8.out++ --> h11.in++;
        h11.out++ --> r8.in++;
        
        r7.out++ --> h9.in++;
        h9.out++ --> r7.in++;
        
        r7.out++ --> h8.in++;
        h8.out++ --> r7.in++;
        
        r5.out++ --> h7.in++;
        h7.out++ --> r5.in++;
}
