@echo off

pushd %~dp0

start /min inject_x64.exe -d -k chusanhook_x64.dll amdaemon.exe -f -c config_common.json config_server.json config_cvt.json
inject_x86.exe -d -k chusanhook_x86.dll chusanApp.exe
taskkill /f /im amdaemon.exe > nul 2>&1

echo.
echo Game processes have terminated
pause