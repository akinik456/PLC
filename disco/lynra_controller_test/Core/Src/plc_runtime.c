#include "plc_runtime.h"

#include <string.h>


/* =========================================================
 * PRIVATE DATA
 * ========================================================= */

static PlcRuntimeContext runtimeContext;

static const PlcProgram *activeProgram = 0;


/* =========================================================
 * INIT
 * ========================================================= */

void PlcRuntime_Init(const PlcProgram *program)
{
    uint16_t i;

    memset(&runtimeContext, 0, sizeof(runtimeContext));

    runtimeContext.io.outputChangeEnableMask = 0xFFFFFFFFUL;

    activeProgram = program;

    runtimeContext.state = PLC_STATE_STOPPED;

    if (activeProgram == 0)
    {
        return;
    }

    for (i = 0;
         (i < activeProgram->sequenceCount) &&
         (i < PLC_MAX_SEQUENCES);
         i++)
    {
        runtimeContext.sequences[i].currentStep = 0;
        runtimeContext.sequences[i].stepElapsedMs = 0;
        runtimeContext.sequences[i].active = true;
        runtimeContext.sequences[i].faulted = false;
    }
}


/* =========================================================
 * START
 * ========================================================= */

void PlcRuntime_Start(void)
{
    if (activeProgram == 0)
    {
        return;
    }

    runtimeContext.state = PLC_STATE_RUNNING;
}


/* =========================================================
 * STOP
 * ========================================================= */

void PlcRuntime_Stop(void)
{
	runtimeContext.state = PLC_STATE_STOPPED;

	    runtimeContext.io.requestedOutputs = 0;
	    runtimeContext.io.digitalOutputs = 0;
}

static bool PlcRuntime_IsTransitionTrue(
    const PlcStep *step,
    PlcSequenceRuntime *sequenceRuntime,
    uint32_t digitalInputs)
{
    switch (step->transitionType)
    {
        case PLC_TRANSITION_TIME:

            if (sequenceRuntime->stepElapsedMs >=
                step->transitionTimeMs)
            {
                return true;
            }

            break;


        case PLC_TRANSITION_INPUT:
        {
            bool inputState =
                (digitalInputs &
                 (1UL << step->transitionValue)) != 0;

            if (inputState == step->expectedState)
            {
                return true;
            }

            break;
        }


        default:
            break;
    }


    return false;
}

static void PlcRuntime_EvaluateInterlocks(void)
{
    uint16_t i;

    /*
     * Start every scan with all output changes enabled.
     */
    runtimeContext.io.outputChangeEnableMask = 0xFFFFFFFFUL;


    if (activeProgram == 0)
    {
        return;
    }


    for (i = 0; i < activeProgram->interlockCount; i++)
    {
        const PlcInterlock *interlock =
            &activeProgram->interlocks[i];


        if (interlock->inputIndex >= 32 ||
            interlock->outputIndex >= 32)
        {
            continue;
        }


        /*
         * If interlock input is ON,
         * inhibit state changes of the selected output.
         */
        bool inputIsOn =
            (runtimeContext.io.digitalInputs &
             (1UL << interlock->inputIndex)) != 0;

        if (inputIsOn == interlock->inputState)
        {
            runtimeContext.io.outputChangeEnableMask &=
                ~(1UL << interlock->outputIndex);
        }
    }
}
/* =========================================================
 * SCAN
 * ========================================================= */

void PlcRuntime_Scan(uint32_t elapsedMs)
{
    uint16_t sequenceIndex;

    uint32_t requestedOutputs = 0;


    if (runtimeContext.state != PLC_STATE_RUNNING)
    {
        return;
    }

    if (activeProgram == 0)
    {
        return;
    }


    for (sequenceIndex = 0;
         (sequenceIndex < activeProgram->sequenceCount) &&
         (sequenceIndex < PLC_MAX_SEQUENCES);
         sequenceIndex++)
    {
        PlcSequenceRuntime *sequenceRuntime;
        const PlcSequence *sequence;
        const PlcStep *step;


        sequenceRuntime = &runtimeContext.sequences[sequenceIndex];

        sequence = &activeProgram->sequences[sequenceIndex];


        if (!sequenceRuntime->active)
        {
            continue;
        }

        if (sequenceRuntime->faulted)
        {
            continue;
        }

        if (sequence->stepCount == 0)
        {
            continue;
        }

        if (sequenceRuntime->currentStep >= sequence->stepCount)
        {
            sequenceRuntime->faulted = true;
            continue;
        }


        step = &sequence->steps[sequenceRuntime->currentStep];


        /*
         * Collect output requests from all active sequences.
         *
         * v0.01 behavior:
         * If any sequence requests an output ON,
         * that output becomes ON.
         */
        requestedOutputs |= step->outputs;


        /*
         * Update step elapsed time.
         */
        sequenceRuntime->stepElapsedMs += elapsedMs;


        /*
         * Step finished?
         */


        if (PlcRuntime_IsTransitionTrue(
                step,
                sequenceRuntime,
                runtimeContext.io.digitalInputs))
        {
            sequenceRuntime->stepElapsedMs = 0;

            if (step->nextStep < sequence->stepCount)
            {
                sequenceRuntime->currentStep = step->nextStep;
            }
            else
            {
                sequenceRuntime->faulted = true;
            }
        }
    }


    /*
     * v0.01:
     * No interlock layer yet.
     *
     * requestedOutputs will later pass through:
     *
     * requestedOutputs
     *       ↓
     * interlocks
     *       ↓
     * finalOutputs
     */
    runtimeContext.io.requestedOutputs = requestedOutputs;

    /*
     * Evaluate program interlocks.
     */
    PlcRuntime_EvaluateInterlocks();

    /*
     * Detect outputs whose requested state differs
     * from their currently applied state.
     */
    uint32_t changedOutputs =
        runtimeContext.io.digitalOutputs ^
        runtimeContext.io.requestedOutputs;


    /*
     * Only changes allowed by outputChangeEnableMask
     * may be applied.
     */
    uint32_t allowedChanges =
        changedOutputs &
        runtimeContext.io.outputChangeEnableMask;


    /*
     * Apply allowed changes.
     * Disabled changes keep their previous state.
     */
    runtimeContext.io.digitalOutputs ^= allowedChanges;


    runtimeContext.scanCounter++;
}
void PlcRuntime_SetOutputChangeEnable(
    uint8_t outputIndex,
    bool enable)
{
    if (outputIndex >= 32)
    {
        return;
    }

    if (enable)
    {
        runtimeContext.io.outputChangeEnableMask |=
            (1UL << outputIndex);
    }
    else
    {
        runtimeContext.io.outputChangeEnableMask &=
            ~(1UL << outputIndex);
    }
}

void PlcRuntime_SetInputs(uint32_t digitalInputs)
{
    runtimeContext.io.digitalInputs = digitalInputs;
}

/* =========================================================
 * GET CONTEXT
 * ========================================================= */

const PlcRuntimeContext *PlcRuntime_GetContext(void)
{
    return &runtimeContext;
}
