<?php
session_start();
require_once('config.php');
include_once('CheckSignin.php');


if(empty($_POST))
{
    if(isset($_GET['Action']) && $_GET['Action']=='Delete')
    {
        $query = 'DELETE FROM LicenceInfo '
        .' WHERE ID = '
        .$mdb2->quote($_GET['ID'],"integer")
        .' AND Status <4 ';
        
        // 只能删除自己的吗？管理员可以更新所有的
        if($_SESSION['Right'] <= 2)
        {
            $query = $query.' AND User1 = '.$mdb2->quote($_SESSION['UserID'],"text");
        }
        
        $result = $mdb2->query($query);
        
        if (PEAR::isError($result)) {
            die("{'Error':'".$result->getMessage()."'}");
        }
        
        header("Location:".getenv("HTTP_REFERER"));
        return;
    }
}

if(get_magic_quotes_gpc()){ 
    function stripslashes_deep($value){ 
         $value=is_array($value)?array_map('stripslashes_deep',$value):stripslashes($value); 
         return $value; 
    } 
    $_POST=array_map('stripslashes_deep',$_POST); 
    $_GET=array_map('stripslashes_deep',$_GET); 
    $_COOKIE=array_map('stripslashes_deep',$_COOKIE); 
    $_REQUEST=array_map('stripslashes_deep',$_REQUEST); 
}

$sampleData = array(
                'License' => array(
                    'ExpireDate' => $_POST['ExpireDate'],
                    'Trial' => $_POST['Trial'],
                    'MachineID' => $_POST['MachineID'],
                    'Product' => $_POST['Product'],
                ),
                'User' => array(
                    'Account' => $_POST['Account'],
                    'UserName' => $_POST['UserName'],
                    'Info' => $_POST['Info'],
                ));

$content1 = json_encode($sampleData);

// 接收各种提交，进行处理

// 1.用户新建一个授权
// 2.用户编辑授权
// 3.审核员审核左边
// 4.审核员审核右边

if(isset($_POST['Create']))
{
    $query = "SELECT COUNT(*) AS NUM_A FROM LicenceInfo WHERE "
            ." User1 = ".$mdb2->quote($_SESSION['UserID'],"text")
            ." AND Status <3 ";
    //echo $query;
    $result = $mdb2->query($query);
    $row = $result->fetchRow();
    //print_r($row);
    //echo $row[strtolower('NUM_A')];
    $count = 5;
    if($row[strtolower('NUM_A')]>$count)
    {
        echo "非审核的授权超过 $count 个，请等部分批准后再新申请";
        return;
    }
    // 插入用户
    $query = 'INSERT INTO LicenceInfo('
        .'User1,Time1,Content1,'
        .'Product,Status,Remark) '
        .' VALUES('
        .$mdb2->quote($_SESSION['UserID'],"text").','
        .'NOW(),'
        .$mdb2->quote($content1,"text").','
        .$mdb2->quote($_POST['Product'],"text").','
        .$mdb2->quote("1","text").','
        .$mdb2->quote($_POST['Remark'],"text")
        .')';
    
    $result = $mdb2->query($query);
    
    if (PEAR::isError($result)) {
        die("{'Error':'".$result->getMessage()."'}");
    }
    
    header("Location:LicenseInfoListView.php");
    return;
}

if(isset($_POST['Update']))
{
    $query = 'UPDATE LicenceInfo SET '
	.' Time1 = NOW(),Content1 = '
	.$mdb2->quote($content1,"text")
    .' , Remark = '
    .$mdb2->quote($_POST['Remark'],"text")
    .' , Status = 1'
	.' WHERE ID = '
	.$mdb2->quote($_GET['ID'],"integer");
    
    // 只能更新自己的吗？管理员可以更新所有的
    if($_SESSION['Right'] <= 2)
    {
        $query = $query.' AND User1 = '.$mdb2->quote($_SESSION['UserID'],"text");
    }

    $result = $mdb2->query($query);
	if (PEAR::isError($result)) {
		die("{'Error':'".$result->getMessage()."'}");
	}
    
    header("Location:".getenv("HTTP_REFERER"));
    return;
}

if(isset($_POST['Review_Left'])||isset($_POST['Review_Right']))
{    
    if(in_array($_POST['Product'],$_SESSION['Products'],true))
    {
        
    }
    else
    {
        if($_SESSION['Right'] > 2)
        {
        }
        else
        {
            echo "您没有权限审核此授权信息";
            return;
        }
    }
    
    $query = 'UPDATE LicenceInfo SET User2 = '
    .$mdb2->quote($_SESSION['UserID'],"text")
	.' , Time2 = NOW(),Content2 = '
	.$mdb2->quote($content1,"text")
    .' , Status = '
    .$mdb2->quote($_POST['Status'],"text")
    .' , Remark = '
    .$mdb2->quote($_POST['Remark'],"text")
	.' WHERE ID = '
	.$mdb2->quote($_GET['ID'],"integer")
    .' AND Product = '
	.$mdb2->quote($_POST['Product'],"text")
    ;
    
    $result = $mdb2->query($query);
    
	if (PEAR::isError($result)) {
		die("{'Error':'".$result->getMessage()."'}");
	}
    
    header("Location:".getenv("HTTP_REFERER"));
    return;
}
?>