# This sample code uses the Appium python client v2
# pip install Appium-Python-Client
# Then you can paste this into a file and simply run with Python

from appium import webdriver
from appium.webdriver.common.appiumby import AppiumBy
from time import sleep
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.common.actions import interaction
from selenium.webdriver.common.actions.action_builder import ActionBuilder
from selenium.webdriver.common.actions.pointer_input import PointerInput
from selenium.webdriver.common.by import By
from selenium.webdriver.support import expected_conditions as EC
from selenium.common.exceptions import TimeoutException
caps = {}
caps["platformName"] = "Android"
caps["appium:deviceName"] = "M2012K11AC"
caps["appium:appPackage"] = "com.eg.android.AlipayGphone"
caps["appium:appActivity"] = ".AlipayLogin"
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
    button = wait.until(EC.presence_of_element_located((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.LinearLayout/android.widget.TabHost/android.widget.TabWidget/android.widget.FrameLayout[4]')))
    button.click()      #点击消息
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找消息1")

try:
    button = wait.until(EC.presence_of_element_located((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.LinearLayout/android.widget.TabHost/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.ListView/android.widget.RelativeLayout[2]')))
    button.click()      # 获取并点击新电涂的聊天
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找新电涂群聊天1")

try:
    button = wait.until(EC.presence_of_element_located((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.RelativeLayout[2]/android.widget.RelativeLayout/android.widget.LinearLayout/android.support.v4.view.ViewPager/android.widget.LinearLayout/android.widget.LinearLayout')))
    button.click()      # 点击聊天中的签到公告
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找新电涂群签到公告1")

sleep(5)

actions = ActionChains(driver)
actions.w3c_actions = ActionBuilder(driver, mouse=PointerInput(interaction.POINTER_TOUCH, "touch"))
actions.w3c_actions.pointer_action.move_to_location(544, 1293)          #点击签到
actions.w3c_actions.pointer_action.pointer_down()
actions.w3c_actions.pointer_action.pause(0.1)
actions.w3c_actions.pointer_action.release()
actions.perform()

driver.back()                                                           #返回到聊天界面
driver.back()                                                           #返回到支付宝主界面

try:
    button = wait.until(EC.presence_of_element_located((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.LinearLayout/android.widget.TabHost/android.widget.TabWidget/android.widget.FrameLayout[5]')))
    button.click()      #点击返回到聊天
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找返回到聊天1")

try:
    button = wait.until(EC.presence_of_element_located((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.LinearLayout/android.widget.TabHost/android.widget.TabWidget/android.widget.FrameLayout[5]')))
    button.click()      #点击返回到主页面
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找返回主页面1")

actions = ActionChains(driver)
actions.w3c_actions = ActionBuilder(driver, mouse=PointerInput(interaction.POINTER_TOUCH, "touch"))
actions.w3c_actions.pointer_action.move_to_location(317, 527)           #点击支付宝会员按钮
actions.w3c_actions.pointer_action.pointer_down()
actions.w3c_actions.pointer_action.pause(0.1)
actions.w3c_actions.pointer_action.release()
actions.perform()

# try:
#     button = wait.until(EC.presence_of_element_located((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.RelativeLayout[2]/android.widget.RelativeLayout/android.widget.RelativeLayout/android.widget.FrameLayout/com.uc.webview.export.WebView/com.uc.webkit.bc/android.webkit.WebView/android.view.View/android.view.View[2]/android.view.View[1]/android.view.View[2]')))
#     button.click()      #点击每日签到领积分
# except TimeoutException:
#     # 处理找不到元素的情况
#     print("超时没找每日签到1")
	
sleep(2)
actions = ActionChains(driver)
actions.w3c_actions = ActionBuilder(driver, mouse=PointerInput(interaction.POINTER_TOUCH, "touch"))
actions.w3c_actions.pointer_action.move_to_location(550, 743)           #点击每日签到领积分
actions.w3c_actions.pointer_action.pointer_down()
actions.w3c_actions.pointer_action.pause(0.1)
actions.w3c_actions.pointer_action.release()
actions.perform()

try:
    button = wait.until(EC.presence_of_element_located((By.XPATH, '//android.widget.FrameLayout[@content-desc="返回"]/android.widget.FrameLayout')))
    button.click()      #点击返回到支付宝会员
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找返回到支付宝会员1")

try:
    button = wait.until(EC.presence_of_element_located((By.XPATH, '//android.widget.FrameLayout[@content-desc="返回"]/android.widget.FrameLayout')))
    button.click()      #点击返回到我的
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找返回到我的1")
# driver.back()                                                           #返回到支付宝会员
# sleep(1)
# driver.back()                                                           #返回到我的
# sleep(1)

try:
    button = wait.until(EC.presence_of_element_located((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.LinearLayout/android.widget.TabHost/android.widget.FrameLayout/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.LinearLayout/android.widget.RelativeLayout/android.widget.RelativeLayout/android.widget.LinearLayout/android.widget.RelativeLayout[3]')))
    button.click()      #点击设置
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找设置1")

try:
    button = wait.until(EC.presence_of_element_located((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.ListView/android.widget.TextView[1]')))
    button.click()      #点击切换账号
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找切换账号1")

actions = ActionChains(driver)
actions.w3c_actions = ActionBuilder(driver, mouse=PointerInput(interaction.POINTER_TOUCH, "touch"))
actions.w3c_actions.pointer_action.move_to_location(488, 1004)                  #点击第二个账号
actions.w3c_actions.pointer_action.pointer_down()
actions.w3c_actions.pointer_action.pause(0.1)
actions.w3c_actions.pointer_action.release()
actions.perform()
try:
    button = wait.until(EC.presence_of_element_located((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.LinearLayout/android.widget.TabHost/android.widget.TabWidget/android.widget.FrameLayout[4]')))
    button.click()      #点击消息
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找消息2")

try:
    button = wait.until(EC.presence_of_element_located((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.LinearLayout/android.widget.TabHost/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.ListView/android.widget.RelativeLayout[2]')))
    button.click()      # 获取并点击新电涂的聊天
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找新电涂群聊天2")

try:
    button = wait.until(EC.presence_of_element_located((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.RelativeLayout[2]/android.widget.RelativeLayout/android.widget.LinearLayout/android.support.v4.view.ViewPager/android.widget.LinearLayout/android.widget.LinearLayout')))
    button.click()      # 点击聊天中的签到公告
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找新电涂群签到公告2")

sleep(5)

actions = ActionChains(driver)
actions.w3c_actions = ActionBuilder(driver, mouse=PointerInput(interaction.POINTER_TOUCH, "touch"))
actions.w3c_actions.pointer_action.move_to_location(544, 1293)          #点击签到
actions.w3c_actions.pointer_action.pointer_down()
actions.w3c_actions.pointer_action.pause(0.1)
actions.w3c_actions.pointer_action.release()
actions.perform()

driver.back()                                                           #返回到聊天界面
driver.back()                                                           #返回到支付宝主界面

try:
    button = wait.until(EC.presence_of_element_located((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.LinearLayout/android.widget.TabHost/android.widget.TabWidget/android.widget.FrameLayout[5]')))
    button.click()      #点击返回到聊天
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找返回到聊天2")

try:
    button = wait.until(EC.presence_of_element_located((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.LinearLayout/android.widget.TabHost/android.widget.TabWidget/android.widget.FrameLayout[5]')))
    button.click()      #点击返回到主页面
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找返回主页面2")

actions = ActionChains(driver)
actions.w3c_actions = ActionBuilder(driver, mouse=PointerInput(interaction.POINTER_TOUCH, "touch"))
actions.w3c_actions.pointer_action.move_to_location(317, 527)           #点击支付宝会员按钮
actions.w3c_actions.pointer_action.pointer_down()
actions.w3c_actions.pointer_action.pause(0.1)
actions.w3c_actions.pointer_action.release()
actions.perform()

# try:
#     button = wait.until(EC.presence_of_element_located((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.RelativeLayout[2]/android.widget.RelativeLayout/android.widget.RelativeLayout/android.widget.FrameLayout/com.uc.webview.export.WebView/com.uc.webkit.bc/android.webkit.WebView/android.view.View/android.view.View[2]/android.view.View[1]/android.view.View[2]')))
#     button.click()      #点击每日签到领积分
# except TimeoutException:
#     # 处理找不到元素的情况
#     print("超时没找每日签到2")
	
sleep(2)
actions = ActionChains(driver)
actions.w3c_actions = ActionBuilder(driver, mouse=PointerInput(interaction.POINTER_TOUCH, "touch"))
actions.w3c_actions.pointer_action.move_to_location(550, 743)           #点击每日签到领积分
actions.w3c_actions.pointer_action.pointer_down()
actions.w3c_actions.pointer_action.pause(0.1)
actions.w3c_actions.pointer_action.release()
actions.perform()

try:
    button = wait.until(EC.presence_of_element_located((By.XPATH, '//android.widget.FrameLayout[@content-desc="返回"]/android.widget.FrameLayout')))
    button.click()      #点击返回到支付宝会员
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找返回到支付宝会员2")

try:
    button = wait.until(EC.presence_of_element_located((By.XPATH, '//android.widget.FrameLayout[@content-desc="返回"]/android.widget.FrameLayout')))
    button.click()      #点击返回到我的
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找返回到我的2")
# driver.back()                                                           #返回到支付宝会员
# sleep(1)
# driver.back()                                                           #返回到我的
# sleep(1)

try:
    button = wait.until(EC.presence_of_element_located((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.LinearLayout/android.widget.TabHost/android.widget.FrameLayout/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.LinearLayout/android.widget.RelativeLayout/android.widget.RelativeLayout/android.widget.LinearLayout/android.widget.RelativeLayout[3]')))
    button.click()      #点击设置
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找设置2")

try:
    button = wait.until(EC.presence_of_element_located((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.ListView/android.widget.TextView[1]')))
    button.click()      #点击切换账号
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找切换账号2")

actions = ActionChains(driver)
actions.w3c_actions = ActionBuilder(driver, mouse=PointerInput(interaction.POINTER_TOUCH, "touch"))
actions.w3c_actions.pointer_action.move_to_location(488, 1004)                  #点击第二个账号
actions.w3c_actions.pointer_action.pointer_down()
actions.w3c_actions.pointer_action.pause(0.1)
actions.w3c_actions.pointer_action.release()
actions.perform()
sleep(3)

driver.quit()