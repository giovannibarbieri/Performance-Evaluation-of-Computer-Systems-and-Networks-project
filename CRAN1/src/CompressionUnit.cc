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

#include "CompressionUnit.h"
#include "myPacket_m.h"

Define_Module(CompressionUnit);

void CompressionUnit::initialize()
{
    X = par("X");
    if(X > MAX_COMPRESSION){
        throw cRuntimeError("X cannot be higher than %f", MAX_COMPRESSION);
    }
    // Initialize timers
    timer1 = new cMessage("timer-proc-1");
    timer2 = new cMessage("timer-proc-2");
    // Initialize queue
    queue = new cQueue("compression-unit-queue");
    // Compute value of S
    S = 0.07 * X;
    if(getSystemModule()->par("debugMode"))
        EV_DEBUG << "S : " << S << endl;
    // Register signal
    queueOccupancyCUSignal = registerSignal("queueOccupancyCU");
    responseTimeCUSignal = registerSignal("responseTimeCU");
}

void CompressionUnit::handleMessage(cMessage *msg)
{
    // MSG TYPES: timer (because of a compression ended) or packet to be compressed
    if(msg->isSelfMessage()){
        handleCompressionEnded(msg);
    } else if(getX() == 0){ // compression ratio is 0 so i can directly send message out to output interface
        send(msg, "out");
    } else{
        emit(queueOccupancyCUSignal, curQueueOccupancy);
        handlePktToBeCompressed(check_and_cast<MyPacket *>(msg));
        pkt_cnt++;
    }
}

void CompressionUnit::handleCompressionEnded(cMessage *msg){
    if(msg == timer1){
        proc1Busy = false;
        emit(responseTimeCUSignal, (simTime() - pkt1->getArrivalTime()));
        send(pkt1, "out");
        pkt1 = nullptr;
    } else {
        proc2Busy = false;
        emit(responseTimeCUSignal, (simTime() - pkt2->getArrivalTime()));
        send(pkt2, "out");
        pkt2 = nullptr;
    }
    if(!queue->isEmpty()){
        MyPacket * pkt = check_and_cast<MyPacket *>(queue->pop());
        curQueueOccupancy -= pkt->getDimension();
        handlePktToBeCompressed(pkt);
    }
}

void CompressionUnit::handlePktToBeCompressed(MyPacket *pkt){
    // Change size of the pkt
    pkt->setDimension(round(pkt->getDimension() * (1 - X)));
    if(!proc1Busy){
        proc1Busy = true;
        pkt1 = pkt;
        scheduleAt(simTime() + S, timer1);
        pkt_cnt1++;
    } else if(!proc2Busy){
        proc2Busy = true;
        pkt2 = pkt;
        scheduleAt(simTime() + S, timer2);
        pkt_cnt2++;
    } else { // Processors are both busy : pkt is queued
        queue->insert(pkt);
        curQueueOccupancy += pkt->getDimension();
        if(getSystemModule()->par("debugMode")){
            EV_DEBUG << "(CU) ID:" << pkt->getId() << " Packet queued " << endl;
        }
    }
}

CompressionUnit::~CompressionUnit(){
    // Delete all objects in heap
    cancelAndDelete(timer1);
    cancelAndDelete(timer2);
    if(pkt1 != nullptr)
        delete pkt1;
    if(pkt2 != nullptr)
        delete pkt2;
    while(!queue->isEmpty())
        delete queue->pop();
    delete queue;
}

double CompressionUnit::getX(){ return X; }

void CompressionUnit::finish()
{
    if((getX() != 0) && getSystemModule()->par("debugMode")){
        EV_DEBUG << "(CU) Total # of packets seen by CU: " << pkt_cnt << '\n' <<
                "(CU) Total # of packets queued in CU (at the end): " << queue->getLength() << '\n' <<
                "(CU) Total # of packets seen by Proc1: " << pkt_cnt1 << '\n' <<
                "(CU) Total # of packets seen by Proc2: " << pkt_cnt2 << endl;
        if(pkt_cnt != (pkt_cnt1 + pkt_cnt2 + queue->getLength()))
            EV_DEBUG << "(CU) ERROR ON PKT_CNT " << endl;
    }
}





