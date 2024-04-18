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

#include "Cell.h"
#include "myPacket_m.h"

Define_Module(Cell);

void Cell::initialize()
{
    delayEtoESignal = registerSignal("delayEndToEnd");
    bytesReceivedSignal = registerSignal("bytesReceived");

    //debugging initialization
    count = 0;
}

void Cell::handleMessage(cMessage *msg)
{
    MyPacket* pkt;
    pkt = check_and_cast<MyPacket*>(msg);

    if(getSystemModule()->par("debugMode")){
        EV_DEBUG<<"(Cell) "<<pkt->getId()<<" : received by "<<getName()<<"\n";
        count++;
    }

    emit(delayEtoESignal, (simTime() - pkt->getArrivalTime()));
    emit(bytesReceivedSignal, pkt->getDimension());
    delete pkt;
}

void Cell::finish(){
    if(getSystemModule()->par("debugMode")){
        EV_DEBUG<<"(Cell) # of packets received by "<<getName()<<": "<<count<<"\n";
    }
}
