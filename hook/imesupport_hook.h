#ifndef _IMESUPPORT_HOOK_H_
#define _IMESUPPORT_HOOK_H_

#include <windows.h>


#ifdef _MSC_VER
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#define INVALID_VALUE 0xffffffff

#ifndef WINVER
#define WINVER 0x6010
#endif


EXPORT BOOL StartHook(HWND hwnd);
EXPORT BOOL EndHook(void);
EXPORT int GetMessageId(void);

#endif

// vim: st=4 sts=4 ts=4 et
