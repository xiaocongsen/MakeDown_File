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
caps["appium:appPackage"] = "com.czb.chezhubang"
caps["appium:appActivity"] = ".activity.MainActivity"
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
wait = WebDriverWait(driver, 6, 0.5)
try:
    button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.FrameLayout/android.widget.FrameLayout/android.widget.FrameLayout/android.view.ViewGroup[2]')))
    button.click()
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找广告按钮")
# TODO 太慢了，需要优化

sleep(1)
# try:                                        
#     button = driver.find_element(By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.RelativeLayout/androidx.viewpager.widget.ViewPager/androidx.recyclerview.widget.RecyclerView/android.widget.FrameLayout/android.widget.FrameLayout/android.widget.FrameLayout/android.view.ViewGroup/android.view.ViewGroup/android.view.ViewGroup[1]/android.widget.ScrollView/android.view.ViewGroup/android.view.ViewGroup[1]/android.view.ViewGroup/android.view.ViewGroup[3]/android.widget.HorizontalScrollView/android.view.ViewGroup/android.view.ViewGroup[2]')
#     button.click()
# except NoSuchElementException:
#     # 处理找不到元素的情况
#     print("超时没找签到按钮")
actions = ActionChains(driver)
actions.w3c_actions = ActionBuilder(driver, mouse=PointerInput(interaction.POINTER_TOUCH, "touch"))
actions.w3c_actions.pointer_action.move_to_location(345, 892)       # 签到按钮坐标
actions.w3c_actions.pointer_action.pointer_down()               # 点击
actions.w3c_actions.pointer_action.pause(0.2)                   # 等待0.1秒
actions.w3c_actions.pointer_action.release()                    # 释放
actions.perform()
sleep(2)
driver.quit()