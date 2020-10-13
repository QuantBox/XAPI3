<?php
session_start();
require_once('config.php');

unset($_SESSION['Error']);
unset($_SESSION['LOGINED']);
unset($_SESSION['UserID']);
unset($_SESSION['ID']);

if(@$_SESSION['SWITCH']==true)
{
    // 取账号信息
    $query = 'SELECT * FROM UserInfo WHERE UserID = '
            .$mdb2->quote($_SESSION['SWITCH_ID'],"text");
}
else
{
    // 取账号信息
    $query = 'SELECT * FROM UserInfo WHERE UserID = '
		.$mdb2->quote($_POST['UserID'],"text");
}

//echo $query;
$result = $mdb2->query($query);

if (PEAR::isError($result)) {
    die("{'Error':'".$result->getMessage()."'}");
}

$row = $result->fetchRow();

if(empty($row))
{
	$_SESSION['Error'] = '账号不存在，请检查账号是否填错';
	header("Location:".getenv("HTTP_REFERER"));
	return;
}

if(@$_SESSION['SWITCH']==true)
{
    // 切换账号，不检查密码
}
else
{
    if(md5($_POST['Password']) != $row[strtolower('Password')])
    {
        $_SESSION['Error'] = '密码错误，请检查账号或密码是否填错';
        header("Location:".getenv("HTTP_REFERER"));
        return;
    }
}

unset($_SESSION['SWITCH']);
unset($_SESSION['SWITCH_ID']);

$_SESSION['LOGINED'] = true;
$_SESSION['UserID'] = $row[strtolower('UserID')];
$_SESSION['ID'] = $row[strtolower('ID')];
$_SESSION['Password'] = $row[strtolower('Password')];
$_SESSION['Right'] =  $row[strtolower('Right')];

// 查找是否有审核权限
$query = 'SELECT * FROM Product_User2 WHERE User2 = '
		.$mdb2->quote($_SESSION['UserID'],"text");
//echo $query;
$result = $mdb2->query($query);

if (PEAR::isError($result)) {
    die("{'Error':'".$result->getMessage()."'}");
}

$Products = array();
while($row = $result->fetchRow())
{
    array_push($Products,$row[strtolower('Product')]);
}
$_SESSION['Products'] = $Products;

// 可以判断一下，只让本机登录的为管理员
/*if($_SESSION['Right'] == 99)
{
    // 居然有IPv6的问题
    $admin_ips = array('127.0.0.1','::1','');
    if(in_array($_SERVER['REMOTE_ADDR'],$admin_ips,true))
    {
    }
    else
    {         
        // 降级为审核员
        $_SESSION['Right'] = 2;
    }
}*/

if($_SESSION['Right'] == 99)
{
    // 通过特殊页面登录成功的管理员才有管理员权限
    //echo (int)$_POST['Token'];
    //echo (int)$admin_token;
    //return;
    // 这个功能与某个管理员登录的php有关联，要改的话得同时改
    if(@$_SESSION['TRY_ADMIN'] == true)
    {
    }
    else
    {         
        // 降级为审核员
        $_SESSION['Right'] = 2;
    }
}

// 会话有效期1小时
$lifeTime = 3600;
setcookie(session_name(), session_id(), time() + $lifeTime, "/");
    
header("Location: $Homepage");
return;
?>