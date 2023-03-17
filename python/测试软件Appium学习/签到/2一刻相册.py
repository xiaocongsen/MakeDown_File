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
caps["appium:appPackage"] = "com.baidu.youavideo"
caps["appium:appActivity"] = ".app.ui.SplashActivity"
caps["appium:platformVersion"] = "13"
caps["appium:noReset"] = True
# caps["appium:unicodeKeyboard"] = True
# caps["appium:resetKeyboard"] = True
caps["appium:dontStopAppOnReset"] = False
caps["appium:ensureWebviewsHavePages"] = True
caps["appium:nativeWebScreenshot"] = True
caps["appium:newCommandTimeout"] = 3600
caps["appium:connectHardwareKeyboard"] = True

driver = webdriver.Remote("http://127.0.0.1:4723/wd/hub", caps)
wait = WebDriverWait(driver, 5)
try:
    button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.TextView[1]')))
    button.click()
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找广告跳过按钮")

try:
    button = wait.until(EC.element_to_be_clickable((By.ID, 'com.baidu.youavideo:id/dialog_home_pop_close_button')))
    button.click()
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找广告关闭按钮")

try:
    button = wait.until(EC.element_to_be_clickable((By.ID, 'com.baidu.youavideo:id/img_close')))
    button.click()
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找升级关闭按钮")

try:
    button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.view.ViewGroup/android.widget.FrameLayout/android.widget.FrameLayout/android.widget.FrameLayout/android.view.ViewGroup[2]/android.widget.ImageView')))
    button.click()
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找签到按钮")

try:
    button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.view.ViewGroup/android.widget.RelativeLayout/android.widget.RelativeLayout/android.widget.FrameLayout/android.webkit.WebView/android.webkit.WebView/android.view.View/android.view.View/android.view.View[3]/android.view.View[2]/android.view.View/android.widget.TextView[8]')))
    button.click()
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找关闭签到成功按钮")

try:
    button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.view.ViewGroup/android.widget.RelativeLayout/android.widget.RelativeLayout/android.widget.FrameLayout/android.webkit.WebView/android.webkit.WebView/android.view.View/android.view.View[4]/android.view.View[2]')))
    button.click()
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找双签到按钮")

# TODO 太慢了，需要优化
try:
    button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.FrameLayout[2]/android.widget.RelativeLayout/android.widget.RelativeLayout/android.widget.FrameLayout/android.webkit.WebView/android.webkit.WebView/android.view.View/android.view.View[4]/android.view.View/android.view.View/android.widget.TextView[2]')))
    button.click()
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找领取按钮")

sleep(1)
driver.quit()