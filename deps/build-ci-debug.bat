@echo off

set vswhere="%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
for /f "usebackq delims=" %%i in (`%vswhere% -latest -property installationPath`) do (
  set vcvarsall="%%i\VC\Auxiliary\Build\vcvars32.bat"
)
@echo on
call %vcvarsall%

cd /D %~dp0
call src\curl\buildconf.bat

cd src\curl\winbuild\

nmake /f Makefile.vc mode=static RTLIBCFG=static MACHINE=x86 DEBUG=yes
xcopy /s ..\builds\libcurl-vc-x86-debug-static-ipv6-sspi-winssl\lib ..\..\..\lib\
