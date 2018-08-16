# 私钥：
``` VIM
-----BEGIN RSA PRIVATE KEY-----
MIIEogIBAAKCAQB8vXG0ImYhLHvHhpi5FS3gd2QhxSQiU6dQ04F1OHB0yRRQ3NXF
5py2NNDw962i4WP1zpUOHh94/mg/KA8KHNJXHtQVLXMRms+chomsQCwkDi2jbgUa
4jRFN/6N3QejJ42jHasY3MJfALcnHCY3KDEFh0N89FV4yGLyDLr+TLqpRecg9pkP
nOp++UTSsxz/e0ONlPYrra/DiaBjsleAESZSI69sPD9xZRt+EciXVQfybI/2SYeA
dXMm1B7tHCcFlOxeUgqYV03VEqiC0jVMwRCd+03NU3wvEmLBvGOmNGudocWIF/y3
VOqyW1byXFLeZxl7s+Y/SthxOYXzu3mF+2/pAgMBAAECggEAK5qZbYt8wenn1uZg
6onRwJ5bfUaJjApL+YAFx/ETtm83z9ByVbx4WWT7CNC7fK1nINy20/mJrOTZkgIx
x6otiNC4+DIsACJqol+RLoo8I9pk77Ucybn65ZteOz7hVZIU+8j6LzW0KDt6yowX
e75r7G/NEpfibNc3Zz81+oDd2x+bHyGbzc9QcePIVuEzkof6jgpbWrQZU14itx9l
VxEgj/fbMccvBx8brR/l9ClmDZd9Y6TWsF1rfJpF3+DPeqFkKCiD7PGz3bs4O/Zd
ZrfV21ZNVusBW49G6bU63gQVKsOf1qGo3efbAW1HVxgTQ/lExVdcMvdenZm+ADKp
L4/wUQKBgQDOfBjn3OC2IerUFu18EgCS7pSjTSibXw+TeX3D5zwszLC091G2rGlT
5DihBUhMfesNdpoZynrs4YB6Sz9C3wSGAB8AM/tNvPhtSVtbMHmrdT2DEEKCvLkO
RNBnt+8aTu2hGRanw9aL1189gzwrmXK5ZuuURfgLrB9ihrvjo4VznQKBgQCapx13
dEA1MwapBiIa3k8hVBCoGPsEPWqM33RBdUqUsP33f9/PCx00j/akwmjgQNnBlAJo
Y7LOqPCyiwOkEf40T4IlHdzYntWQQvHhfBwqSgdkTE9tKj43Ddr7JVFRL6yMSbW3
9qAp5UX/+VzOLGAlfzJ8CBnkXwGrnKPCVbnZvQKBgQCd+iof80jlcCu3GteVrjxM
LkcAbb8cqG1FWpVTNe4/JFgqDHKzPVPUgG6nG2CGTWxxv4UFKHpGE/11E28SHYjb
cOpHAH5LqsGy84X2za649JkcVmtclUFMXm/Ietxvl2WNdKF1t4rFMQFIEckOXnd8
y/Z/Wcz+OTFF82l7L5ehrQKBgFXl9m7v6e3ijpN5LZ5A1jDL0Yicf2fmePUP9DGb
ZTZbbGR46SXFpY4ZXEQ9GyVbv9dOT1wN7DXvDeoNXpNVzxzdAIt/H7hN2I8NL+4v
EjHG9n4WCJO4v9+yWWvfWWA/m5Y8JqusV1+N0iiQJ6T4btrE4JSVp1P6FSJtmWOK
W/T9AoGAcMhPMCL+N+AvWcYt4Y4mhelvDG8e/Jj4U+lwS3g7YmuQuYx7h5tjrS33
w4o20g/3XudPMJHhA3z+d8b3GaVM3ZtcRM3+Rvk+zSOcGSwn3yDy4NYlv9bdUj/4
H+aU1Qu1ZYojFM1Gmbe4HeYDOzRsJ5BhNrrV12h27JWkiRJ4F/Q=
-----END RSA PRIVATE KEY-----
```

# Navicat Premium for Mac 破解

## 第一步
    安装 navicat120_premium_cs.dmg 不要打开软件
## 第二步
    应用程序 中找到 Navicat Premium 右键显示包含内容
    把 /Contents/Resources/rpk 文件的内容改成下面的公钥

``` VIM
-----BEGIN PUBLIC KEY-----
MIIBITANBgkqhkiG9w0BAQEFAAOCAQ4AMIIBCQKCAQB8vXG0ImYhLHvHhpi5FS3g
d2QhxSQiU6dQ04F1OHB0yRRQ3NXF5py2NNDw962i4WP1zpUOHh94/mg/KA8KHNJX
HtQVLXMRms+chomsQCwkDi2jbgUa4jRFN/6N3QejJ42jHasY3MJfALcnHCY3KDEF
h0N89FV4yGLyDLr+TLqpRecg9pkPnOp++UTSsxz/e0ONlPYrra/DiaBjsleAESZS
I69sPD9xZRt+EciXVQfybI/2SYeAdXMm1B7tHCcFlOxeUgqYV03VEqiC0jVMwRCd
+03NU3wvEmLBvGOmNGudocWIF/y3VOqyW1byXFLeZxl7s+Y/SthxOYXzu3mF+2/p
AgMBAAE=
-----END PUBLIC KEY-----
```
## 第三步
    在命令行输入 sudo spctl --master-disable 开启任何来源

## 第四步
    断网 开启应用 点击激活
    输入 注册码 NAVH-T4PX-WT8W-QBL5
    点击 激活
    点击 手动激活
    得到 请求码

## 第五步
    联网
    进入 http://tool.chacuo.net/cryptrsaprikey
    第一个填写私钥
    第二个填写请求码
    点击解密
    得到明文 类似
    {
        "K" : "NAVHT4PXWT8WQBL5",
        "P" : "Mac 10.13",
        "DI" : "ODQ2Yjg2ZDBjMTEzMjhh"
    }
## 第六步

    替换掉明文中的DI 和 时间搓(改成近期时间搓)
    {"K":"NAVHT4PXWT8WQBL5", "N":"xcs", "O":"xcs", "DI":"ODQ2Yjg2ZDBjMTEzMjhh", "T":1526939230}
    得到自己的激活明文

## 第七步
    http://tool.chacuo.net/cryptrsaprikey
    第一个填私钥
    第二个填自己的激活明文
    点击加密
    得到激活码
## 第八不
    断网
    在激活框中输入激活码
    点击激活
    Ok  联网
    结束