//
// Copyright (c) 2010-2021 Antmicro
//
// This file is licensed under the MIT License.
// Full license text is available in 'licenses/MIT.txt'.
//
#ifndef DbbifSlave_H
#define DbbifSlave_H
#include "dbbif.h"
#include <src/renode_bus.h>

enum class DbbifReadState  {AR, R};
enum class DbbifWriteState {AW, W, B};

struct DbbifSlave : public Dbbif
{
    DbbifSlave(uint32_t dataWidth, uint32_t addrWidth);
    virtual void tick(bool countEnable, uint64_t steps);
    virtual void write(uint64_t addr, uint64_t value);
    virtual uint64_t read(uint64_t addr);
    virtual void reset();

    void readWord(uint64_t addr);
    void writeWord(uint64_t addr, uint64_t data, uint8_t strb);

    void clearSignals();
    void updateSignals();
    void writeHandler();
    void readHandler();

    DbbifWriteState writeState;
    DbbifReadState  readState;

    uint8_t awready_new;
    uint8_t wready_new;
    uint8_t bvalid_new;

    uint8_t arready_new;
    uint8_t rvalid_new;
    uint8_t rlast_new;
    uint64_t rdata_new;

    uint64_t      writeAddr;
    uint8_t       writeLen;

    uint64_t      readAddr;
    uint8_t       readLen;

    char buffer [50];
};
#endif
