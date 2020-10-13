@echo off
echo 此工具完成XAPI的COM版的注册。
echo 请右键“以管理员方式运行”否则无权限操作
echo 这是在64位注册表和32位注册表下都添加COM组件
echo 进程外与进程内的运行方式都加添了，但有些系统可能无法正确选择是进程内还是进程外，可能需要clear_InprocServer32.reg配合使用
@echo on
cd %~dp0
%~d0
x64\RegAsm.exe XAPI_CSharp.exe /nologo /codebase /tlb
x86\RegAsm.exe XAPI_CSharp.exe /nologo /codebase /tlb
pause