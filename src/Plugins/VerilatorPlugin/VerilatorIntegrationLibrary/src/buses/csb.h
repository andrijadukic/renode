#ifndef CSB_H_
#define CSB_H_

#include "bus.h"

struct CSB : public BaseBus
{
    virtual void tick(bool countEnable, uint64_t steps);
    virtual void write(uint64_t addr, uint64_t value);
    virtual uint64_t read(uint64_t addr);
    virtual void reset();
    void timeoutTick(uint8_t* signal,
                     uint8_t expectedValue,
                     int timeout);


    uint8_t* csb_clock;
    uint8_t* csb_ready;
    uint8_t* csb_r_valid;
    uint32_t* csb_r_data;
    uint8_t* csb_wr_complete;
    uint8_t* csb_valid;
    uint16_t* csb_addr;
    uint32_t* csb_wdat;
    uint8_t* csb_write;
    uint8_t* csb_nposted;


};


#endif // CSB_H_
