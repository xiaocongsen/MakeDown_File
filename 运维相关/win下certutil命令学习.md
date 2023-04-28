# 列出计算机上安装的所有证书
    certutil -store my

# 列出计算机上安装的特定颁发机构的证书
    certutil -store my "<CA名称>"

# 查看某个证书的详细信息
    certutil -store my "<证书的序列号>"
    certutil -store -user my "<证书的序列号>"     #-user选项是查找当前用户的个人证书 序列号在证书管理器中可以找到


# 删除证书
certutil -delstore my "<证书的序列号>"
certutil -delstore -user my "<证书的序列号>"

# 输出证书信息
certutil -dump filename.crt