<?php
session_start();
//echo session_id();
require_once('config.php');

?><!DOCTYPE HTML><html>
<head>
<meta charset="UTF-8">
<title>登录</title>
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