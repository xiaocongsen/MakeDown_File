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
from selenium.common.exceptions import NoSuchElementException
from selenium.common.exceptions import StaleElementReferenceException
from appium.webdriver.common.touch_action import TouchAction

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
    button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.LinearLayout/android.widget.TabHost/android.widget.TabWidget/android.widget.FrameLayout[5]')))
    button.click()      #点击我的按钮
except StaleElementReferenceException:
    button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.LinearLayout/android.widget.TabHost/android.widget.TabWidget/android.widget.FrameLayout[5]')))
    button.click()      #点击我的按钮  
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找我的按钮")

try:
    element = wait.until(EC.element_to_be_clickable((By.ID, 'com.alipay.mobile.antui:id/right_text')))
    location = element.location
    size = element.size
    x = location['x'] + size['width'] / 2
    y = location['y'] + size['height'] / 2
    action = TouchAction(driver)
    action.tap(x=x, y=y).perform()
except StaleElementReferenceException:
    element = wait.until(EC.element_to_be_clickable((By.ID, 'com.alipay.mobile.antui:id/right_text')))
    location = element.location
    size = element.size
    x = location['x'] + size['width'] / 2
    y = location['y'] + size['height'] / 2
    action = TouchAction(driver)
    action.tap(x=x, y=y).perform()
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找设置1")

# try:
#     button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.ListView/android.widget.TextView[1]')))
#     button.click()      #点击切换账号
# except TimeoutException:
#     # 处理找不到元素的情况
#     print("超时没找切换账号1")
sleep(0.5)
actions = ActionChains(driver)
actions.w3c_actions = ActionBuilder(driver, mouse=PointerInput(interaction.POINTER_TOUCH, "touch"))
actions.w3c_actions.pointer_action.move_to_location(544, 1554)
actions.w3c_actions.pointer_action.pointer_down()       #点击切换账号
actions.w3c_actions.pointer_action.pause(0.2)
actions.w3c_actions.pointer_action.release()
actions.perform()
sleep(0.5)
try:                                                          
    elements = driver.find_elements(By.ID,'com.alipay.mobile.securitybiz:id/layout_container')
    count = len(elements)
except NoSuchElementException:
    # 处理找不到元素的情况
    print("没有找到账号列表")
