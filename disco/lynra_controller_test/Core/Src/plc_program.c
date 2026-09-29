#include "plc_program.h"


/* =========================================================
 * SEQUENCE 0
 *
 * STEP 0:
 *   Q1 OFF
 *   Wait for I1
 *
 * STEP 1:
 *   Q1 ON
 *   Wait 1000 ms
 *
 * Then return to STEP 0
 * ========================================================= */

static const PlcStep sequence0Steps[] =
{
	{
		.outputs = 0x00000000,

		.transitionType = PLC_TRANSITION_INPUT,
		.transitionValue = 0,          /* I1 */
		.expectedState = true,         /* Wait for I1 ON */

		.transitionTimeMs = 0,
		.timeoutMs = 3000,
		.nextStep = 1
	},

	{
	    .outputs = (1UL << 0),

	    .transitionType = PLC_TRANSITION_TIME,
	    .transitionValue = 0,
	    .expectedState = false,        /* Not used */

	    .transitionTimeMs = 1000,
		.timeoutMs = 0,
	    .nextStep = 0
	}
};


/* =========================================================
 * SEQUENCE 1
 *
 * Q2 continuously blinks:
 *
 * STEP 0 : Q2 OFF  500 ms
 * STEP 1 : Q2 ON   500 ms
 * ========================================================= */

static const PlcStep sequence1Steps[] =
{
    {
        .outputs = 0x00000000,

        .transitionType = PLC_TRANSITION_TIME,
        .transitionValue = 0,

        .transitionTimeMs = 500,
		.timeoutMs = 0,
        .nextStep = 1
    },

	{
	    .outputs = (1UL << 1),

	    .transitionType = PLC_TRANSITION_TIME,
	    .transitionValue = 0,
	    .expectedState = false,        /* Not used */

	    .transitionTimeMs = 1000,
		.timeoutMs = 0,
	    .nextStep = 0
	}
};


/* =========================================================
 * SEQUENCES
 * ========================================================= */

static const PlcSequence sequences[] =
{
    {
        .steps = sequence0Steps,
        .stepCount = 2
    },

    {
        .steps = sequence1Steps,
        .stepCount = 2
    }
};


/* =========================================================
 * INTERLOCKS
 *
 * I1 ON -> Q2 state change inhibited
 * ========================================================= */

static const PlcInterlock interlocks[] =
{
    {
        .inputIndex = 0,       /* I1 */
        .inputState = true,    /* I1 ON */

        .outputIndex = 1       /* Q2 */
    }
};

/* =========================================================
 * PLC PROGRAM
 * ========================================================= */

const PlcProgram g_plcProgram =
{
    .sequences = sequences,
    .sequenceCount = 2,

    .interlocks = interlocks,
    .interlockCount = 1
};
