#include "stdafx.h"
#include <thread>
#include <stdio.h>
#include "TRACE.h"
#include "DSP_Server.h"

#include <string.h>
#define HAS_PREFIX(string, prefix) (strncmp(string, prefix, strlen(prefix)) == 0)

int main()
{
    MyDSP dsp;
    DSP_Server dspServer(&dsp);
    char cmd[100];

    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    while (dspServer.isRunning())
    {
        gets_s(cmd, 100);
        if (strlen(cmd) == 0) continue;

        if (HAS_PREFIX(cmd, "stop"))
        {
            dspServer.Stop();
        }
        else
        {
            //fprintf(stderr, "cmd: %s\n", cmd);
            if (dspServer.command(cmd)) puts(cmd);
        }
    }

    TRACE("Thread stopped: Quitting\n");
    return 0;
}
