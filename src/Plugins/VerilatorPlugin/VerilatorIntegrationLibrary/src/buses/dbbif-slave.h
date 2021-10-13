//
// Copyright (c) 2010-2021 Antmicro
//
// This file is licensed under the MIT License.
// Full license text is available in 'licenses/MIT.txt'.
//
#ifndef DbbifSlave_H
#define DbbifSlave_H

#include "dbbif-slave.h"
#include <cmath>
#include <cinttypes>

DbbifSlave::DbbifSlave(uint32_t dataWidth, uint32_t addrWidth) : Dbbif(dataWidth, addrWidth) {
    writeState = DbbifWriteState::AW;
    readState = DbbifReadState::AR;

    arready_new = 0;
    rvalid_new = 0;
    rlast_new = 0;
    rdata_new = 0;

    awready_new = 0;
    wready_new = 0;
    bvalid_new = 0;
}

void DbbifSlave::tick(bool countEnable, uint64_t steps = 1) {
    for (uint64_t i = 0; i < steps; i++) {
        readHandler();
        writeHandler();
        *aclk = 1;
        evaluateModel();
        updateSignals();
        *aclk = 0;
        evaluateModel();
    }

    // Since we can run out of steps during an Dbbif transaction we must let
    // the Dbbif master know that we can't accept more data at the moment.
    // To do that we set all handshake signals to 0 and readHandler/writeHandler
    // will handle resuming the transaction once tick is called again.
    clearSignals();

    if (countEnable) {
        tickCounter += steps;
    }
}

// Clear signals when leaving tick

void DbbifSlave::clearSignals() {
    // Read
    *arready = arready_new = 0;
    *rvalid = rvalid_new = 0;
    *rdata = rdata_new = 0;
    // Write
    *awready = awready_new = 0;
    *wready = wready_new = 0;
    *bvalid = bvalid_new = 0;
}

// Update signals after rising edge

void DbbifSlave::updateSignals() {
    // Read
    *arready = arready_new;
    *rvalid = rvalid_new;
    *rlast = rlast_new;
    *rdata = rdata_new;
    // Write
    *awready = awready_new;
    *wready = wready_new;
    *bvalid = bvalid_new;
}

// Sample signals before rising edge in handlers

void DbbifSlave::readWord(uint64_t addr) {
    sprintf(buffer, "Dbbif read from: 0x%"
    PRIX64, addr);
    this->agent->log(0, buffer);
    rdata_new = this->agent->requestFromAgent(addr);
}

void DbbifSlave::readHandler() {
    switch (readState) {
        case DbbifReadState::AR:
            arready_new = 1;
            if (*arready == 1 && *arvalid == 1) {
                arready_new = 0;

                readLen = *arlen;
                readState = DbbifReadState::R;
                readAddr = *araddr;

                rlast_new = (readLen == 0);

                this->agent->log(0, "Dbbif read start");

                readWord(readAddr);
            }
            break;
        case DbbifReadState::R:
            rvalid_new = 1;

            if (*rready == 1 && *rvalid == 1) {
                if (readLen == 0) {
                    readState = DbbifReadState::AR;
                    rvalid_new = 0;
                    rlast_new = 0;
                    this->agent->log(0, "Dbbif read transfer completed");
                } else {
                    readLen--;
                    readAddr += int(dataWidth / 8); // TODO: make data width configurable
                    readWord(readAddr);
                    rlast_new = (readLen == 0);
                }
            }
            break;
        default:
            readState = DbbifReadState::AR;
            break;
    }
}

void DbbifSlave::writeWord(uint64_t addr, uint64_t data, uint8_t strb) {
    sprintf(buffer, "Dbbif write to: 0x%"
    PRIX64
    ", data: 0x%X", addr, data);
    this->agent->log(0, buffer);
    this->agent->pushToAgent(writeAddr, *wdata);
}

void DbbifSlave::writeHandler() {
    switch (writeState) {
        case DbbifWriteState::AW:
            awready_new = 1;
            if (*awready == 1 && *awvalid == 1) {
                awready_new = 0;

                writeState = DbbifWriteState::W;
                writeAddr = *awaddr;

                this->agent->log(0, "Dbbif write start");
            }
            break;
        case DbbifWriteState::W:
            wready_new = 1;
            if (*wready == 1 && *wvalid == 1) {
                writeWord(writeAddr, *wdata, *wstrb);
                if (writeLen == 0) {
                    writeState = DbbifWriteState::B;
                    wready_new = 0;
                } else {
                    writeLen--;
                    writeAddr += int(dataWidth / 8); // TODO: make data width configurable
                }
            }
            break;
        case DbbifWriteState::B:
            bvalid_new = 1;
            if (*bready == 1 && *bvalid == 1) {
                bvalid_new = 0;
                writeState = DbbifWriteState::AW;
                this->agent->log(0, "Dbbif write transfer completed");
            }
            break;
        default:
            writeState = DbbifWriteState::AW;
            break;
    }
}

void DbbifSlave::reset() {
    *aresetn = 1;
    tick(true);
    *aresetn = 0;
    tick(true);
}

// You can't read/write using slave bus
void DbbifSlave::write(uint64_t addr, uint64_t value) {
    throw "Unsupported";
}

uint64_t DbbifSlave::read(uint64_t addr) {
    throw "Unsupported";
}
