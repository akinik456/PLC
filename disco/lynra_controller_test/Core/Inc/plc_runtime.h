#ifndef PLC_RUNTIME_H
#define PLC_RUNTIME_H

#include <stdint.h>

#include "plc_types.h"
#include "plc_program.h"


/* =========================================================
 * PLC RUNTIME
 * ========================================================= */

void PlcRuntime_Init(const PlcProgram *program);

void PlcRuntime_Start(void);

void PlcRuntime_Stop(void);

void PlcRuntime_Scan(uint32_t elapsedMs);

const PlcRuntimeContext *PlcRuntime_GetContext(void);

void PlcRuntime_SetOutputChangeEnable(
    uint8_t outputIndex,
    bool enable);

void PlcRuntime_SetInputs(uint32_t digitalInputs);


#endif /* PLC_RUNTIME_H */
