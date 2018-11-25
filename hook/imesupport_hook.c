#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <versionhelpers.h>
#include <stdio.h>
#include <tchar.h>
#include <strsafe.h>
#include "imesupport_hook.h"

static LRESULT CALLBACK MyHookProc(int nCode, WPARAM wp, LPARAM lp);
static LRESULT CALLBACK WindowMessageHookProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void Trace(const TCHAR *str, BOOL append);

static HINSTANCE hModule = NULL;
static HHOOK hHook = NULL;

BOOL WINAPI DllMain(HINSTANCE hModuleDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    hModule = hModuleDLL;

    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            break;
        case DLL_PROCESS_DETACH:
            if (hHook != NULL) {
                EndHook();
            }
            break;
    }
    return TRUE;
}


EXPORT BOOL StartHook(HWND hwnd)
{
    if (hHook != NULL) {
        return FALSE;
    }

    DWORD tid = GetWindowThreadProcessId(hwnd, NULL);
    hHook = SetWindowsHookEx(WH_GETMESSAGE, MyHookProc, hModule, tid);

    return hHook != NULL;
}

EXPORT BOOL EndHook(void)
{
    if (hHook == NULL) {
        return FALSE;
    }
    BOOL ret = UnhookWindowsHookEx(hHook);
    hHook = NULL;
    return ret;
}

void SetFont(LOGFONT *lf)
{
    FILE *fp;
    TCHAR szFontInfoFile[_MAX_PATH];
    TCHAR szFull[_MAX_PATH];
    TCHAR szDrive[_MAX_PATH];
    TCHAR szDir[_MAX_DIR];
    TCHAR szFontName[128] = {0};

    GetModuleFileName(hModule, szFull, sizeof(szFull) / sizeof(TCHAR));
    _tsplitpath(szFull, szDrive, szDir, NULL, NULL);
    _tmakepath(szFontInfoFile, szDrive, szDir, _T("font_name"), _T("txt"));

    if ((fp = fopen(szFontInfoFile, _T("r"))) != NULL) {
        if (fread(szFontName, sizeof(TCHAR), 128, fp) > 0) {
            _sntprintf((LPSTR) lf->lfFaceName, LF_FACESIZE, _T("%s"), szFontName);
        }

        fclose(fp);
    }
}

EXPORT int GetMessageId(void)
{
    static UINT message = 0;

    if (message == 0) {
        message = RegisterWindowMessage(_T("WM_IMESUPPORT_SET_INLINE_POSITION"));
    }

    return message;
}

static LRESULT CALLBACK MyHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode < 0) {
        return CallNextHookEx(hHook, nCode, wParam, lParam);
    }
    else if (nCode == HC_ACTION) {
        const MSG *p = (const MSG *)lParam;
        WindowMessageHookProc(p->hwnd, p->message, p->wParam, p->lParam);
    }

    return CallNextHookEx(hHook, nCode, wParam, lParam);
}

static void im_set_composition(HWND hWnd, int x, int y, LOGFONT *lf)
{
    HIMC hImc;

    if ((hImc = ImmGetContext(hWnd)) != (HIMC) 0) {
        if (ImmGetOpenStatus(hImc)) {
            ImmSetCompositionFont(hImc, lf);

            COMPOSITIONFORM cfs;
            cfs.dwStyle = CFS_POINT;
            cfs.ptCurrentPos.x = x;
            cfs.ptCurrentPos.y = y;

            // MapWindowPoints(HWND_DESKTOP, hWnd, &cfs.ptCurrentPos, 1);
            ImmSetCompositionWindow(hImc, &cfs);
        }

        ImmReleaseContext(hWnd, hImc);
    }
}

static LRESULT CALLBACK WindowMessageHookProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static int x = INVALID_VALUE;
    static int y = INVALID_VALUE;
    static int font_size = INVALID_VALUE;
    static LOGFONT lf =
    {
        -12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        PROOF_QUALITY, FIXED_PITCH | FF_DONTCARE,
        "Consolas"
    };
    static UINT uDpi = 96;
    static float scaling = 1.0;
    HIMC hImc;

    SetFont(&lf);
    switch (msg) {
        case WM_IME_STARTCOMPOSITION:
        case WM_IME_COMPOSITION:
        case WM_IME_NOTIFY:
            if (x == INVALID_VALUE || y == INVALID_VALUE) {
                break;
            }
            if ((hImc = ImmGetContext(hWnd)) != (HIMC) 0) {
                if (msg == WM_IME_NOTIFY && wParam == IMN_SETCOMPOSITIONWINDOW) {
                    ImmReleaseContext(hWnd, hImc);
                    break;
                }

                im_set_composition(hWnd, x, y, &lf);
                ImmReleaseContext(hWnd, hImc);
            }
            break;
        default:
            if (msg == GetMessageId()) {
                if (wParam != INVALID_VALUE && lParam != INVALID_VALUE) {
                    x = (wParam >> 16) & 0xffff;
                    y = wParam & 0xffff;

                    font_size = (lParam >> 16) & 0xffff;

                    HDC hdc;
                    hdc = GetDC(hWnd);
                    lf.lfHeight = - MulDiv(font_size, GetDeviceCaps(hdc, LOGPIXELSY), 72);
                    ReleaseDC(hWnd, hdc);

#if WINVER > 0x6010
                    DPI_AWARENESS dpiAwareness = GetAwarenessFromDpiAwarenessContext(GetThreadDpiAwarenessContext());
                    switch (dpiAwareness) {
                        case DPI_AWARENESS_SYSTEM_AWARE:
                            uDpi = GetDpiForSystem();
                            break;
                        case DPI_AWARENESS_PER_MONITOR_AWARE:
                            uDpi = GetDpiForWindow(hWnd);
                            break;
                    }
                    scaling = (float) (uDpi / 96.0);
#else
                    scaling = (float) ((lParam & 0xffff) / 100.0);
#endif

                    x = (int) (x * scaling);
                    y = (int) (y * scaling);

                }
                else {
                    x = INVALID_VALUE;
                    y = INVALID_VALUE;
                    font_size = INVALID_VALUE;
                }
            }
            break;
    }

    return 0;
}

static void Trace(const TCHAR *str, BOOL append)
{
    FILE *fp;
    TCHAR szLogFile[_MAX_PATH];
    TCHAR szFull[_MAX_PATH];
    TCHAR szDrive[_MAX_DRIVE];
    TCHAR szDir[_MAX_DIR];

    GetModuleFileName(hModule, szFull, sizeof(szFull) / sizeof(TCHAR));
    _tsplitpath(szFull, szDrive, szDir, NULL, NULL);
    _tmakepath(szLogFile, szDrive, szDir, _T("imesupport"), _T("log"));

    fp = _tfopen(szLogFile, append ? _T("a") : _T("w"));

    if (fp == NULL) {
        return;
    }

    _ftprintf(fp, _T("%s"), str);
    _ftprintf(fp, _T("\n"));
    fclose(fp);
}


// vim: st=4 sts=4 ts=4 et
