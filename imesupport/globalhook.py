# -*- coding: utf-8 -*-
import ctypes
from os.path import join, dirname, abspath
from os import popen

INVALID_VALUE = 0xffff

WM_IMESUPPORT_SET_INLINE_POSITION = -1
imesupport_dll = None


def setup(hwnd, arch_x64, dll_dir=dirname(dirname(abspath(__file__)))):
    # Default DLL location: ../imesupport_hook_xxx.dll
    global imesupport_dll
    global WM_IMESUPPORT_SET_INLINE_POSITION
    if imesupport_dll is not None:
        return True

    dll_name = 'imesupport_hook'
    if arch_x64:
        dll_name += '_x64'
    else:
        dll_name += '_x86'

    with popen('wmic os get caption') as ret:
        systeminfo = ret.read()
        if systeminfo.find('Windows 10') < 0:
            dll_name += '_win7'

    dll_name += '.dll'
    print(dll_name)
    imesupport_dll = ctypes.cdll.LoadLibrary(join(dll_dir, dll_name))
    WM_IMESUPPORT_SET_INLINE_POSITION = imesupport_dll.GetMessageId()
    return imesupport_dll.StartHook(hwnd)


def cleanup():
    global imesupport_dll

    imesupport_dll.EndHook()
    return ctypes.cdll.FreeLibrary(imesupport_dll)


def set_inline_position(hwnd, x, y, font_size, scaling):
    if imesupport_dll is not None:
        ctypes.windll.user32.PostMessageW(
            hwnd, WM_IMESUPPORT_SET_INLINE_POSITION, x << 16 | y,
            font_size << 16 | scaling)


def clear_inline_position(hwnd):
    if imesupport_dll is not None:
        ctypes.windll.user32.PostMessageW(hwnd,
                                          WM_IMESUPPORT_SET_INLINE_POSITION,
                                          INVALID_VALUE, INVALID_VALUE)
