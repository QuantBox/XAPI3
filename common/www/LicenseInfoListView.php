<?php
session_start();
require_once('config.php');
include_once('CheckSignin.php');
require_once('EnumFun.php');

$result = $mdb2->query(
	'SELECT * FROM LicenceInfo WHERE User1 = '
	.$mdb2->quote($_SESSION['UserID'],"text")
    ." ORDER BY ID DESC"
    );
    if (PEAR::isError($result)) {
        die("{'Error':'".$result->getMessage()."'}");
    }
?><!DOCTYPE HTML><html>
<head>
<meta charset="UTF-8">
<title>授权信息列表</title>
</head>
<body>

<a href="LicenseInfoAdd.php" target="_blank">申请新授权</a>
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
  <th>删除</th>
  </tr>
  <?php
  
while ($row = $result->fetchRow())
{
   //$content1 = strval($row[strtolower('Content1')]);
   //$content2 = strval($row[strtolower('Content2')]);
   $product = $row[strtolower('Product')];
   $status = $row[strtolower('Status')];
   echo "<tr>";
   echo "<td>".$row[strtolower('ID')]."</td>";
   echo "<td>".$row[strtolower('User1')]."</td>";
   echo "<td>".$row[strtolower('Time1')]."</td>";
   echo "<td>".$row[strtolower('User2')]."</td>";
   echo "<td>".$row[strtolower('Time2')]."</td>";
   echo "<td>".$product."</td>";
   echo "<td>".EnumStatusToString($status)."</td>";
   echo "<td>".$row[strtolower('Remark')]."</td>";
   echo "<td><a href='LicenseInfoView.php?ID=".$row[strtolower('ID')]."' target='_blank'>查看或下载</a></td>";
   if($status<4)
   {
        echo "<td><a href='LicenseInfoAction.php?Action=Delete&ID=".$row[strtolower('ID')]."'>删除</a></td>";
   }
   else
   {
        echo "<td></td>";
   }
   
   echo "</tr>";
}
?>
</table>

</body>
</html>