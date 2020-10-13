<?php
session_start();
require_once('config.php');
include_once('CheckSignin.php');
?><!DOCTYPE HTML>
<html>
<head>
<meta charset="UTF-8">
<title>主页</title>
</head>
<body>
当前用户：<?php echo @$_SESSION['UserID']; ?> 当前IP: <?php echo $_SERVER['REMOTE_ADDR']; ?> <a href="Logout.php">退出</a>  <a href="aboutus.php"  target="_blank">关于我们</a>
<hr/>
<h1>用户信息</h1>
<a href="UserInfoView.php" target="_blank">查看/修改个人信息，修改密码</a>
<hr/>
<h1>产品信息</h1>
<a href="ProductInfoListView.php" target="_blank">查看产品列表，下载与本授权系统配套的DLL</a>
<hr/>
<h1>授权信息</h1>
<a href="LicenseInfoListView.php" target="_blank">查看授权列表,添加授权</a>
<?php if(@$_SESSION['Right'] >= 2) {?>
<hr/>
<h1>审核管理</h1>
<a href="LicenseInfoListView2.php" target="_blank">审核用户</a>
<?php }?>
<?php if(@$_SESSION['Right'] == 99) {?>
<hr/>
<h1>管理员专区</h1>
<a href="LicenseInfoListView99.php" target="_blank">审核用户</a>
<a href="User2ListView.php" target="_blank">查看审核人员列表</a>
<a href="Signup.php" target="_blank">注册用户</a>
<a href="SwitchUserID.php" target="_blank">切换用户ID</a>
<?php }?>
</body>
</html>