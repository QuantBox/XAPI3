<?php
session_start();
require_once('config.php');
include_once('CheckSignin.php');

if(isset($_POST['ModifyInfo']))
{
    $result = $mdb2->query(
    'UPDATE UserInfo SET Name = '
    .$mdb2->quote($_POST['Name'],"text")
    .' , Phone = '
    .$mdb2->quote($_POST['Phone'],"text")
    .' , Email = '
    .$mdb2->quote($_POST['Email'],"text")
    .' , IM = '
    .$mdb2->quote($_POST['IM'],"text")
    .' , Remark = '
    .$mdb2->quote($_POST['Remark'],"text")
    .' WHERE ID = '
    .$mdb2->quote($_SESSION['ID'],"integer")
    );
    if (PEAR::isError($result)) {
        die("{'Error':'".$result->getMessage()."'}");
    }

    header("Location:".getenv("HTTP_REFERER"));
    return;
}

if(isset($_POST['ModifyPassword']))
{
    if($_SESSION['Password'] != md5($_POST['OldPassword']))
    {
        $_SESSION['Error'] = '原密错误';
        header("Location:".getenv("HTTP_REFERER"));
        return;
    }
    
    if($_POST['NewPassword'] != $_POST['NewPassword2'])
    {
        $_SESSION['Error'] = '两次密码不相同';
        header("Location:".getenv("HTTP_REFERER"));
        return;
    }
    
    if($_SESSION['Password'] == $_POST['NewPassword'])
    {
        $_SESSION['Error'] = '新密码与原密码相同';
        header("Location:".getenv("HTTP_REFERER"));
        return;
    }

    $result = $mdb2->query(
    'UPDATE UserInfo SET Password = '
    .$mdb2->quote(md5($_POST['NewPassword']),"text")
    .' WHERE ID = '
    .$mdb2->quote($_SESSION['ID'],"integer")
    );
    if (PEAR::isError($result)) {
        die("{'Error':'".$result->getMessage()."'}");
    }
    
    header("Location:Logout.php");
    return;
}
?>