# 反编译apk
    apktool d your_app.apk  得到your_app文件夹

# 修改
    修改your_app文件夹下的文件

# 重新打包
    apktool b your_app -o new_app.apk

# 生成签名文件
    keytool -genkey -v -keystore XCS -alias xiaocs -keyalg RSA -keysize 2048 -validity 10000

# 签名
    jarsigner -verbose -sigalg SHA1withRSA -digestalg SHA1 -keystore XCS new_app.apk xiaocs
    
# 对齐
    zipalign -v 4 new_app.apk new_app_aligned.apk

# 验证签名
    jarsigner -verify -verbose -certs new_app_aligned.apk
    jarsigner -verify my_teld_aligned.apk

# 安装
    adb install new_app_aligned.apk