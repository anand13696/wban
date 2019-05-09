/*
 * Bnc.cc
 *
 *  Created on: Mar 25, 2019
 *      Author: USER
 */

#include "WbanPacket_m.h"
#include <algorithm>
#include <stdlib.h>
using namespace omnetpp;

#define STACKSIZE    16384

/**
 * Simulates a Bnc between sensor and processor
 */
class Bnc : public cSimpleModule
{
  public:
    Bnc() : cSimpleModule(STACKSIZE) {}
    virtual void activity() override;
};

Define_Module(Bnc);

void Bnc::activity()
{
    double bncPosX = 250, bncPosY = 350; //Setting default value of the BNC
    cTopology *topo; // temp variable to access initial location of the nodes
    topo = new cTopology("topo");
    topo->extractByNedTypeName(cStringTokenizer("node.Node").asVector());
    int numOfNodes = topo->getNumNodes();
    sensorLocation *nodeLocation = new sensorLocation[numOfNodes];
    double dist[numOfNodes]; //Array to store the distance between the node and the BNC
    double Eav[numOfNodes]; //Array to store the energy available of the sensor at any moment
    double UF[numOfNodes]; //Array to store the utility factor of each node
    double uf[numOfNodes], xi[numOfNodes]; //Arrays to store the factors to obtain new position of BNC
    double pathLossExpo = 1.9; //The path loss exponent constant
    double ampCoeff = 12 * pow(10, -6); //The Energy dissipated by the transmit amplifier, it is constant for a sensor and hence amplifierCoefficient

    for (int i = 0; i < numOfNodes; i++) {

        VirtualMobilityManager *nodeMobilityModule = check_and_cast<VirtualMobilityManager*> (topo->getNode(i)->getModule()->getSubmodule("MobilityManager"));
        nodeLocation[i] = nodeMobilityModule->getLocation();

        EV << "Node Location[" << i << "] - x " << nodeLocation[i].x << " y " << nodeLocation[i].y << endl;

        dist[i] = sqrt(pow(nodeLocation[i].x-bncPosX,2) +pow(nodeLocation[i].y-bncPosY,2));

        /*
         * Available Energy = initialEnergyOfTheSensor - energyTransmittedBySensor
         * energyTransmittedBySensor = Etx_elec * k + Eamp * k * dist^pathLossExpo
         * Etx_elec - Energy dissipated by the radio to run the circuitry of the transmitter in nJ/bit (Taken 2.5)
         * Eamp - Energy dissipated by the transmit amplifier in nJ/bit-m^pathLossExpo (Taken 12*10^-6)
         * k - number of bits (Taken 1 to calculate the energy taken by each bit)
         */
        Eav[i] = nodeLocation[i].energy - 2.5 - ampCoeff * pow(dist[0], pathLossExpo);
        UF[i] = Eav[i] / pow(dist[i], pathLossExpo);
    }

    double maxUF = UF[0];
    for (int i = 1; i < numOfNodes; i++) {
        if(UF[i] > maxUF)
            maxUF = UF[i];
    }

    for (int i = 0; i < numOfNodes; i++) {
        uf[i] = (maxUF - UF[i]) / maxUF;
    }
    double maxuf = uf[0], tmp = 0;
    for (int i = 1; i < 5; i++) {
        if(uf[i] > maxuf)
            maxuf = uf[i];
    }

    for (int i = 0; i < 5; i++) {
            xi[i] = uf[i] / maxuf;
    }

    for (int i = 0; i < numOfNodes; i++) {
        tmp += xi[i] * nodeLocation[i].x;
    }
    bncPosX = tmp / numOfNodes;

    tmp = 0;
    for (int i = 0; i < numOfNodes; i++) {
            tmp += xi[i] * nodeLocation[i].y;
    }
    bncPosY = tmp / numOfNodes;

    simtime_t pkDelay = 1 / (double)par("pkRate");
    int queueMaxLen = (int)par("queueMaxLen");
    cQueue queue("queue");
    for ( ; ; ) {
        // receive msg
        cMessage *msg;
        if (!queue.isEmpty())
            msg = (cMessage *)queue.pop();
        else
            msg = receive();

        // model processing delay; packets that arrive meanwhile are queued
        waitAndEnqueue(pkDelay, &queue);

        // send msg to destination
        WbanPacket *pk = check_and_cast<WbanPacket *>(msg);
        int dest = pk->getDestAddress();
        EV << "Relaying msg to addr=" << dest << "\n";
        send(msg, "port$o", dest);

        // display status: normal=queue empty, yellow=queued packets; red=queue overflow
        int qLen = queue.getLength();
        if (hasGUI())
            getDisplayString().setTagArg("i", 1, qLen == 0 ? "" : qLen < queueMaxLen ? "gold" : "red");

        // model finite queue size
        while (queue.getLength() > queueMaxLen) {
            EV << "Buffer overflow, discarding " << queue.front()->getName() << endl;
            delete queue.pop();
        }
    }
}

/*
 * Section of code to calculate the energy.
 */
