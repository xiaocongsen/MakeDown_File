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
caps["appium:appPackage"] = "com.jingdong.app.mall"
caps["appium:appActivity"] = ".main.MainActivity"
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
wait = WebDriverWait(driver, 5, 0.5)
try: 
    button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.FrameLayout/android.widget.RelativeLayout[2]/android.widget.LinearLayout[2]/android.view.View')))
    button.click()
except TimeoutException:
    # 处理找不到元素的情况
    print("无法找到元素 ...这是啥")

wait = WebDriverWait(driver, 5, 0.5)
try:
    button = wait.until(EC.element_to_be_clickable((By.XPATH, '//androidx.recyclerview.widget.RecyclerView[@content-desc="NewAppcenter"]/android.widget.RelativeLayout[8]')))
    button.click()
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找主页领京豆按钮")

sleep(8)
print("等待8秒后点击")
actions = ActionChains(driver)
actions.w3c_actions = ActionBuilder(driver, mouse=PointerInput(interaction.POINTER_TOUCH, "touch"))
actions.w3c_actions.pointer_action.move_to_location(117, 986)
actions.w3c_actions.pointer_action.pointer_down()
actions.w3c_actions.pointer_action.pause(0.1)
actions.w3c_actions.pointer_action.release()
actions.perform()   #知道了
    
sleep(2)            
print("点击签到前")
actions = ActionChains(driver)
actions.w3c_actions = ActionBuilder(driver, mouse=PointerInput(interaction.POINTER_TOUCH, "touch"))
actions.w3c_actions.pointer_action.move_to_location(548, 460)       #签到
actions.w3c_actions.pointer_action.pointer_down()
actions.w3c_actions.pointer_action.pause(0.1)
actions.w3c_actions.pointer_action.release()
actions.perform()

# try:
#     element = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.RelativeLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.FrameLayout/com.tencent.tbs.core.webkit.WebView/android.webkit.WebView/android.view.View/android.view.View/android.view.View/android.view.View/android.view.View[1]/android.view.View/android.view.View[4]')))
#     location = element.location
#     size = element.size
#     x = location['x'] + size['width'] / 2
#     y = location['y'] + size['height'] / 2
#     action = TouchAction(driver)
#     action.tap(x=x, y=y).perform()
# except TimeoutException:
#     # 处理找不到元素的情况
#     print("超时没找知道了按钮")


# try:
#     element = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.RelativeLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.FrameLayout/com.tencent.tbs.core.webkit.WebView/android.webkit.WebView/android.view.View/android.view.View/android.view.View/android.view.View/android.view.View[1]/android.view.View/android.view.View[1]/android.view.View[1]/android.view.View/android.view.View[2]/android.view.View/android.view.View[3]/android.view.View[1]/android.view.View[1]')))
#     location = element.location
#     size = element.size
#     x = location['x'] + size['width'] / 2
#     y = location['y'] + size['height'] / 2
#     action = TouchAction(driver)
#     action.tap(x=x, y=y).perform()
# except TimeoutException:
#     # 处理找不到元素的情况
#     print("超时没找领京豆按钮")


# try:
#     button = wait.until(EC.element_to_be_clickable((By.XPATH, '//android.widget.ImageView[@content-desc="返回"]')))
#     button.click()
# except TimeoutException:
#     # 处理找不到元素的情况
#     print("超时没找返回按钮")

sleep(3)

driver.quit()