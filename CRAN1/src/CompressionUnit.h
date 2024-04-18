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

#ifndef __CRAN1_COMPRESSIONUNIT_H_
#define __CRAN1_COMPRESSIONUNIT_H_

#include <omnetpp.h>
#include "myPacket_m.h"

using namespace omnetpp;

class CompressionUnit : public cSimpleModule
{
    private:
        double X; // compression ratio, it belongs to [0,1)
        bool proc1Busy = false, proc2Busy = false;
        cMessage * timer1, * timer2; // timers for compression processes
        MyPacket * pkt1 = nullptr, * pkt2 = nullptr; // to store packets during compression
        cQueue * queue;
        simtime_t S; // Service time: computed at the initialization as (70ms x X)

        // For recording
        int curQueueOccupancy = 0;
        simsignal_t queueOccupancyCUSignal;
        simsignal_t responseTimeCUSignal;

        // For debugging
        int pkt_cnt = 0, pkt_cnt1 = 0, pkt_cnt2 = 0;

        //maximum compression ratio
        const double MAX_COMPRESSION = 0.9;
    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
        void handleCompressionEnded(cMessage *msg);
        void handlePktToBeCompressed(MyPacket *pkt);
        ~CompressionUnit();
        virtual void finish() override;
    public:
        double getX();
};

#endif
