//
// Copyright (c) 2010-2021 Antmicro
//
// This file is licensed under the MIT License.
// Full license text is available in 'licenses/MIT.txt'.
//
#ifndef Dbbif_H
#define Dbbif_H

#include "bus.h"
#include <src/renode_bus.h>

struct Dbbif : public BaseBus {
    Dbbif(uint32_t dataWidth, uint32_t addrWidth);

    virtual void tick(bool countEnable, uint64_t steps);

    virtual void write(uint64_t addr, uint64_t value);

    virtual uint64_t read(uint64_t addr);

    virtual void reset();

    void timeoutTick(uint8_t *signal, uint8_t value, int timeout = 20);

    void setAgent(RenodeAgent *agent);

    RenodeAgent *agent;

    uint32_t dataWidth;
    uint32_t addrWidth;

    // Global Dbbif Signals
    uint8_t *aclk;
    uint8_t *aresetn;

    // Write Address Channel Signals
    uint8_t *awvalid;
    uint8_t *awready;
    uint8_t *awlen;
    uint64_t *awaddr;
    uint8_t *awid;

    // Write Data Channel Signals
    uint8_t *wvalid;
    uint8_t *wready;
    uint64_t *wdata;
    uint8_t *wlast;
    uint8_t *wstrb;

    // Write Response Channel Signals
    uint8_t *bvalid;
    uint8_t *bready;
    uint8_t *bid;

    // Read Address Channel Signals
    uint8_t *arvalid;
    uint8_t *arready;
    uint8_t *arlen;
    uint64_t *araddr;
    uint8_t *arid;

    // Read Data Channel Signals
    uint8_t *rvalid;
    uint8_t *rready;
    uint8_t *rlast;
    uint64_t *rdata;
    uint8_t *rid;
};

#endif
