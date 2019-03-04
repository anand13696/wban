/*
 * graph.cpp
 *
 *  Created on: Feb 26, 2019
 *      Author: USER
 */
#include <string.h>
#include <omnetpp.h>
#include "graph.h"

using namespace omnetpp;

class GraphNode : public cSimpleModule
{
  private:
    long numSent;
    long numReceived;
    cHistogram hopCountStats;
    cOutVector hopCountVector;

  protected:
    virtual TicTocMsg15 *generateMessage();
    virtual void forwardMessage(TicTocMsg15 *msg);
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;

    // The finish() function is called by OMNeT++ at the end of the simulation:
    virtual void finish() override;
};

Define_Module(GraphNode);

void GraphNode::initialize()
{
    // Initialize variables
    numSent = 0;
    numReceived = 0;
    WATCH(numSent);
    WATCH(numReceived);

    hopCountStats.setName("hopCountStats");
    hopCountVector.setName("HopCount");

    // Module 0 sends the first message
    if (getIndex() == 0) {
        // Boot the process scheduling the initial message as a self-message.
        TicTocMsg15 *msg = generateMessage();
        scheduleAt(0.0, msg);
    }
}

void GraphNode::handleMessage(cMessage *msg)
{
    TicTocMsg15 *ttmsg = check_and_cast<TicTocMsg15 *>(msg);

    if (ttmsg->getDestination() == getIndex()) {
        // Message arrived
        int hopcount = ttmsg->getHopCount();
        EV << "Message " << ttmsg << " arrived after " << hopcount << " hops.\n";
        bubble("ARRIVED, starting new one!");

        // update statistics.
        numReceived++;
        hopCountVector.record(hopcount);
        hopCountStats.collect(hopcount);

        delete ttmsg;

        // Generate another one.
        EV << "Generating another message: ";
        TicTocMsg15 *newmsg = generateMessage();
        EV << newmsg << endl;
        forwardMessage(newmsg);
        numSent++;
    }
    else {
        // We need to forward the message.
        forwardMessage(ttmsg);
    }
}

TicTocMsg15 *GraphNode::generateMessage()
{
    // Produce source and destination addresses.
    int src = getIndex();
    int n = getVectorSize();
    int dest = intuniform(0, n-2);
    if (dest >= src)
        dest++;

    char msgname[20];
    sprintf(msgname, "tic-%d-to-%d", src, dest);

    // Create message object and set source and destination field.
    TicTocMsg15 *msg = new TicTocMsg15(msgname);
    msg->setSource(src);
    msg->setDestination(dest);
    return msg;
}

void GraphNode::forwardMessage(TicTocMsg15 *msg)
{
    // Increment hop count.
    msg->setHopCount(msg->getHopCount()+1);

    // Same routing as before: random gate.
    int n = gateSize("gate");
    int k = intuniform(0, n-1);

    EV << "Forwarding message " << msg << " on gate[" << k << "]\n";
    send(msg, "gate$o", k);
}

void GraphNode::finish()
{
    // This function is called by OMNeT++ at the end of the simulation.
    EV << "Sent:     " << numSent << endl;
    EV << "Received: " << numReceived << endl;
    EV << "Hop count, min:    " << hopCountStats.getMin() << endl;
    EV << "Hop count, max:    " << hopCountStats.getMax() << endl;
    EV << "Hop count, mean:   " << hopCountStats.getMean() << endl;
    EV << "Hop count, stddev: " << hopCountStats.getStddev() << endl;

    recordScalar("#sent", numSent);
    recordScalar("#received", numReceived);

    hopCountStats.recordAs("hop count");
}
