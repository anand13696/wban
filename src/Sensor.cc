/*
 * Sensor.cc
 *
 *  Created on: Mar 25, 2019
 *      Author: USER
 */

#include "WbanPacket_m.h"

using namespace omnetpp;

#define STACKSIZE    16384

/**
 * Sensor computer; see NED file for more info
 */
class Sensor : public cSimpleModule
{
  public:
    Sensor() : cSimpleModule(STACKSIZE) {}
    virtual void activity() override;
};

Define_Module(Sensor);


void Sensor::activity()
{
    // query module parameters
    simtime_t timeout = par("timeout");
    cPar& connectionIaTime = par("connIaTime");
    cPar& queryIaTime = par("queryIaTime");
    cPar& numQuery = par("numQuery");

    // Sensor sends connection request and receives connection acknowledgement after which it sends data to the BNC.
    // When the sensor wants to disconnect it sends a disconnection request and receives a disconnection acknowledgement
    WbanPacket *connReq, *connAck, *discReq, *discAck;
    WbanDataPacket *query, *answer;
    int actNumQuery = 0, i = 0;
    WATCH(actNumQuery);
    WATCH(i);

    // assign address: index of Switch's gate to which we are connected
    int ownAddr = gate("port$o")->getNextGate()->getIndex();
    int serverAddr = gate("port$o")->getNextGate()->size()-1;
    int serverprocId = 0;
    WATCH(ownAddr);
    WATCH(serverAddr);
    WATCH(serverprocId);

    for ( ; ; ) {
        if (hasGUI())
            getDisplayString().setTagArg("i", 1, "");

        // keep an interval between subsequent connections
        wait((double)connectionIaTime);

        if (hasGUI())
            getDisplayString().setTagArg("i", 1, "green");

        // connection setup
        EV << "sending WBAN_CONN_REQ\n";
        connReq = new WbanPacket("WBAN_CONN_REQ", WBAN_CONN_REQ);
        connReq->setSrcAddress(ownAddr);
        connReq->setDestAddress(serverAddr);
        connReq->setPriority(1);
        send(connReq, "port$o");

        EV << "waiting for WBAN_CONN_ACK\n";
        connAck = (WbanPacket *)receive(timeout);
        if (connAck == nullptr)
            goto broken;
        serverprocId = connAck->getServerProcId();
        EV << "got WBAN_CONN_ACK, my server process is ID=" << serverprocId << endl;
        delete connAck;

        if (hasGUI()) {
            getDisplayString().setTagArg("i", 1, "gold");
            bubble("Connected!");
        }

        // communication
        actNumQuery = (long)numQuery;
        for (i = 0; i < actNumQuery; i++) {
            EV << "sending DATA(query)\n";
            query = new WbanDataPacket("DATA(query)", WBAN_DATA);
            query->setSrcAddress(ownAddr);
            query->setDestAddress(serverAddr);
            query->setServerProcId(serverprocId);
            query->setPayLoad("query");
            send(query, "port$o");

            EV << "waiting for DATA(result)\n";
            answer = (WbanDataPacket *)receive(timeout);
            if (answer == nullptr)
                goto broken;
            EV << "got DATA(result)\n";
            delete answer;

            wait((double)queryIaTime);
        }

        if (hasGUI())
            getDisplayString().setTagArg("i", 1, "blue");

        // connection teardown
        EV << "sending WBAN_DISC_REQ\n";
        discReq = new WbanPacket("WBAN_DISC_REQ", WBAN_DISC_REQ);
        discReq->setSrcAddress(ownAddr);
        discReq->setDestAddress(serverAddr);
        discReq->setServerProcId(serverprocId);
        send(discReq, "port$o");

        EV << "waiting for WBAN_DISC_ACK\n";
        discAck = (WbanPacket *)receive(timeout);
        if (discAck == nullptr)
            goto broken;
        EV << "got WBAN_DISC_ACK\n";
        delete discAck;

        if (hasGUI())
            bubble("Disconnected!");

        continue;

        // error handling
      broken:
        EV << "Timeout, connection broken!\n";
        if (hasGUI())
            bubble("Connection broken!");
    }
}

