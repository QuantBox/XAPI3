<?php
error_reporting(E_ALL & ~(E_STRICT | E_NOTICE|E_DEPRECATED));
//error_reporting(E_ALL);

// 时区设置，这个地方是用来处理mysql时间与php时间用
date_default_timezone_set('PRC');
// 审核后还有多少时间内可以再审核
$ReviewTimeLimit2 = 3600*3;
// 管理员有一天的时间来再审核
$ReviewTimeLimit99 = 3600*24;

// ======数据库部分======
$user = 'root';
$pass = '123456';
$host = 'localhost';
$db_name = 'test';

$dsn = "mysql://$user:$pass@$host/$db_name";

require_once('MDB2.php');

$mdb2 =& MDB2::factory($dsn);
if (PEAR::isError($mdb2)) {
	die("{'Error':'".$mdb2->getMessage()."'}");
}
$mdb2->setFetchMode(MDB2_FETCHMODE_ASSOC);
$mdb2->exec("SET NAMES 'utf8'");

//=================================================


// 这两个参数用于生成授权使用，需要检查授权是否可用
$ExePath = "D:\AppServ\www\License.exe";
$VbsPath = "D:\AppServ\www\ConvertCode.vbs";
//密钥目录一定不能在网页目录下，不然就被人下载
$PrivateKeyDir = "D:\AppServ\keys";

// 是否开放公众注册
$OpenSignup = true;

// 未处理的授权条目上限
$PendingLicenseCount = 5;

// 首页，如果改动了首页文件，此处也可能需要改变
// 目前在Session.php和SwitchUserID.php中出现
$Homepage = "Home.php";

?>