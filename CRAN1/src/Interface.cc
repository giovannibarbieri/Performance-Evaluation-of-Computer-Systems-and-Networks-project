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

#include "Interface.h"
#include "myPacket_m.h"

Define_Module(Interface);

void Interface::initialize()
{
    interfaceBusy = false;
    speed = par("C");
    if(speed == 0){
        throw cRuntimeError("speed cannot be 0");
    }
    beep = new cMessage("interface-timer");
    queue = new cQueue("transmission-queue");
    // Register signal
    queueOccupancyISignal = registerSignal("queueOccupancyI");
}

void Interface::handleMessage(cMessage *msg)
{
    if(msg->isSelfMessage()) // transmission of current packet ended
        handleTransmissionEnd(check_and_cast<MyPacket*> (currentTransmissionPacket));
    else // new packet arrived from CompressionUnit
    {
        emit(queueOccupancyISignal, curQueueOccupancy);

        if(getSystemModule()->par("debugMode")){
            counterArrived++; // DEBUG
            EV_DEBUG << "(i) PacketID: " << msg->getId() << " Arrived\n";
        }
        if(interfaceBusy){
            MyPacket * pkt = check_and_cast<MyPacket *>(msg);
            queue->insert(pkt);
            curQueueOccupancy += pkt->getDimension();
            if(getSystemModule()->par("debugMode")){
                EV_DEBUG << "(i) PacketID: " << msg->getId() << ": Queued\n";
            }
        }
        else
            startTransmission(check_and_cast<MyPacket*> (msg));
    }
}

void Interface::startTransmission(MyPacket* msg)
{
    if(speed){ // the function is executed only if speed is above 0
        int dim = msg->getDimension();
        scheduleAt(simTime()+dim/speed, beep);
        if(getSystemModule()->par("debugMode")){
            EV_DEBUG << "(i) PacketID: " << msg->getId() << " Transmission started\n";
            EV_DEBUG << "(i) Time start: " << simTime() << "\n";
        }
        interfaceBusy = true;
        currentTransmissionPacket = msg;
    }
}

void Interface::handleTransmissionEnd(MyPacket* msg)
{
    send(currentTransmissionPacket, "out", currentTransmissionPacket->getTargetCell());
    currentTransmissionPacket = nullptr;
    interfaceBusy = false;

    if(getSystemModule()->par("debugMode")){
        counterTransmitted++; // DEBUG
        EV_DEBUG << "(i) PacketID: " << msg->getId() << " Transmission ended\n";
        EV_DEBUG << "(i) Time end: " << simTime() << "\n";
    }
    if(!queue->isEmpty()){
        MyPacket * pkt = check_and_cast<MyPacket *>(queue->pop());
        startTransmission(pkt);
        curQueueOccupancy -= pkt->getDimension();
        EV_DEBUG << "(i) PacketID: " << msg->getId() << ": pop\n";
    }
}

Interface::~Interface()
{
    cancelAndDelete(beep);
    if(currentTransmissionPacket != nullptr)
        delete currentTransmissionPacket;

    while(!queue->isEmpty())
        delete queue->pop();
    delete queue;
}

void Interface::finish(){
    EV_DEBUG << "(i) Number of arrived packets: " << counterArrived << "\n";
    EV_DEBUG << "(i) Number of transmitted packets: " << counterTransmitted << "\n";
}
