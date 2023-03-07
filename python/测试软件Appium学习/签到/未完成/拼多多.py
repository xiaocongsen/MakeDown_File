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
from selenium.common.exceptions import StaleElementReferenceException
from appium.webdriver.common.touch_action import TouchAction


caps = {}
caps["platformName"] = "Android"
caps["appium:deviceName"] = "M2012K11AC"
caps["appium:appPackage"] = "com.xunmeng.pinduoduo"
caps["appium:appActivity"] = ".ui.activity.MainFrameActivity"
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
try:                                        
    button = driver.find_element(By.ID, 'xxx')
    button.click()
except NoSuchElementException:
    # 处理找不到元素的情况
    print("无法找到元素")

wait = WebDriverWait(driver, 5)
try:
    button = wait.until(EC.presence_of_element_located((By.XPATH, '//android.widget.FrameLayout[@content-desc="快捷入口列表"]/android.support.v7.widget.RecyclerView/android.widget.RelativeLayout[8]')))
    button.click()
except TimeoutException:
    print("超时没找签到按钮")


sleep(1)
driver.quit()



except StaleElementReferenceException:           # 处理查找到的元素与之前存储的元素不再是同一个对象 重新获取元素
    button = wait.until(EC.presence_of_element_located((By.XPATH, 'xxx')))
    button.click()


try:
    element = wait.until(EC.presence_of_element_located((By.XPATH, 'xx')))
    location = element.location
    size = element.size
    x = location['x'] + size['width'] / 2
    y = location['y'] + size['height'] / 2
    action = TouchAction(driver)
    action.tap(x=x, y=y).perform()
except TimeoutException:
    print("超时没找我的按钮")
