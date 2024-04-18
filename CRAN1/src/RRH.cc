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

#include "RRH.h"
#include "CompressionUnit.h"

Define_Module(RRH);

void RRH::initialize()
{
    if(getSystemModule()->par("debugMode")){
        count = 0;
    }

    cModule *module = getModuleByPath("CRAN.bbu.CU");
    CompressionUnit *cu = check_and_cast<CompressionUnit *>(module);
    X = cu->getX();
}

void RRH::handleMessage(cMessage *msg)
{
    currentTransmissionPacket = check_and_cast<MyPacket*>(msg);

    if(getSystemModule()->par("debugMode")){
        EV_DEBUG<<"(RRH) "<<currentTransmissionPacket->getId()<<" : received by "<<getName()<<"\n";
        count++;
    }

    decompression(currentTransmissionPacket);
    send(currentTransmissionPacket, "out");
}

void RRH::decompression(MyPacket *msg)
{
    int compressedDimension = msg->getDimension();
    int initDimension = round(compressedDimension / (1 - X));
    msg->setDimension(initDimension);
    if(getSystemModule()->par("debugMode")){
        EV_DEBUG<<"(RRH) "<<currentTransmissionPacket->getId()<<" decompressed, size changed from "<<compressedDimension<<" to "<<initDimension<<"\n";
    }
}

void RRH::finish()
{
    if(getSystemModule()->par("debugMode")){
        EV_DEBUG<<"(RRH) # of packets received by "<<getName()<<": "<<count<<"\n";
    }
}
