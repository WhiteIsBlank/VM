#include"detect.h"
bool checkDebug()
{
    // �������Ƿ񱻵���������
    BOOL isDebugged = FALSE;
    CheckRemoteDebuggerPresent(GetCurrentProcess(), &isDebugged);

    return isDebugged;
}