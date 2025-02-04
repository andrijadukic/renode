//
// Copyright (c) 2021 Antmicro
//
// This file is licensed under the MIT License.
// Full license text is available in 'licenses/MIT.txt'.
//

#include "cfu.h"

void Cfu::tick(bool countEnable, uint64_t steps = 1)
{
  *clk = 0;
  evaluateModel();
  for(uint64_t i = 0; i < steps; i++) {
    *clk = 1;
    evaluateModel();
    *clk = 0;
    evaluateModel();
  }

  if(countEnable) {
      tickCounter += steps;
  }
}

void Cfu::timeoutTick(uint8_t* signal, uint8_t expectedValue, int timeout = 2000)
{
  do
  {
    tick(true);
    timeout--;
  }
  while((*signal != expectedValue) && timeout > 0);

  if(timeout == 0) {
    throw "Operation timeout";
  }
}

uint64_t Cfu::execute(uint32_t functionID, uint32_t data0, uint32_t data1, int* error)
{
  uint64_t result;
  *req_func_id = functionID;
  *req_data0 = data0;
  *req_data1 = data1;
  *req_valid = 1;
  *resp_ready = 0;

  /* Wait for CFU to accept a command */
  if(*req_ready != 1) {
    timeoutTick(req_ready, 1);
  } else {
    tick(true);
  }

  /* CPU passed a command so deassert req_valid */
  *req_valid = 0;

  /* Check if CFU finished operation and wait for it if it's not finished */
  if(*resp_valid != 1) {
    timeoutTick(resp_valid, 1);
  }

  /* Indicate receive and save output from CFU */
  *resp_ready = 1;
  result = *resp_data;

  /* Tick until CFU is ready for next request */
  timeoutTick(req_ready, 1);

  /* Error signal is not supported by CFU yet so set it to 0 */
  *error = 0;

  return result;
}

void Cfu::reset()
{
  *rst = 1;
  tick(true);
  *rst = 0;
  tick(true);
}
