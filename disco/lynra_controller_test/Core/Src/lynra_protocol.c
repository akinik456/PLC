#include "lynra_protocol.h"
#include <string.h>

static LynraProtocolTxFn tx = NULL;

static void SendString(const char *text)
{
    if (tx != NULL)
    {
        tx((const uint8_t *)text, strlen(text));
    }
}

void LynraProtocol_Init(LynraProtocolTxFn txFunction)
{
    tx = txFunction;
}

void LynraProtocol_ProcessCommand(const char *command)
{
    if (strcmp(command, "PING") == 0)
    {
        SendString("PONG\r\n");
    }
    else if (strcmp(command, "INFO") == 0)
    {
        SendString(
            "DEVICE=LYNRA_CTRL\r\n"
            "FW=0.0.1\r\n"
            "HW=F429_DEV\r\n"
            "STATUS=READY\r\n"
        );
    }
    else
    {
        SendString("ERR=UNKNOWN_COMMAND\r\n");
    }
}
