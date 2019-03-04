package wban;

import ned.IBidirectionalChannel;


@license(LGPL);
//
// TODO documentation
//
network Network
{
    @display("bgb=489,348;bgl=2");
    types:
    submodules:
        Pulse: <paramName> like IBidirectionalChannel {
            @display("p=79,142;is=s");
        }
        EEG: <paramName> like IBidirectionalChannel {
            @display("p=221,32");
        }
        ECG: <paramName> like IBidirectionalChannel {
            @display("p=272,108");
        }
        Sink: <paramName> like IBidirectionalChannel {
            @display("p=272,188");
        }
        MotionSensor: <paramName> like IBidirectionalChannel {
            @display("p=190,304");
        }
        MotionSensor1: <paramName> like IBidirectionalChannel {
            @display("p=283,304");
        }
    connections:
        EEG.a <--> Sink.a;
        ECG.b <--> Sink.b;
}

