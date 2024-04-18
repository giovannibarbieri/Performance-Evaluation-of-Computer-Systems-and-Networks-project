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

#include "AS.h"
#include "myPacket_m.h"

Define_Module(AS);

void AS::initialize()
{
    N = getSystemModule()->par("N");
    if(N == 0){
        throw cRuntimeError("N cannot be 0");
    }
    simtime_t t = par("t");

    beep = new cMessage("AS-next-pkt-gen-timer");

    //schedule the first packet
    scheduleAt(simTime() + t, beep);

    //debugging initialization
    count = 0;
}

void AS::handleMessage(cMessage *msg)
{

    MyPacket *packet = new MyPacket("pkt");
    int targetCell = floor(uniform(0, N, 2));
    simtime_t t = par("t");
    int s = ceil((double)par("s"));

    packet->setId(count);
    packet->setDimension(s);
    packet->setArrivalTime(simTime());
    packet->setTargetCell(targetCell);
    send(packet, "out");

    if(getSystemModule()->par("debugMode")){
        EV_DEBUG<<"(AS) "<<count<<": Packet sent"<<"\n";
        EV_DEBUG<<"(AS) next arrival in: "<<t<<"\n";
    }

    count++;
    scheduleAt(simTime() + t, beep);
}

void AS::finish(){
    if(getSystemModule()->par("debugMode")){
        EV_DEBUG<<"(AS) # of packets sent by AS: "<<count<<"\n";
    }
}

AS::~AS(){
    cancelAndDelete(beep);
}
