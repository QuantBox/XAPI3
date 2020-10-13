<?php
session_start();
//echo session_id();
require_once('config.php');

// 标记如果是管理员就用管理员身份
// 这个网页不要让外界知道，最好每过一段时间就改一下文件
$_SESSION['TRY_ADMIN'] = true; 

?><!DOCTYPE HTML><html>
<head>
<meta charset="UTF-8">
<title>管理员登录</title>
</head>
<body>
<form action="Session.php" method="post">
<?php echo @$_SESSION['Error']; unset($_SESSION['Error']);?>
<p>用户名：<input name="UserID"/></p>
<p>密码：<input type="password" name="Password"/></p>
<p><button type='submit'>登录</button></p>
</form>
<?php 
if($OpenSignup)
{
    echo "<a href=Signup.php>注册</a>";
}
?>

</body>
</html>