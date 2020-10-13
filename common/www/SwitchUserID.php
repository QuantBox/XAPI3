<?php
session_start();
require_once('config.php');

if(@$_SESSION['Right'] != 99)
{
	header("Location: $Homepage");
	return;
}

if(!empty($_POST['UserID']))
{
	$_SESSION['SWITCH'] = true;
	$_SESSION['SWITCH_ID'] = $_POST['UserID'];
	
	header("Location:Session.php");
	return;
}
?>
<!DOCTYPE HTML>
<html>
<head>
<meta charset="UTF-8">
<title>主页</title>
</head>
<body>

<form action="SwitchUserID.php" method="post">
<p>这个功能只对管理员开放，用于切换到某个用户上看授权设置是否正确,或对账号信息进行修改</p>
<p>UserID<input name="UserID"/><button type='submit'>切换</button></p>
</form>

</body>
</html>