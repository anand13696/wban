package wban;

network Graph
{
    submodules:
        eeg: Sensor {
            @display("p=200,45");
        }
        ecg: Sensor {
            @display("p=250,265");
        }
        pulse: Sensor {
            @display("p=30,430");
        }
        bnc: Bnc {
            @display("p=250,350");
        }
        ms1: Sensor {
            @display("p=130,775");
        }
        ms2: Sensor {
            @display("p=270,775");
        }
    connections:
        pulse.port <--> bnc.port++;
        ecg.port <--> bnc.port++;
        eeg.port <--> bnc.port++;
        ms1.port <--> bnc.port++;
        ms2.port <--> bnc.port++;
}
