REM 11 合并持仓
REM 22 查资金
REM 7 循环下单
PATH d:\Users\Kan\Anaconda3;d:\Users\Kan\Anaconda3\Scripts;d:\Users\Kan\Anaconda3\Library\bin;%PATH%
set date_Ymd=%date:~0,4%%date:~5,2%%date:~8,2%
python.exe test_tdx_api.py --input=22;7;22;41 1>log/%date_Ymd%.log 2>&1
REM 最后这里加一个发邮件就成了
python mail.py --username=123456 --password= --from=123456@qq.com --to=123456@qq.com --log=log --bat=%~f0