Run32()

MsgBox "点我开始"

Dim XApi
Set XApi = CreateObject("XAPI.COM")
XApi.SetLibPath "C:\Program Files\SmartQuant Ltd\OpenQuant 2014\XAPI\x86\CTP\CTP_Quote_x86.dll"
XApi.SetServerInfo "Address","tcp://218.202.237.33:10012"
XApi.SetServerInfo "BrokerID","9999"
XApi.SetUserInfo "UserID","037505"
XApi.SetUserInfo "Password","123456"
XApi.Connect

Dim data
For counter = 1 to 100
Set data = XApi.TryDequeue
IF data Is Nothing Then
    WScript.Sleep 100'1000毫秒
Else
    Select Case data.Type_String
    
    Case "OnConnectionStatus" XApi_OnConnectionStatus data.Sender,data.Data1,data.Data2,data.Data3,data.Data4
    End Select
End If
Next

MsgBox "不要点我，等着反馈"

Function XApi_OnConnectionStatus(sender, status, status_String, userLogin, size1)
    MsgBox status_String
End Function

Sub Run32()
    'Author: Demon
    'Date: 2015/7/9
    'Website: http://demon.tw

    Dim strComputer, objWMIService, colItems, objItem, strSystemType
    strComputer = "."
    Set objWMIService = GetObject("winmgmts:\\" & strComputer & "\root\cimv2")
    Set colItems = objWMIService.ExecQuery("Select * from Win32_ComputerSystem",,48)
    
    For Each objItem in colItems
        strSystemType = objItem.SystemType
    Next
    
    If InStr(strSystemType, "x64") > 0 Then
        Dim fso, WshShell, strFullName
        Set fso = CreateObject("Scripting.FileSystemObject")
        Set WshShell = CreateObject("WScript.Shell")
        strFullName = WScript.FullName
        If InStr(1, strFullName, "system32", 1) > 0 Then
            strFullName = Replace(strFullName, "system32", "SysWOW64", 1, 1, 1)
            WshShell.Run strFullName & " " &_
                """" & WScript.ScriptFullName & """", 10, False
            WScript.Quit
        End If
    End If
End Sub