<?php
session_start();
require_once('config.php');
include_once('CheckSignin.php');
require_once('EnumFun.php');

$result = $mdb2->query(
	'SELECT * FROM LicenceInfo WHERE '
    ." ID = "
    .$mdb2->quote($_GET['ID'],"integer")
    );
if (PEAR::isError($result)) {
	die("{'Error':'".$result->getMessage()."'}");
}
$row = $result->fetchRow();

$product = $row[strtolower('Product')];

if(in_array($product,$_SESSION['Products'],true))
{
}
else
{
    if($_SESSION['Right'] > 2)
    {
    }
    else if($_SESSION['UserID'] == $row[strtolower('User1')])
    {
    }
    else
    {
        echo "您没有权限查看此授权信息";
        return;       
    }
}

$content1 = strval($row[strtolower('Content1')]);
$content2 = strval($row[strtolower('Content2')]);

$json_content1 = json_decode($content1,true);
$json_content2 = json_decode($content2,true);
?><!DOCTYPE HTML><html>
<head>
<meta charset="UTF-8">
<title>授权信息</title>
</head>
<body>

<table border="1">
  <tr>
  <th>ID</th>
  <td><?php echo $row[strtolower('ID')];?></td>
  <th>产品</th>
  <td><?php echo $row[strtolower('Product')];?></td>
  </tr>
  <tr>
  <th>申请人员</th>
  <td><?php echo $row[strtolower('User1')];?></td>
  <th>审核人员</th>
  <td><?php echo $row[strtolower('User2')];?></td>
  </tr>
  <tr>
  <th>申请时间</th>
  <td><?php echo $row[strtolower('Time1')];?></td>
  <th>审核时间</th>
  <td><?php echo $row[strtolower('Time2')];?></td>
  </tr>
  <tr>
  <th>申请内容</th>
  <td><pre><?php print_r($json_content1);?></pre></td>
  <th>审核内容</th>
  <td><pre><?php print_r($json_content2);?></pre></td>
  </tr>
  <tr>
  <th>状态</th>
  <td><?php echo EnumStatusToString($row[strtolower('Status')]);?></td>
  <th>备注</th>
  <td><?php echo $row[strtolower('Remark')];?></td>
  </tr>
</table>
<?php
// 审核员也可以下载，因为可能需要检查一下是否可用
if($row[strtolower('Status')]>=3)
{
?>
<hr/>
<h1>下载</h1>
<p><?php echo "<a href='LicenseInfoGenerate.php?ID=".$row[strtolower('ID')]."' target='_blank'>生成授权并下载</a>"?></p>
<?php
}

// 审核员是否能修改呢申请信息呢？不能修改才对
if($row[strtolower('Status')]<3 && $_SESSION['UserID'] == $row[strtolower('User1')])
{
?>
<hr/>
<form action="LicenseInfoAction.php?ID=<?php echo $row[strtolower('ID')];?>" method="post">
<h1>在没有通过审核人员审核前，是可以再编辑的</h1>
<p>Product: <input type="text" name="Product" readonly value="<?php echo $row[strtolower('Product')];?>"/></p>
<p>ExpireDate: <input type="text" name="ExpireDate" value="<?php echo $json_content1['License']['ExpireDate'];?>"/>到期时间，格式：例如20160315</p>
<p>Trial: <input type="text" name="Trial" value="<?php echo $json_content1['License']['Trial'];?>"/>单次会话下单次数，0表示无限制</p>
<p>MachineID: <input type="text" name="MachineID" value="<?php echo $json_content1['License']['MachineID'];?>"/>机器码，如果没有绑定账号就必须绑定机器码，支持正则</p>
<p>Account: <input type="text" name="Account" value="<?php echo $json_content1['User']['Account'];?>"/>登录时所用的客户号，支持正则</p>
<p>UserName: <input type="text" name="UserName" value="<?php echo $json_content1['User']['UserName'];?>"/>接口登录后所能取到的客户真实姓名，不支持正则</p>
<p>Info: <input type="text" name="Info" value="<?php echo $json_content1['User']['Info'];?>"/>额外信息，一般由审核员填写，会写入到授权文件中</p>
<hr/>
<p>上面的信息如果不会填写，那就把*.Licence中的文本直接粘贴到下面的备注即可，后面的事交审核人员处理</p>
<p>备注: <textarea rows="5" cols="60" name="Remark"><?php echo $row[strtolower('Remark')];?></textarea>备注，需告知审核员或用户的消息</p>
<input type="submit" value="Submit" name="Update"/>
</form>
<?php }?>

</body>
</html>
