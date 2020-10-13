REM 2 查询持仓
REM 42 持仓回报等10秒
REM 12 写回目标持仓文件
REM 13 合并增量文件
REM 7 循环下单
PATH d:\Users\Kan\Anaconda3;d:\Users\Kan\Anaconda3\Scripts;d:\Users\Kan\Anaconda3\Library\bin;%PATH%
set date_Ymd=%date:~0,4%%date:~5,2%%date:~8,2%
python.exe test_ctp_api.py --input=2;42;12;13;7 1>log/%date_Ymd%.log 2>&1
REM 最后这里加一个发邮件就成了
python mail.py --username=123456 --password=654321 --from=123456@qq.com --to=123456@qq.com;654321@qq.com --log=log --bat=%~f0
