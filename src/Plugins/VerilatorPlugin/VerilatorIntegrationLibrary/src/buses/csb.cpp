#include "csb.h"

void CSB::tick(bool countEnable, uint64_t steps = 1)
{
    for(uint32_t i = 0; i < steps; ++i)
    {
        *csb_clock = 1;
        evaluateModel();
        *csb_clock = 0;
        evaluateModel();

    }

    if (countEnable)
    {
        tickCounter += steps;
    }
}

void CSB::timeoutTick(uint8_t* signal, uint8_t value, int timeout = 20)
{
    do
    {
        tick(true);
        --timeout;
    } while(*signal != value && timeout > 0);

    if (!timeout)
    {
        throw "CSB Operation timeout";
    }
}

void CSB::write(uint64_t addr, uint64_t value)
{
    *csb_nposted = 0;
    *csb_valid = 1;
    // FIXME: Ako NVDLA sere makni shiftanje?
    *csb_addr = addr >> 2;
    *csb_wdat = value;
    *csb_write = 1;

    timeoutTick(csb_ready, 1);

    *csb_valid = 0;
    *csb_addr = 0;
    *csb_wdat = 0;
}

uint64_t CSB::read(uint64_t addr)
{
    *csb_valid = 1;
    *csb_addr = addr;
    *csb_write = 0;

    timeoutTick(csb_ready, 1);

    *csb_valid = 0;
    *csb_addr = 0;
    *csb_write = 0;

    timeoutTick(csb_r_valid, 1);

    return *csb_r_data;
}

// set all to 0?
void CSB::reset() { }
