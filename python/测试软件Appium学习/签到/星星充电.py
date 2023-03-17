# This sample code uses the Appium python client v2
# pip install Appium-Python-Client
# Then you can paste this into a file and simply run with Python
import re
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
from appium.webdriver.common.touch_action import TouchAction

caps = {}
caps["platformName"] = "Android"
caps["appium:deviceName"] = "M2012K11AC"
caps["appium:appPackage"] = "com.wanbangauto.chargepile"
caps["appium:appActivity"] = ".DefaultAliasActivity"
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
wait = WebDriverWait(driver, 6)
# TODO 问题处理

try:
    button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.FrameLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.LinearLayout/android.widget.ImageView')))
    button.click()
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找广告关闭按钮")

try:
    button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.FrameLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.FrameLayout[1]/android.widget.RelativeLayout/android.widget.RelativeLayout/android.widget.FrameLayout/android.widget.LinearLayout/androidx.recyclerview.widget.RecyclerView/android.widget.LinearLayout[4]')))
    button.click()          #点击优惠活动
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找优惠活动按钮")
sleep(0.5)
try:
    button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.RelativeLayout[2]/android.webkit.WebView/android.webkit.WebView/android.view.View/android.view.View/android.view.View/android.view.View/android.view.View/android.view.View[1]/android.view.View/android.view.View/android.view.View/android.widget.TextView')))
    button.click()          #点击签到
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找签到按钮")

# 可能需要点击确定
try:
    button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.RelativeLayout[1]/android.widget.LinearLayout/android.widget.RelativeLayout/android.widget.ImageButton')))
    button.click()          #点击返回
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找返回按钮")

try:
    button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.LinearLayout/android.widget.LinearLayout/android.widget.FrameLayout[5]')))
    button.click()          #点击全部
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找全部按钮")	

try:
    button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.FrameLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.view.ViewGroup/android.widget.ScrollView/android.widget.RelativeLayout/android.widget.LinearLayout/android.widget.LinearLayout[2]/android.widget.FrameLayout[4]/android.widget.LinearLayout/androidx.recyclerview.widget.RecyclerView/android.widget.LinearLayout[1]')))
    button.click()          #点击摇一摇按钮
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找摇一摇按钮")	
	
try:
    button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.RelativeLayout[2]/android.webkit.WebView/android.webkit.WebView/android.view.View/android.view.View/android.view.View/android.widget.TextView[2]')))
    button.click()          #点击好友邀请
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找好友邀请按钮")	

sleep(1)
action = TouchAction(driver)
action.tap(x=530, y=620).perform()

element = driver.find_element(By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.RelativeLayout[2]/android.webkit.WebView/android.webkit.WebView/android.view.View/android.view.View/android.view.View/android.widget.TextView[1]')
match = re.search(r"\d+(?=次)", element.text)
if match:
    count = int(match.group())
    while count != 0:
        count = count - 1
        try:
            button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.RelativeLayout[2]/android.webkit.WebView/android.webkit.WebView/android.view.View/android.view.View/android.view.View/android.widget.TextView[3]')))
            button.click()          #点击手动摇一摇
        except TimeoutException:
            # 处理找不到元素的情况
            print("超时没找手动摇一摇按钮")	

        sleep(1)
        action = TouchAction(driver)
        action.tap(x=60, y=620).perform()
else:
    print("超时没找摇一摇次数按钮")	

sleep(2)
driver.quit()