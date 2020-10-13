<?php
session_start();
require_once('config.php');
include_once('CheckSignin.php');


$query = 'SELECT * FROM UserInfo WHERE UserID = '
		.$mdb2->quote($_SESSION['UserID'],"text");
$result = $mdb2->query($query);

$row = $result->fetchRow();
// 不显示密码
unset($row[strtolower('Password')]);
?><!DOCTYPE HTML>
<html>
<head>
<meta charset="UTF-8">
<title>个人信息</title>
</head>
<body>
<?php
echo "<h1>个人信息</h1><pre>";
print_r($row);
echo "</pre>";
?>
<hr/>
<form action="UserInfoAction.php" method="post">
<h1>修改个人信息</h1>
<p>以下是用来完善个人信息使用，如已经通过审核可不用修改。请认真填写信息。</p>
<p>Name: <input type="text" name="Name" value="<?php echo $row[strtolower('Name')];?>"/>真实姓名</p>
<p>Phone: <input type="text" name="Phone" value="<?php echo $row[strtolower('Phone')];?>"/>电话，手机号或固定电话都可以</p>
<p>Email: <input type="text" name="Email" value="<?php echo $row[strtolower('Email')];?>"/>电子邮箱地址</p>
<p>QQ: <input type="text" name="IM" value="<?php echo $row[strtolower('IM')];?>"/>即时聊天工具，如QQ\微信等</p>
<p>Remark: <textarea rows="10" cols="60" name="Remark"><?php echo $row[strtolower('Remark')];?></textarea>备注信息,请填写公司、职务等信息</p>
<input type="submit" value="Submit" name="ModifyInfo"/>
</form>
<hr/>
<form action="UserInfoAction.php" method="post">
<h1>修改密码</h1>
<p>修改成功后将不提示，直接跳转到重新登录界面</p>
<?php echo @$_SESSION['Error']; unset($_SESSION['Error']);?>
<p>原始密码: <input type="password" name="OldPassword" value=""/></p>
<p>新密码: <input type="password" name="NewPassword" value=""/></p>
<p>确认密码: <input type="password" name="NewPassword2" value=""/></p>
<input type="submit" value="Submit" name="ModifyPassword"/>

</form>

</body>
</html>