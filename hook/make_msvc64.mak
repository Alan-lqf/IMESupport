all: ..\imesupport_hook_x64.dll

..\imesupport_hook_x64.dll: imesupport_hook_x64.dll
	copy imesupport_hook_x64.dll ..\imesupport_hook_x64.dll

imesupport_hook_x64.dll: imesupport_hook.c imesupport_hook.h
	cl /DWINVER=0xA000 /wd4996 /LD /Feimesupport_hook_x64.dll imesupport_hook.c imm32.lib user32.lib gdi32.lib kernel32.lib ntdll.lib

clean:
	cmd /C "del ..\imesupport_hook_x64.dll imesupport_hook_x64.dll imesupport_hook_x64.exp imesupport_hook_x64.lib imesupport_hook.obj" /F /Q
