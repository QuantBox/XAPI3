<?php
// 授权信息的状态
function EnumStatusToString($status)
{
    switch($status)
    {
    case 1:
        return "等待审核，可再编辑";
    case 2:
        return "审核拒绝，需重新编辑";
    case 3:
        return "审核通过,可下载,可再审核";
    case 4:
        return "已下载，不可编辑和删除";
    default:
        return "未知状态".$status;
    }
}

function EnumRightToString($right)
{
    switch($right)
    {
    case 0:
        return "注册用户";
    case 1:
        return "普通用户";
    case 2:
        return "审核员";
    case 99:
        return "管理员";
    default:
        return "未知用户".$right;
    }
}
?>