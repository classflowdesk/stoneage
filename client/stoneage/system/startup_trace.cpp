#include "systeminc/startup_trace.h"

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>

static const char *kStartupTracePath = "client-startup.log";

void StartupTraceReset(void)
{
    DeleteFileA(kStartupTracePath);
}

void StartupTrace(const char *component, const char *format, ...)
{
    char message[1024];
    FILE *file = NULL;
    SYSTEMTIME time;
    va_list args;

    va_start(args, format);
    vsnprintf_s(message, sizeof(message), _TRUNCATE, format, args);
    va_end(args);

    GetLocalTime(&time);
    if (fopen_s(&file, kStartupTracePath, "a") != 0 || file == NULL)
        return;

    fprintf(file, "%02u:%02u:%02u.%03u [%s] %s\r\n",
            time.wHour, time.wMinute, time.wSecond, time.wMilliseconds,
            component, message);
    fclose(file);
}
