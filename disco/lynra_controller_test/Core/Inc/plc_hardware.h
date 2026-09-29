#ifndef PLC_HARDWARE_H
#define PLC_HARDWARE_H

#include "plc_types.h"


void PlcHardware_Init(void);

uint32_t PlcHardware_ReadInputs(void);

void PlcHardware_ApplyOutputs(const PlcIoImage *io);


#endif /* PLC_HARDWARE_H */
