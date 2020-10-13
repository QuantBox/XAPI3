<?php
session_start();

require_once('DirFileFun.php');
$path = "temp/".session_id();
my_del_dir($path);

// 主要是不想用同一ID就得加上
$lifeTime = -3600;
setcookie(session_name(), session_id(), time() + $lifeTime, "/");

session_unset();
session_destroy();

header("Location:Signin.php");
return;
?>