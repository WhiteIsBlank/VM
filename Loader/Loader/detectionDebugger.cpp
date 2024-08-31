#include"detect.h"
bool checkDebug()
{
    // 检查进程是否被调试器附加
    BOOL isDebugged = FALSE;
    CheckRemoteDebuggerPresent(GetCurrentProcess(), &isDebugged);

    return isDebugged;
}