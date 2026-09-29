#ifndef PLC_PROGRAM_H
#define PLC_PROGRAM_H

#include <stdint.h>
#include "plc_types.h"

/* =========================================================
 * STEP TRANSITION TYPE
 * ========================================================= */

typedef enum
{
    PLC_TRANSITION_TIME = 0,
    PLC_TRANSITION_INPUT_ON

} PlcTransitionType;


/* =========================================================
 * PLC STEP
 * ========================================================= */

typedef struct
{
    uint32_t outputs;

    PlcTransitionType transitionType;

    uint16_t transitionValue;
    uint32_t transitionTimeMs;

    uint16_t nextStep;

} PlcStep;


/* =========================================================
 * PLC SEQUENCE
 * ========================================================= */

typedef struct
{
    const PlcStep *steps;
    uint16_t stepCount;

} PlcSequence;

/* =========================================================
 * PLC INTERLOCK
 *
 * v0.01
 *
 * If input is ON, output state change is inhibited.
 * ========================================================= */

typedef struct
{
    uint8_t inputIndex;
    bool inputState;       /* true = ON, false = OFF */

    uint8_t outputIndex;

} PlcInterlock;

/* =========================================================
 * PLC PROGRAM
 * ========================================================= */

typedef struct
{
    const PlcSequence *sequences;
    uint16_t sequenceCount;

    const PlcInterlock *interlocks;
    uint16_t interlockCount;

} PlcProgram;




extern const PlcProgram g_plcProgram;


#endif /* PLC_PROGRAM_H */
