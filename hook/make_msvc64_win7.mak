all: ..\imesupport_hook_x64_win7.dll

..\imesupport_hook_x64_win7.dll: imesupport_hook_x64_win7.dll
	copy imesupport_hook_x64_win7.dll ..\imesupport_hook_x64_win7.dll

imesupport_hook_x64_win7.dll: imesupport_hook.c imesupport_hook.h
	cl /wd4996 /LD /Feimesupport_hook_x64_win7.dll imesupport_hook.c imm32.lib user32.lib gdi32.lib kernel32.lib ntdll.lib

clean:
	cmd /C "del ..\imesupport_hook_x64_win7.dll imesupport_hook_x64_win7.dll imesupport_hook_x64_win7.exp imesupport_hook_x64_win7.lib imesupport_hook.obj" /F /Q
