#coding=utf-8

from appium import webdriver
from selenium.webdriver.common.by import By
from time import sleep

desired_caps = {}
desired_caps['platformName'] = 'Android' # 打开什么平台的app，固定的 > 启动安卓平台
desired_caps['platformVersion'] = '13' # 安卓系统的版本号：adb shell getprop ro.build.version.release
desired_caps['deviceName'] = 'M2012K11AC' # 手机/模拟器的型号：adb shell getprop ro.product.model
desired_caps['appPackage'] = 'com.byd.aeri.caranywhere' # app的名字：
# 安卓8.1之前：adb shell dumpsys activity | findstr “mFocusedActivity”
# 安卓8.1之后：adb shell dumpsys activity | findstr “mResume”
desired_caps['appActivity'] = 'com.byd.aeri.splash.module_splash.activity.LaunchActivity' # 同上↑
desired_caps['unicodeKeyboard'] = True # 为了支持中文
desired_caps['resetKeyboard'] = True # 设置成appium自带的键盘
desired_caps['noReset'] = True # 使用app的缓存


driver = webdriver.Remote('http://localhost:4723/wd/hub', desired_caps)
# button = driver.find_element(By.ID, 'com.miui.securitycenter:id/cta_positive')
# button.click()
sleep(3)

button = driver.find_element(By.ID, 'com.byd.aeri.caranywhere:id/btn_my')
button.click()
button = driver.find_element(By.ID, 'com.byd.aeri.caranywhere:id/tv_set_main_everyday_sign')
button.click()
sleep(1)
button = driver.find_element(By.ID, 'com.byd.aeri.caranywhere:id/tv_got_it')
button.click()

driver.quit()