<?php
session_start();
require_once('config.php');
include_once('CheckSignin.php');
require_once('DirFileFun.php');
require_once('JSON.php');


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
        echo "您没有下载权限";
        return;       
    }
}

if($row[strtolower('Status')]<3)
{
    header("Location:".getenv("HTTP_REFERER"));
    return;
}

$content2 = strval($row[strtolower('Content2')]);

$path = "temp";

if(!file_exists($path))
{
    mkdir($path);
}

$result = $mdb2->query(
	'SELECT * FROM UserInfo '
	.' WHERE UserID = '
	.$mdb2->quote($_SESSION['UserID'],"text")
    );
	if (PEAR::isError($result)) {
		die("{'Error':'".$result->getMessage()."'}");
	}
$row2 = $result->fetchRow();

if(!empty($row2[strtolower("SessionID")]))
{
   if(session_id() != $row2[strtolower("SessionID")])
    {
        $path3 = $path."/".$row2[strtolower("SessionID")];
        my_del_dir($path3);
    }
}

$result = $mdb2->query(
	'UPDATE UserInfo SET SessionID = '
	.$mdb2->quote(session_id(),"text")
	.' WHERE UserID = '
	.$mdb2->quote($_SESSION['UserID'],"text")
    );
	if (PEAR::isError($result)) {
		die("{'Error':'".$result->getMessage()."'}");
	}

$path = $path."/".session_id();

if(!file_exists($path))
{
    mkdir($path);
}
$Product = $row[strtolower("Product")];
$path1 = "$path/$Product.License";
$path2 = "$path/$Product.Signature";

?><!DOCTYPE HTML><html>
<head>
<meta charset="UTF-8">
<title>授权生成</title>
</head>
<body>
<?php
if(write_ini_file(json_decode($content2,true), $path1, true))
{
    $LicensePath = $path1;
    echo "生成授权文件成功<br/>";
    
    $cmd = "cscript.exe $VbsPath \"$LicensePath\"";
    $ret = exec($cmd);
    //echo $ret;
    if("OK" == $ret)
    {
         echo "编码转换成功<br/>";
    }
    else
    {
        echo "编码转换失败<br/>";
        return;
    }
    
    echo "<a href=$path1>下载License文件，请鼠标右键->链接另存为</a><br/>";
    
    $PrivateKeyPath = "$PrivateKeyDir\\$Product\\$Product.PrivateKey";

    $cmd = "$ExePath --License \"$LicensePath\" --PrivateKey \"$PrivateKeyPath\" ";
    $ret = exec($cmd);
    // 判断是否执行成功
    if("OK" == $ret)
    {
        echo "生成签名文件成功<br/>";
        echo "<a href=$path2>下载Signature文件，请鼠标右键->链接另存为</a><br/>";
        
        // 更新下载状态
        $result = $mdb2->query(
        'UPDATE LicenceInfo SET Status = 4'
        .' WHERE ID = '
        .$mdb2->quote($_GET['ID'],"integer")
        );
        if (PEAR::isError($result)) {
            die("{'Error':'".$result->getMessage()."'}");
        }
    }
    else
    {
        echo "生成签名文件失败<br/>";
    }
}
else
{
    echo "生成License失败<br/>";
}

?>
<p>请将License文件和Signature文件放到与dll同目录下</p>
<p>如果你拿到的授权文件为Xxx.License和Xxx.Signature，但dll名为Xxx_x86.dll，请将两个授权名改为Xxx_x86.License和Xxx_x86.Signature。
因为有可能授权分发者对x86与x64使用相同的授权</p>
</body>
</html>