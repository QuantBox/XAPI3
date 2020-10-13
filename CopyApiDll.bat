REM 32位调试版
xcopy "include\Tdx\win32\*.*" "bin\Debug\x86\Tdx" /Y
xcopy "include\CTP\win32\*.dll" "bin\Debug\x86\CTP" /Y
xcopy "include\Kingstar\win32\*.*" "bin\Debug\x86\Kingstar" /Y
xcopy "include\LTS_v2\win32\*.dll" "bin\Debug\x86\LTS_v2" /Y
xcopy "include\UFX\win32\*.*" "bin\Debug\x86\UFX" /Y
xcopy "include\Wind\win32\*.dll" "bin\Debug\x86\Wind" /Y
xcopy "include\Sgit\win32\*.dll" "bin\Debug\x86\Sgit" /Y
xcopy "include\CTP_SOPT\win32\*.dll" "bin\Debug\x86\CTP_SOPT" /Y
xcopy "include\ATP\win32\*.dll" "bin\Debug\x86\ATP" /Y
xcopy "include\CTPZQ\win32\*.dll" "bin\Debug\x86\CTPZQ" /Y
xcopy "include\CTP_SE\win32\*.dll" "bin\Debug\x86\CTP_SE" /Y

REM 32位发布版
xcopy "include\Tdx\win32\*.*" "bin\Release\x86\Tdx" /Y
xcopy "include\CTP\win32\*.dll" "bin\Release\x86\CTP" /Y
xcopy "include\Kingstar\win32\*.*" "bin\Release\x86\Kingstar" /Y
xcopy "include\LTS_v2\win32\*.dll" "bin\Release\x86\LTS_v2" /Y
xcopy "include\UFX\win32\*.*" "bin\Release\x86\UFX" /Y
xcopy "include\Wind\win32\*.dll" "bin\Release\x86\Wind" /Y
xcopy "include\Sgit\win32\*.dll" "bin\Release\x86\Sgit" /Y
xcopy "include\CTP_SOPT\win32\*.dll" "bin\Release\x86\CTP_SOPT" /Y
xcopy "include\ATP\win32\*.dll" "bin\Release\x86\ATP" /Y
xcopy "include\CTPZQ\win32\*.dll" "bin\Release\x86\CTPZQ" /Y

REM 64位调试版
xcopy "include\CTP\win64\*.dll" "bin\Debug64\x64\CTP" /Y
xcopy "include\CTP_SE\win64\*.dll" "bin\Debug64\x64\CTP_SE" /Y

REM 64位发布版
xcopy "include\CTP\win64\*.dll" "bin\Release64\x64\CTP" /Y

pause