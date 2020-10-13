<?php
session_start();
require_once('config.php');
//include_once('CheckSignin.php');
require_once('EnumFun.php');

$result = $mdb2->query(
	'SELECT * FROM ProductInfo ');
if (PEAR::isError($result)) {
	die("{'Error':'".$result->getMessage()."'}");
}
?><!DOCTYPE HTML><html>
<head>
<meta charset="UTF-8">
<title>产品信息列表</title>
</head>
<body>
<p>如想登记自己开发的组件，请与管理员联系 <a href="aboutus.php" target="_blank">关于我们</a></p>
<hr/>
<table border="1">
  <tr>
  <th>ID</th>
  <th>产品名</th>
  <th>介绍页</th>
  </tr>
  <?php
while ($row = $result->fetchRow())
{
   echo "<tr>";
   echo "<td>".$row[strtolower('ID')]."</td>";
   echo "<td>".$row[strtolower('Name')]."</td>";
   echo "<td><a href='".$row[strtolower('Url')]."'>查看并下载</a></td>";
   echo "</tr>";
}
?>
</table>

</body>
</html>
