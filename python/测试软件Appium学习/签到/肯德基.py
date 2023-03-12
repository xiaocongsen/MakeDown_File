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
from appium.webdriver.common.touch_action import TouchAction

caps = {}
caps["platformName"] = "Android"
caps["appium:deviceName"] = "M2012K11AC"
caps["appium:appPackage"] = "com.yek.android.kfc.activitys"
caps["appium:appActivity"] = "com.yum.brandkfc.SplashAct"
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

wait = WebDriverWait(driver, 15)
try:
    button = wait.until(EC.presence_of_element_located((By.ID, 'com.yek.android.kfc.activitys:id/homev2_view_me_iv_12')))
    button.click()
except TimeoutException:
    print("超时没找主页签到按钮")

sleep(3)
try:                                                          
    elements = driver.find_elements(By.CLASS_NAME ,'android.widget.TextView')
    for i in elements:
        if '签到' == i.text:
            location = i.location
            size = i.size
            x = location['x'] + size['width'] / 2
            y = location['y'] + size['height'] / 2
            action = TouchAction(driver)
            action.tap(x=x, y=y).perform()
        continue
except NoSuchElementException:
    # 处理找不到元素的情况
    print("超时没找签到按钮")



# wait = WebDriverWait(driver, 5)
# try:
    
#     element = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.RelativeLayout/android.widget.FrameLayout/android.view.ViewGroup/android.view.ViewGroup/android.view.ViewGroup/android.view.ViewGroup[1]/android.widget.ScrollView/android.view.ViewGroup/android.view.ViewGroup[2]/android.view.ViewGroup[3]/android.view.ViewGroup[2]')))
#     location = element.location
#     size = element.size
#     x = location['x'] + size['width'] / 2
#     y = location['y'] + size['height'] / 2
#     action = TouchAction(driver)
#     action.tap(x=x, y=y).perform() 
# except TimeoutException:
#     print("超时没找签到按钮")

sleep(2)
driver.quit()