<?php
session_start();
require_once('config.php');
include_once('CheckSignin.php');


$query = "SELECT COUNT(*) AS NUM_A FROM LicenceInfo WHERE "
        ." User1 = ".$mdb2->quote($_SESSION['UserID'],"text")
        ." AND Status <3 ";
//echo $query;
$result = $mdb2->query($query);
$row = $result->fetchRow();
//print_r($row);
//echo $row[strtolower('NUM_A')];
if($row[strtolower('NUM_A')]>$PendingLicenseCount)
{
    echo "未审核的授权超过 $PendingLicenseCount 个，请等部分批准后再新申请";
    return;
}

$result = $mdb2->query(
	'SELECT * FROM ProductInfo ');
if (PEAR::isError($result)) {
	die("{'Error':'".$result->getMessage()."'}");
}
?>
<!DOCTYPE HTML><html>
<head>
<meta charset="UTF-8">
<title>申请授权</title>
</head>
<body>

<form method="post" action="LicenseInfoAction.php" method="post">
<p>注意：您的申请经过人工审核后才能通过，请准确填写。被打回来后重新填写将耽误双方的时间</p>
<p>Product: <select name="Product">
<?php
while ($row = $result->fetchRow())
{
   $name = $row[strtolower('Name')];
   echo "<option value=\"$name\">$name</option>";
}?>
</select>产品ID</p>
<p>ExpireDate: <input type="text" name="ExpireDate" value="<?php echo date("Ymd")?>"/>到期时间，格式：例如20160315</p>
<p>Trial: <input type="text" name="Trial" value="5"/>单次会话下单次数，0表示无限制</p>
<p>MachineID: <input type="text" name="MachineID" value=".*"/>机器码，如果没有绑定账号就必须绑定机器码，支持正则</p>
<p>Account: <input type="text" name="Account" value=".*"/>登录时所用的客户号，支持正则</p>
<p>UserName: <input type="text" name="UserName" value=""/>接口登录后所能取到的客户真实姓名，不支持正则</p>
<p>Info: <input type="text" name="Info" value=""/>额外信息，一般由审核员填写，会写入到授权文件中</p>
<hr/>
<p>上面的信息如果不会填写，那就把*.Licence中的文本直接粘贴到下面的备注即可，后面的事交审核人员处理</p>
<p>Remark: <textarea rows="5" cols="60" name="Remark"><?php echo $row[strtolower('Remark')];?></textarea>备注，需告知审核员的消息</p>
<input type="submit" value="Submit" name="Create"/>
</form>

</body>
</html>