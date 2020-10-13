<?php
session_start();
require_once('config.php');

if(@$_SESSION['Right'] == 99)
{
}
else if($OpenSignup == false)
{
    $_SESSION['Error'] = '对不起，目前不开放注册';
    header("Location:Signin.php");
    return;
}

if(strlen($_POST['UserID'])<5||strlen($_POST['UserID'])>16)
{
    $_SESSION['Error'] = '用户名长度不对';
    header("Location:".getenv("HTTP_REFERER"));
    return;
}

if($_POST['Password'] != $_POST['Password2'])
{
    $_SESSION['Error'] = '两次密码不同！';
    header("Location:".getenv("HTTP_REFERER"));
    return;
}

// 检查用户是否已经存在，因为在有些MDB2上报的错太不靠谱
$query = 'SELECT * FROM UserInfo WHERE UserID = '
    .$mdb2->quote($_POST['UserID'],"text");
//echo $query;
$result = $mdb2->query($query);

if (PEAR::isError($result)) {
    die("{'Error':'".$result->getMessage()."'}");
}

$row = $result->fetchRow();

if(!empty($row))
{
	$_SESSION['Error'] = '用户名已经存在';
	header("Location:".getenv("HTTP_REFERER"));
	return;
}

// 插入用户
$query = 
	'INSERT INTO UserInfo(UserID,Password,'
	.'RegisterTime,IP) '
	.' VALUES('
	.$mdb2->quote($_POST['UserID'],"text").','
	.$mdb2->quote(md5($_POST['Password']),"text").','
	.'NOW(),'
	.$mdb2->quote($_SERVER['REMOTE_ADDR'],"text").')';
//echo $query;
$result = $mdb2->query($query);

if (PEAR::isError($result)) {
	die("{'Error':'".$result->getMessage()."'}");
}

$_SESSION['Error'] = '注册成功，请用新账号登录！';
header("Location:Signin.php");
return;
?>