
pk12util -d sql:$HOME/.pki/nssdb -i ./c/xxx.p12         SSL客户端身份验证添加个人证书和私钥
pk12util -o xxx.p12 -n xxx -d sql:$HOME/.pki/nssdb      导出证书和私钥

certutil -d sql:$HOME/.pki/nssdb -L      显示证书


certutil -d sql:$HOME/.pki/nssdb -A -t "C,," -n XCSCA -i ./XCSCA.crt         添加一个根证书到数据库中，作为信任证书
certutil -d sql:$HOME/.pki/nssdb -A -t ",," -n XCSCA -i ./XCSCA.crt         添加一个中间证书到数据库中，作为信任证书
certutil -d sql:$HOME/.pki/nssdb -A -t "P,," -n XCSCA -i ./XCSCA.crt        添加一个私有证书到数据库中，作为信任证书

certutil -d sql:$HOME/.pki/nssdb -D -n XCSCA                删除

certutil -L -n xxx -d .pki/nssdb/       查询某个证书信息



# 公共根证书位置：

"/etc/ssl/certs/ca-certificates.crt",                // Debian/Ubuntu/Gentoo etc.
"/etc/pki/tls/certs/ca-bundle.crt",                  // Fedora/RHEL 6
"/etc/ssl/ca-bundle.pem",                            // OpenSUSE
"/etc/pki/tls/cacert.pem",                           // OpenELEC
"/etc/pki/ca-trust/extracted/pem/tls-ca-bundle.pem", // CentOS/RHEL 7

"/etc/ssl/certs",               // SLES10/SLES11, https://golang.org/issue/12139
"/system/etc/security/cacerts", // Android
"/usr/local/share/certs",       // FreeBSD
"/etc/pki/tls/certs",           // Fedora/RHEL
"/etc/openssl/certs",           // NetBSD
/etc/pki/CA/certs               //Amazon Linux