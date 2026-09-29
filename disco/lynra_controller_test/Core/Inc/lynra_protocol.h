#ifndef LYNRA_PROTOCOL_H
#define LYNRA_PROTOCOL_H

#include <stdint.h>
#include <stddef.h>

typedef void (*LynraProtocolTxFn)(const uint8_t *data, size_t length);

void LynraProtocol_Init(LynraProtocolTxFn txFunction);
void LynraProtocol_ProcessCommand(const char *command);

#endif