print("账号列表数量:",count)
index = count - 1
while count > 0:
    element = elements[index]
    count = count - 1
    location = element.location
    size = element.size
    x = location['x'] + size['width'] / 2
    y = location['y'] + size['height'] / 2
    action = TouchAction(driver)
    action.tap(x=x, y=y).perform()

    try:
        button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.LinearLayout/android.widget.TabHost/android.widget.TabWidget/android.widget.FrameLayout[4]')))
        button.click()      #点击消息
    except TimeoutException:
        # 处理找不到元素的情况
        print("超时没找消息2")

    sleep(0.5)
    try:                                                          
        elements = driver.find_elements(By.ID,'com.alipay.mobile.socialwidget:id/item_name')
        for i in elements:
            if '新电途' in i.text:
                element = i
                break
            else:
                element = None
            continue
    except NoSuchElementException:
        # 处理找不到元素的情况
        print("超时没找聊天记录1")

    if element is not None:
        location = i.location
        size = i.size
        x = location['x'] + size['width'] / 2
        y = location['y'] + size['height'] / 2
        action = TouchAction(driver)
        action.tap(x=x, y=y).perform()
        try:
            button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.RelativeLayout[2]/android.widget.RelativeLayout/android.widget.LinearLayout/android.support.v4.view.ViewPager/android.widget.LinearLayout/android.widget.LinearLayout')))
            button.click()      # 点击聊天中的签到公告
        except TimeoutException:
            # 处理找不到元素的情况
            print("超时没找新电涂群签到公告2")

        sleep(5)

        actions = ActionChains(driver)
        actions.w3c_actions = ActionBuilder(driver, mouse=PointerInput(interaction.POINTER_TOUCH, "touch"))
        actions.w3c_actions.pointer_action.move_to_location(544, 1293)          #点击签到
        actions.w3c_actions.pointer_action.pointer_down()
        actions.w3c_actions.pointer_action.pause(0.2)
        actions.w3c_actions.pointer_action.release()
        actions.perform()
        
        sleep(1)
        driver.back()                                                           #返回到聊天界面
        
        try:
            button = wait.until(EC.element_to_be_clickable((By.ID, 'com.alipay.mobile.antui:id/back_button')))
            button.click()      #点击返回按钮
        except TimeoutException:
            # 处理找不到元素的情况
            driver.back()
            print("超时没找返回到聊天主页")
    else:
        print("当前账号没找到新电途群")

    try:
        button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.LinearLayout/android.widget.TabHost/android.widget.TabWidget/android.widget.FrameLayout[5]')))
        button.click()      #点击我的按钮
    except TimeoutException:
        # 处理找不到元素的情况
        print("超时没找到我的1")
        driver.back()
        try:
            button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.LinearLayout/android.widget.TabHost/android.widget.TabWidget/android.widget.FrameLayout[5]')))
            button.click()      #点击我的按钮
        except TimeoutException:
            print("超时没找到我的2")

    sleep(0.5)
    actions = ActionChains(driver)
    actions.w3c_actions = ActionBuilder(driver, mouse=PointerInput(interaction.POINTER_TOUCH, "touch"))
    actions.w3c_actions.pointer_action.move_to_location(317, 527)           #点击支付宝会员按钮
    actions.w3c_actions.pointer_action.pointer_down()
    actions.w3c_actions.pointer_action.pause(0.1)
    actions.w3c_actions.pointer_action.release()
    actions.perform()

    # try:
    #     button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.RelativeLayout[2]/android.widget.RelativeLayout/android.widget.RelativeLayout/android.widget.FrameLayout/com.uc.webview.export.WebView/com.uc.webkit.bc/android.webkit.WebView/android.view.View/android.view.View[2]/android.view.View[1]/android.view.View[2]')))
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
    sleep(3)
    try:
        button = wait.until(EC.element_to_be_clickable((By.XPATH, '//android.widget.FrameLayout[@content-desc="返回"]/android.widget.FrameLayout')))
        button.click()      #点击返回到支付宝会员
    except StaleElementReferenceException:
        button = wait.until(EC.element_to_be_clickable((By.XPATH, '//android.widget.FrameLayout[@content-desc="返回"]/android.widget.FrameLayout')))
        button.click()      #点击返回到支付宝会员
    except TimeoutException:
        # 处理找不到元素的情况
        driver.back()
        print("超时没找返回到支付宝会员1")

    print("count:", count)
    if count == 0:
        driver.quit()
        exit()
    try:
        button = wait.until(EC.element_to_be_clickable((By.XPATH, '//android.widget.FrameLayout[@content-desc="返回"]/android.widget.FrameLayout')))
        button.click()      #点击返回到我的
    except StaleElementReferenceException:
        button = wait.until(EC.element_to_be_clickable((By.XPATH, '//android.widget.FrameLayout[@content-desc="返回"]/android.widget.FrameLayout')))
        button.click()      #点击返回到我的
    except TimeoutException:
        # 处理找不到元素的情况
        driver.back()
        print("超时没找返回到我的1")
    # driver.back()                                                           #返回到支付宝会员
    # sleep(1)
    # driver.back()                                                           #返回到我的
    # sleep(1)

    try:
        element = wait.until(EC.element_to_be_clickable((By.ID, 'com.alipay.mobile.antui:id/right_text')))
        location = element.location
        size = element.size
        x = location['x'] + size['width'] / 2
        y = location['y'] + size['height'] / 2
        action = TouchAction(driver)
        action.tap(x=x, y=y).perform()
    except TimeoutException:
        # 处理找不到元素的情况
        print("超时没找设置1")

    # try:
    #     button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.ListView/android.widget.TextView[1]')))
    #     button.click()      #点击切换账号
    # except TimeoutException:
    #     # 处理找不到元素的情况
    #     print("超时没找切换账号1")

    sleep(0.5)
    actions = ActionChains(driver)
    actions.w3c_actions = ActionBuilder(driver, mouse=PointerInput(interaction.POINTER_TOUCH, "touch"))
    actions.w3c_actions.pointer_action.move_to_location(544, 1554)
    actions.w3c_actions.pointer_action.pointer_down()       #点击切换账号
    actions.w3c_actions.pointer_action.pause(0.2)
    actions.w3c_actions.pointer_action.release()
    actions.perform()
    sleep(0.5)
    elements = driver.find_elements(By.ID,'com.alipay.mobile.securitybiz:id/layout_container')
    continue

driver.quit()