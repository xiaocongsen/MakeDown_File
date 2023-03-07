adb devices                                                     # 查看连接的设备
adb shell                                                       # 进入Android设备的shell环境。
adb shell pm list packages                                      # 查看所有安装的包名
adb push <local> <remote>                                       # 将本地文件复制到 Android 设备。   
adb pull <remote> <local>                                       # 将 Android 设备上的文件复制到本地。
adb install path/to/app.apk                                     # 安装apk
adb install path/to/app.apk                                     # 卸载apk
adb get-state                                                   # 查看设备状态
adb shell am start -n <package-name>/<activity-name>            # 启动app
adb shell am force-stop <package-name>                          # 强制停止app
adb shell pm clear package_name                                 # 清除app数据 
adb shell screencap -p /sdcard/screenshot.png                   # 截图
adb shell screenrecord <filename>                               # 在 Android 设备上录制屏幕并保存到指定的文件。
adb reboot                                                      # 重启Android设备。
adb reboot recovery                                             # 进入Android设备的recovery模式。
adb reboot bootloader                                           # 进入Android设备的bootloader模式。
adb logcat                                                      # 查看 Android 设备的日志信息。
adb shell dumpsys                                               # 显示设备的状态信息。
adb shell input keyevent <keycode>                              # 发送指定的键盘事件到 Android 设备。
adb shell input text <text>                                     # 在 Android 设备上输入指定的文本。

adb shell cmd package resolve-activity --brief <package-name>   # 查看app的入口activity
adb shell getprop ro.build.version.release                      # 查看Android版本
adb shell getprop ro.product.model                              # 查看设备型号

adb shell am monitor                                            # 监控activity的生命周期 输出打开关闭app的包名









adb shell wm size <width>x<height> - 设置 Android 设备的屏幕分辨率。
adb shell pm grant <package-name> <permission> - 授予应用指定的权限。
adb shell pm revoke <package-name> <permission> - 撤销应用指定的权限。
adb shell settings put system <key> <value> - 设置 Android 设备系统设置。
adb shell getprop <property> - 获取 Android 设备的系统属性。
adb shell dumpsys window windows | grep -E 'mCurrentFocus|mFocusedApp' - 显示当前的窗口焦点和正在运行的应用。
adb shell input tap <x> <y> - 在 Android 设备上模拟触摸屏幕操作。
adb shell input swipe <x1> <y1> <x2> <y2> - 在 Android 设备上模拟滑动屏幕操作。
adb shell input keyevent --longpress <keycode> - 发送指定的键盘长按事件到 Android 设备。
adb shell dumpsys battery - 显示 Android 设备的电池信息。
adb shell pm list packages -f - 显示所有已安装的应用及其安装路径。
adb shell pm path <package-name> - 显示指定应用的安装


adb shell uiautomator           # 用于自动化测试的工具

