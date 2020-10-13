<?php
session_start();
require_once('config.php');
include_once('CheckSignin.php');
require_once('EnumFun.php');

$result2 = $mdb2->query(
	'SELECT * FROM ProductInfo ');
if (PEAR::isError($result2)) {
	die("{'Error':'".$result2->getMessage()."'}");
}

$query = "SELECT * FROM LicenceInfo WHERE 1=1 ";
if(!empty($_POST['Product']))
{
    $query = $query." AND Product = ".$mdb2->quote($_POST['Product'],"text");
}
if(!empty($_POST['Status']))
{
    $query = $query." AND Status = ".$mdb2->quote($_POST['Status'],"text");
}
if(!empty($_POST['User1']))
{
    $query = $query." AND User1 = ".$mdb2->quote($_POST['User1'],"text");
}
if(!empty($_POST['User2']))
{
    $query = $query." AND User2 = ".$mdb2->quote($_POST['User2'],"text");
}
$query = $query." ORDER BY ID DESC";

$result = $mdb2->query($query);

?><!DOCTYPE HTML><html>
<head>
<meta charset="UTF-8">
<title>授权信息列表</title>
</head>
<body>

<p>您有以下产品的审核权限:<?php echo implode(";",$_SESSION['Products'])?></p>
<form method="post">
<p>申请人员:<input type="text" name="User1" value="<?php if(empty($_POST))
{
}
else
{
    echo $_POST['User1'];
}
?>"/>审核人员:<input type="text" name="User2" value=""/>
产品:<select name="Product">
<?php
echo "<option value=>--ALL--</option>";
while ($row2 = $result2->fetchRow())
{
   $product = $row2[strtolower('Name')];
   echo "<option value=\"$product\">$product</option>";
}?>
</select>
状态: <select name="Status">
  <option value ="">--ALL--</option>
  <option value ="1"><?php echo EnumStatusToString(1);?></option>
  <option value ="2"><?php echo EnumStatusToString(2);?></option>
  <option value ="3"><?php echo EnumStatusToString(3);?></option>
  <option value ="4"><?php echo EnumStatusToString(4);?></option>
</select><input type="submit" value="查询" /></p>
</form>
<hr/>
<table border="1">
  <tr>
  <th>ID</th>
  <th>申请人员</th>
  <th>申请时间</th>
  <th>审核人员</th>
  <th>审核时间</th>
  <th>产品</th>
  <th>状态</th>
  <th>备注</th>
  <th>详情</th>
  <th>审核</th>
  </tr>
  <?php
  
while ($row = $result->fetchRow())
{
   if(in_array($product,$_SESSION['Products'],true))
   {
       $product = $row[strtolower('Product')];
       $status = $row[strtolower('Status')];
       echo "<tr>";
       echo "<td>".$row[strtolower('ID')]."</td>";
       echo "<td>".$row[strtolower('User1')]."</td>";
       echo "<td>".$row[strtolower('Time1')]."</td>";
       echo "<td>".$row[strtolower('User2')]."</td>";
       echo "<td>".$row[strtolower('Time2')]."</td>";;
       echo "<td>".$product."</td>";
       echo "<td>".EnumStatusToString($status)."</td>";
       echo "<td>".$row[strtolower('Remark')]."</td>";
       echo "<td><a href='LicenseInfoView.php?ID=".$row[strtolower('ID')]."' target='_blank'>查看或下载</a></td>";
       echo "<td><a href='LicenseInfoReview.php?ID=".$row[strtolower('ID')]."' target='_blank'>审核</a></td>";
       echo "</tr>";
   }
}
?>
</table>

</body>
</html>