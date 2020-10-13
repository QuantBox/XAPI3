<?php
session_start();
require_once('config.php');
include_once('CheckSignin.php');
require_once('EnumFun.php');

$result = $mdb2->query(
	'SELECT * FROM Product_User2 ORDER BY Product');
if (PEAR::isError($result)) {
	die("{'Error':'".$result->getMessage()."'}");
}
?>
<!DOCTYPE HTML>
<html>
<head>
<meta charset="UTF-8">
<title>审核人员列表</title>
</head>
<body>

<p>只是给管理员核对使用，由于一般很少有改动，所以直接编辑数据库即可。</p>
<p>如果要给某个用户添加审核功能，还需要改用户信息表的Right,2=<?php echo EnumRightToString(2); ?>，否则审核员是看不到可审核列表的</p>
<table border="1">
  <tr>
  <th>ID</th>
  <th>Product</th>
  <th>User2</th>
  </tr>
  <?php
  
while ($row = $result->fetchRow())
{
   echo "<tr>";
   echo "<td>".$row[strtolower('ID')]."</td>";
   echo "<td>".$row[strtolower('Product')]."</td>";
   echo "<td>".$row[strtolower('User2')]."</td>";
   echo "</tr>";
}
?>
</table>

</body>
</html>