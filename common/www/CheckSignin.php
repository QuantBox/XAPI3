<?php
if(@$_SESSION['LOGINED'] != true)
{
	$_SESSION['Error'] = '请先登录！';
	header("Location:Signin.php");
	return;
}
?>