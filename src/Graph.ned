package wban;
simple GraphNode extends Node
{
    gates:
        inout g[];
}

//simple HubNode extends Node
//{
//    parameters:
//        @display("i=device/switch");
//    gates:
//        inout g[];
//}

network Graph
{
    submodules:
        eeg: GraphNode {
            @display("p=200,45");
        }
        ecg: GraphNode {
            @display("p=250,265");
        }
        pulse: GraphNode {
            @display("p=30,430");
        }
        bnc: GraphNode {
            @display("i=device/switch","p=250,350");
        }
        ms1: GraphNode {
            @display("p=130,775");
        }
        ms2: GraphNode {
            @display("p=270,775");
        }
    connections:
        pulse.g++ <--> bnc.g++;
        ecg.g++ <--> bnc.g++;
        ms2.g++ <--> bnc.g++;
        ms1.g++ <--> bnc.g++;
        eeg.g++ <--> bnc.g++;
}