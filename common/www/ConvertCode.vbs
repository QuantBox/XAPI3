'/*===========================================================
' * Intro        把要转换的多个文件/文件夹拖到该文件上即可
' * FileName     ConvertCode.vbs
' * Author       Florian
' * Version      v1.0
' * WEB           http://www.cnblogs.com/fanzhidongyzby
' * Email        fanzhidongyzby@163.com
' * LastModify  2014-06-11 00:39:58
' *==========================================================*/

'-------------------------------------------------
'设置编码：默认    utf-8    -->    gb2312
'-------------------------------------------------

SrcCode="utf-8"
DestCode="gb2312"

'-------------------------------------------------
'解析参数
'-------------------------------------------------
' 由于使用了WSH.Echo直接执行会弹框，所以要通过cscript.exe来执行

Set fs = CreateObject("scripting.filesystemobject")
Set objArgs = WScript.Arguments
If objArgs.Count>0 Then
    For I = 0 To objArgs.Count - 1
        FileUrl = objArgs(I)
        Call ConvertDir(FileUrl)
    Next
Else
    'MsgBox "没有文件/文件夹被拖入！"
    WSH.Echo "NO"
    wscript.quit
End If
'MsgBox    "转换成功！"
WSH.Echo "OK"

'-------------------------------------------------
'函数名称:ConvertDir
'作用:将任意目录内的文件进行编码转换
'-------------------------------------------------

Function ConvertDir(DirUrl)
    If fs.FileExists(DirUrl) Then  
        Call ConvertFile(DirUrl)
    Else
        Call SearchDir(DirUrl)
    End If
End Function


'-------------------------------------------------
'函数名称:SearchDir
'作用:递归查找目录内的文件，进行编码转换
'-------------------------------------------------

Function SearchDir(path)    
    Set folder = fs.getfolder(path)
    Set subfolders = folder.subfolders
    Set Files = folder.Files    
    For Each i In Files
        Call ConvertFile(i.path)
    Next    
    For Each j In subfolders        
        Call SearchDir(j.path)
    Next
End Function

'-------------------------------------------------
'函数名称:ConvertFile
'作用:将一个文件进行编码转换
'-------------------------------------------------

Function ConvertFile(FileUrl)
    Call WriteToFile(FileUrl, ReadFile(FileUrl, SrcCode), DestCode)
End Function

'-------------------------------------------------
'函数名称:ReadFile
'作用:利用AdoDb.Stream对象来读取各种格式的文本文件
'-------------------------------------------------

Function ReadFile(FileUrl, CharSet)
    Dim Str
    Set stm = CreateObject("Adodb.Stream")
    stm.Type = 2
    stm.mode = 3
    stm.charset = CharSet
    stm.Open
    stm.loadfromfile FileUrl
    Str = stm.readtext
    stm.Close
    Set stm = Nothing
    ReadFile = Str
End Function

'-------------------------------------------------
'函数名称:WriteToFile
'作用:利用AdoDb.Stream对象来写入各种格式的文本文件
'-------------------------------------------------

Function WriteToFile (FileUrl, Str, CharSet)
    Set stm = CreateObject("Adodb.Stream")
    stm.Type = 2
    stm.mode = 3
    stm.charset = CharSet
    stm.Open
    stm.WriteText Str
    stm.SaveToFile FileUrl, 2
    stm.flush
    stm.Close
    Set stm = Nothing
End Function