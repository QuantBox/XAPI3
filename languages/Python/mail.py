#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
与发送邮件相关的功能
可以加上邮件直投
"""
from email.mime.image import MIMEImage
from email.mime.text import MIMEText
from email.mime.application import MIMEApplication
from email.mime.multipart import MIMEMultipart
import datetime

import smtplib

import sys
import getopt

def add_image(src, imgid):
    """
    添加图片函数
    在HTML中使用方法 <img src=\"cid:performance\"></img>
    :param src: 图片路径
    :param imgid: 图片id，输入名需为 '<performance>'，否则iPhone上看不到图片
    :return:
    """
    fp = open(src, 'rb')
    msg = MIMEImage(fp.read())
    fp.close()  # 关闭文件
    msg.add_header('Content-ID', imgid)  # 指定图片文件的Content-ID，imgid，<img>标签中的src用到
    return msg


def add_attach(src, filename):
    """
    添加附件，并指定附件名
    :param src:
    :param filename:
    :return:
    """
    fp = open(src, 'rb')
    att = MIMEApplication(fp.read())
    fp.close()
    att["Content-Type"] = 'application/octet-stream'
    att.add_header('Content-Disposition', 'attachment', filename=('gbk', '', filename))
    return att


def add_html(html):
    """
    添加文本
    :param html:
    :return:
    """
    txt = MIMEText(html, _subtype='html', _charset='utf-8')
    return txt


def add_email(from_mail, to_list, subject):
    """
    1.直投收件箱:
    from_mail:可以是任意地址
    to_list:只有同MX服务商的才能收到，别的只能在收件人处显示

    2.通过发件箱
    from_mail:自己有效的发件箱
    to_list:任意有效地址

    :param from_mail:
    :param to_list:
    :param subject:
    :return:
    """
    msg = MIMEMultipart('related')
    msg['Subject'] = subject
    msg['From'] = from_mail
    msg['To'] = ";".join(to_list)
    return msg


def send_mail_ex(mail_host, msg, user=None, password=None):
    """
    1.直投收件箱:
    mail_host:只能是对方MX服务器
    user:必填空
    password:

    2.通过发件箱
    mail_host:自己的发件服务器
    from_mail:自己有效的发件箱
    to_list:任意有效地址
    user:自己有效的用户名
    password:自己有效的密码

    :param mail_host: 服务器域名或IP
    :param user:
    :param password:
    :return:
    """
    try:
        # s = smtplib.SMTP()
        s = smtplib.SMTP_SSL()
        s.connect(mail_host)
        # 如果有用户名和密码就进行认证，表示是连接的发件服务器
        if user is not None:
            s.login(user, password)

        s.sendmail(from_mail, to_list, msg.as_string())
        s.close()
        return True
    except Exception as e:
        print(str(e))
        return False


def mxlookup(host='163.com'):
    """
    得到MX地址，可用于直投
    以前需要安装pydns，现在改成安装dnspython
    :param host:
    :return:
    """
    # try:
    #     import DNS
    # except:
    #     pass
    # mxs = DNS.mxlookup(host)
    # MX_preference, mail_exchanger = mxs[0]
    # return mail_exchanger

    import dns.resolver
    answers = dns.resolver.query(host, 'MX')
    for rdata in answers:
        print('Host', rdata.exchange, 'has preference', rdata.preference)
    return answers[0].exchange.__str__().rstrip('.')


if __name__ == '__main__':
    from_mail = "123@qq.com"
    to_list = [
        "123@qq.com",  # wk
    ]

    opts, args = getopt.getopt(sys.argv[1:], "", ["username=", "password=", "from=", "to=", "log=", "bat="])
    for op, value in opts:
        print("参数", op, value)
        if op == "--username":
            username = value
        elif op == "--password":
            password = value
        elif op == "--from":
            from_mail = value
        elif op == "--to":
            to = value
        elif op == "--log":
            log = value
        elif op == "--bat":
            bat = value

    to_list = to.split(';')

    # 不想公开自己的账号密码
    if password == "":
        password = "123456"  #

    today = datetime.date.today()
    today_Ymd = today.strftime('%Y%m%d')

    # 显示运行的脚本文件内容
    html = ''
    filename = r'%s' % bat
    txt = open(filename, 'r').read()
    html += '</br>==%s==</br><pre>%s</pre></br>' % (filename, txt)

    # 显示日志
    try:
        filename = r'%s/%s.log' % (log, today_Ymd)
        txt = open(filename, 'r').read()
        html += '</br>==%s==</br><pre>%s</pre></br>' % (filename, txt)
    except:
        pass

    # 取最后一行显示
    x = txt.splitlines()
    subject = x[-1]

    msg = add_email(from_mail, to_list, subject)

    msg.attach(add_html(html))

    # 直投
    if send_mail_ex('smtp.qq.com', msg, username, password):
        print("发送成功")
    else:
        print("发送失败")
