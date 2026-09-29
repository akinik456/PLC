#ifndef PLC_TYPES_H
#define PLC_TYPES_H

#include <stdint.h>
#include <stdbool.h>


/* =========================================================
 * PLC STATE
 * ========================================================= */

typedef enum
{
    PLC_STATE_STOPPED = 0,
    PLC_STATE_RUNNING,
    PLC_STATE_FAULT

} PlcState;


/* =========================================================
 * PLC I/O IMAGE
 *
 * Runtime does not access physical GPIO directly.
 * Hardware layer updates inputs and applies outputs.
 * ========================================================= */

typedef struct
{
    uint32_t digitalInputs;

    uint32_t requestedOutputs;
    uint32_t digitalOutputs;

    uint32_t outputChangeEnableMask;

} PlcIoImage;

typedef enum
{
    PLC_FAULT_NONE = 0,
    PLC_FAULT_STEP_TIMEOUT,
    PLC_FAULT_INVALID_STEP

} PlcFaultCode;


/* =========================================================
 * PLC SEQUENCE RUNTIME
 *
 * Each sequence runs independently.
 * ========================================================= */

typedef struct
{
    uint16_t currentStep;
    uint32_t stepElapsedMs;

    bool active;
    bool faulted;

    PlcFaultCode faultCode;

} PlcSequenceRuntime;
/* =========================================================
 * PLC RUNTIME CONTEXT
 *
 * This structure contains the live state of the PLC.
 * It lives in RAM.
 * ========================================================= */

#define PLC_MAX_SEQUENCES    8

typedef struct
{
    PlcState state;

    PlcIoImage io;

    PlcSequenceRuntime sequences[PLC_MAX_SEQUENCES];

    uint16_t faultCode;

    uint32_t scanCounter;

} PlcRuntimeContext;


#endif /* PLC_TYPES_H */
