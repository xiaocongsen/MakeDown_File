#coding=utf-8

from appium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.common.exceptions import TimeoutException


desired_caps = {}
desired_caps['platformName'] = 'Android' # 打开什么平台的app，固定的 > 启动安卓平台
desired_caps['platformVersion'] = '13' # 安卓系统的版本号：adb shell getprop ro.build.version.release
desired_caps['deviceName'] = 'M2012K11AC' # 手机/模拟器的型号：adb shell getprop ro.product.model
desired_caps['appPackage'] = 'com.byd.aeri.caranywhere' # app的名字：
# 安卓8.1之前：adb shell dumpsys activity | findstr “mFocusedActivity”

desired_caps['appActivity'] = 'com.byd.aeri.splash.module_splash.activity.LaunchActivity' # 同上↑
# desired_caps['}unicodeKeyboard'] = True # 为了支持中文
# desired_caps['resetKeyboard'] = True # 设置成appium自带的键盘
# desired_caps["appium:dontStopAppOnReset"] = True # 不关闭应用，不重启应用
desired_caps['noReset'] = True # 使用app的缓存


driver = webdriver.Remote('http://localhost:4723/wd/hub', desired_caps)

wait = WebDriverWait(driver, 5)
try:
    button = wait.until(EC.element_to_be_clickable((By.ID, 'com.byd.aeri.caranywhere:id/btn_skip')))
    button.click()
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找啥？")

try:
    button = wait.until(EC.element_to_be_clickable((By.ID, 'com.byd.aeri.caranywhere:id/btn_my')))
    button.click()
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找我的按钮")

try:
    button = wait.until(EC.element_to_be_clickable((By.ID, 'com.byd.aeri.caranywhere:id/tv_set_main_everyday_sign')))
    button.click()
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找每日签到按钮")

try:
    button = wait.until(EC.element_to_be_clickable((By.ID, 'com.byd.aeri.caranywhere:id/tv_got_it')))
    button.click()
except TimeoutException:
    # 处理找不到元素的情况
    print("找不到已签到按钮，可能已经完成签到")
    
driver.quit()