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

#ifndef __CRAN1_INTERFACE_H_
#define __CRAN1_INTERFACE_H_

#include <omnetpp.h>
#include "myPacket_m.h"

using namespace omnetpp;

/**
 * TODO - Generated class
 */
class Interface : public cSimpleModule
{
  private:
    double speed;
    bool interfaceBusy;
    cQueue * queue;
    cMessage* beep; // timer for completed transmission
    MyPacket* currentTransmissionPacket;

    // For recording
    int curQueueOccupancy = 0;
    simsignal_t queueOccupancyISignal;

    // DEBUGGING VARIABLES
    int counterArrived = 0;
    int counterTransmitted = 0;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    void startTransmission(MyPacket* msg);
    void handleTransmissionEnd(MyPacket* msg);
    virtual ~Interface();
    // DEBUG FUNCTIONS
    virtual void finish() override;
};

#endif
