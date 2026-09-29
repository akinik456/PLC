#include "plc_hardware.h"

#include "main.h"


/* =========================================================
 * INIT
 * ========================================================= */

void PlcHardware_Init(void)
{
    HAL_GPIO_WritePin(
        GPIOG,
        GPIO_PIN_13 | GPIO_PIN_14,
        GPIO_PIN_RESET);
}

/* =========================================================
 * READ INPUTS
 * ========================================================= */

uint32_t PlcHardware_ReadInputs(void)
{
    uint32_t inputs = 0;

    /* I1 - USER button PA0 - active HIGH */
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET)
    {
        inputs |= (1UL << 0);
    }

    /* I2 - PE2 - active LOW */
    if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_2) == GPIO_PIN_RESET)
    {
        inputs |= (1UL << 1);
    }

    return inputs;
}


/* =========================================================
 * APPLY OUTPUTS
 * ========================================================= */

void PlcHardware_ApplyOutputs(const PlcIoImage *io)
{
    if (io == 0)
    {
        return;
    }


    /* Q1 -> PG13 / Green LED */

    if (io->digitalOutputs & (1UL << 0))
    {
        HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_RESET);
    }


    /* Q2 -> PG14 / Red LED */

    if (io->digitalOutputs & (1UL << 1))
    {
        HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_RESET);
    }
}
