# This sample code uses the Appium python client v2
# pip install Appium-Python-Client
# Then you can paste this into a file and simply run with Python

from appium import webdriver
from appium.webdriver.common.appiumby import AppiumBy
from time import sleep
# For W3C actions
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.common.actions import interaction
from selenium.webdriver.common.actions.action_builder import ActionBuilder
from selenium.webdriver.common.actions.pointer_input import PointerInput
from selenium.webdriver.common.by import By
from selenium.common.exceptions import NoSuchElementException
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.common.exceptions import TimeoutException

caps = {}
caps["platformName"] = "Android"
caps["appium:deviceName"] = "M2012K11AC"
caps["appium:appPackage"] = "com.baidu.netdisk"
caps["appium:appActivity"] = ".ui.DefaultMainActivity"
caps["appium:platformVersion"] = "13"
caps["appium:noReset"] = True
# caps["appium:unicodeKeyboard"] = True
# # caps["appium:resetKeyboard"] = True
caps["appium:dontStopAppOnReset"] = False
caps["appium:ensureWebviewsHavePages"] = True
caps["appium:nativeWebScreenshot"] = True
caps["appium:newCommandTimeout"] = 3600
caps["appium:connectHardwareKeyboard"] = True

driver = webdriver.Remote("http://127.0.0.1:4723/wd/hub", caps)
wait = WebDriverWait(driver, 3)
try:
    button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.TextView[1]')))
    button.click()
    print("点击广告跳过按钮")
except TimeoutException:
    # 处理找不到元素的情况
    print("无法找到广告跳过按钮")
wait = WebDriverWait(driver, 1, 0.5)

try:
    button = wait.until(EC.element_to_be_clickable((By.ID, 'com.baidu.netdisk:id/new_quick_settings_done')))
    button.click()
except TimeoutException:
    # 处理找不到元素的情况
    print("无法找到暂不开启备份按钮")

try:
    button = wait.until(EC.element_to_be_clickable((By.ID, 'com.baidu.netdisk:id/dialog_button_cancel')))
    button.click()
except TimeoutException:
    # 处理找不到元素的情况
    print("无法找到暂不开启备份按钮 2")

wait = WebDriverWait(driver, 5, 0.5)

try:
    button = wait.until(EC.element_to_be_clickable((By.ID, 'com.baidu.netdisk:id/iv_close')))
    button.click()
except TimeoutException:
    # 处理找不到元素的情况
    print("无法找到广告关闭")

try:
    button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/androidx.slidingpanelayout.widget.SlidingPaneLayout/android.widget.FrameLayout/android.widget.FrameLayout[2]/android.widget.LinearLayout/android.widget.LinearLayout[5]')))
    button.click()
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找我的按钮")

try:
    button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.view.ViewGroup/android.widget.ImageView')))
    button.click()
except TimeoutException:
    # 处理找不到元素的情况
    print("无法找到广告关闭2")

try:
    button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/androidx.slidingpanelayout.widget.SlidingPaneLayout/android.widget.FrameLayout/android.widget.FrameLayout[1]/android.widget.LinearLayout/android.widget.FrameLayout/android.view.ViewGroup/android.widget.ScrollView/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.LinearLayout[1]/android.view.ViewGroup[2]')))
    button.click()
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找待领取积分按钮")

# 优化
sleep(4)
try:                                        
    button = driver.find_element(By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.RelativeLayout/android.widget.FrameLayout/android.widget.FrameLayout/android.widget.FrameLayout/android.webkit.WebView/android.webkit.WebView/android.view.View/android.view.View[2]/android.view.View[1]')
    button.click()
except NoSuchElementException:
    # 处理找不到元素的情况
    print("无法找到元素")

sleep(2)
driver.quit()