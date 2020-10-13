<?php
session_start();
require_once('config.php');
?><!DOCTYPE HTML>
<html>
<head>
<meta charset="UTF-8">
<title>注册</title>
</head>
<body>

<?php
if(@$_SESSION['Right'] == 99)
{
}
else if($OpenSignup == false)
{
    echo "对不起，目前不开放注册";
    return;
}
?>

<form action="Join.php" method="post">
<?php echo @$_SESSION['Error']; unset($_SESSION['Error']);?>
<p>用户名<input name="UserID"/></p>
<p>密码<input type="password" name="Password"/></p>
<p>密码2<input type="password" name="Password2"/>再次输入密码</p>
<p><button type='submit'>注册</button></p>
</form>
</body>
</html>