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
caps["appium:appPackage"] = "com.jd.jrapp"
caps["appium:appActivity"] = ".bm.mainbox.main.MainActivity"
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

wait = WebDriverWait(driver, 8)
try:
    element = wait.until(EC.presence_of_element_located((By.ID, 'com.jd.jrapp:id/home_header_grid_title')))
    try:
        elements = driver.find_elements(By.ID ,'com.jd.jrapp:id/home_header_grid_title')
        for i in elements:
            if '签到' == i.text:
                location = i.location
                size = i.size
                x = location['x'] + size['width'] / 2
                y = location['y'] + size['height'] / 2
                action = TouchAction(driver)
                action.tap(x=x, y=y).perform()
                break
            continue
    except NoSuchElementException:
        # 处理找不到元素的情况
        print("超时没找签到按钮1")
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找签到按钮2")

try:
    button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/com.jd.jrapp.library.framework.base.slide.SlideFrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.FrameLayout/android.widget.FrameLayout/com.tencent.tbs.core.webkit.WebView/android.webkit.WebView/android.view.View[7]/android.view.View/android.view.View[1]')))
    button.click()          #点击关闭金贴过期提示
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找金贴过期关闭按钮")   #TODO 可能不是放在这里

try:
    button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/com.jd.jrapp.library.framework.base.slide.SlideFrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.FrameLayout/android.widget.FrameLayout/com.tencent.tbs.core.webkit.WebView/android.webkit.WebView/android.view.View[1]/android.view.View[2]/android.view.View[8]')))
    button.click()          #点击签到领金贴
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找签到领金贴按钮")
sleep(2)
try:
    button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/com.jd.jrapp.library.framework.base.slide.SlideFrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.FrameLayout/android.widget.FrameLayout/com.tencent.tbs.core.webkit.WebView/android.webkit.WebView/android.view.View[1]/android.view.View/android.view.View[1]')))
    button.click()          #点击关闭立即查看
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找关闭立即查看按钮")

sleep(1)
try:
    button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/com.jd.jrapp.library.framework.base.slide.SlideFrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.FrameLayout/android.widget.FrameLayout/com.tencent.tbs.core.webkit.WebView/android.webkit.WebView/android.view.View[6]')))
    button.click()          #点击双签领金豆
except StaleElementReferenceException:
    button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/com.jd.jrapp.library.framework.base.slide.SlideFrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.FrameLayout/android.widget.FrameLayout/com.tencent.tbs.core.webkit.WebView/android.webkit.WebView/android.view.View[6]')))
    button.click()          #点击双签领金豆
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找双签领金豆按钮")
try:
    button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/com.jd.jrapp.library.framework.base.slide.SlideFrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.FrameLayout/android.widget.FrameLayout/com.tencent.tbs.core.webkit.WebView/android.webkit.WebView/android.view.View/android.view.View[5]/android.view.View[2]/android.view.View/android.view.View[2]')))
    button.click()          #点击立即领取
except TimeoutException:
    print("超时没找弹窗的立即领取按钮")
    try:
        button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/com.jd.jrapp.library.framework.base.slide.SlideFrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.FrameLayout/android.widget.FrameLayout/com.tencent.tbs.core.webkit.WebView/android.webkit.WebView/android.view.View/android.view.View[2]/android.view.View/android.view.View[3]')))
        button.click()          #点击立即领取
    except TimeoutException:
        print("超时没找点击领奖按钮")

try:
    button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/com.jd.jrapp.library.framework.base.slide.SlideFrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.FrameLayout/android.widget.FrameLayout/com.tencent.tbs.core.webkit.WebView/android.webkit.WebView/android.view.View/android.view.View[5]/android.view.View[1]')))
    button.click()          #点击关闭已领取按钮
except StaleElementReferenceException:
    button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/com.jd.jrapp.library.framework.base.slide.SlideFrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.FrameLayout/android.widget.FrameLayout/com.tencent.tbs.core.webkit.WebView/android.webkit.WebView/android.view.View/android.view.View[5]/android.view.View[1]')))
    button.click()          #点击关闭已领取按钮
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找已领取按钮")

# try:
#     button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/com.jd.jrapp.library.framework.base.slide.SlideFrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.LinearLayout/android.widget.RelativeLayout/android.widget.RelativeLayout/android.widget.LinearLayout[1]/android.widget.Button')))
#     button.click()          #点击返回按钮
# except StaleElementReferenceException:
#     button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/com.jd.jrapp.library.framework.base.slide.SlideFrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.LinearLayout/android.widget.RelativeLayout/android.widget.RelativeLayout/android.widget.LinearLayout[1]/android.widget.Button')))
#     button.click()          #点击返回按钮
# except TimeoutException:
#     # 处理找不到元素的情况
#     print("超时没找返回按钮1")

# try:
#     button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/com.jd.jrapp.library.framework.base.slide.SlideFrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.LinearLayout/android.widget.RelativeLayout/android.widget.RelativeLayout/android.widget.LinearLayout[1]/android.widget.Button')))
#     button.click()          #点击返回按钮
# except StaleElementReferenceException:
#     button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/com.jd.jrapp.library.framework.base.slide.SlideFrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.LinearLayout/android.widget.RelativeLayout/android.widget.RelativeLayout/android.widget.LinearLayout[1]/android.widget.Button')))
#     button.click()          #点击返回按钮
# except TimeoutException:
#     # 处理找不到元素的情况
#     print("超时没找返回按钮2")

# try:
#     button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/com.jd.jrapp.library.framework.base.slide.SlideFrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.LinearLayout/android.widget.RelativeLayout/android.widget.RelativeLayout/android.widget.LinearLayout[1]/android.widget.Button')))
#     button.click()          #点击返回按钮
# except StaleElementReferenceException:
#     button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/com.jd.jrapp.library.framework.base.slide.SlideFrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.widget.RelativeLayout/android.widget.LinearLayout/android.widget.RelativeLayout/android.widget.RelativeLayout/android.widget.LinearLayout[1]/android.widget.Button')))
#     button.click()          #点击返回按钮
# except TimeoutException:
#     # 处理找不到元素的情况
#     print("超时没找返回按钮3")
driver.back()
sleep(1)
driver.back()
sleep(1)
driver.back()
sleep(1)
try:
    button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.view.ViewGroup/android.view.ViewGroup/android.widget.RelativeLayout[5]')))
    button.click()          #点击我的
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找我的按钮")

try:
    button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/android.view.ViewGroup/android.widget.FrameLayout/android.view.ViewGroup/android.view.ViewGroup[1]/androidx.recyclerview.widget.RecyclerView/android.widget.RelativeLayout[1]/android.view.ViewGroup/android.view.ViewGroup/android.view.ViewGroup/android.view.ViewGroup[2]/android.view.ViewGroup[1]')))
    button.click()          #点击积分
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找积分按钮")
    
try:
    button = wait.until(EC.element_to_be_clickable((By.XPATH, '/hierarchy/android.widget.FrameLayout/android.widget.LinearLayout/android.widget.FrameLayout/com.jd.jrapp.library.framework.base.slide.SlideFrameLayout/android.view.ViewGroup/androidx.viewpager.widget.ViewPager/android.widget.FrameLayout/android.view.ViewGroup/android.view.ViewGroup/android.view.ViewGroup[2]/android.view.ViewGroup[2]/androidx.recyclerview.widget.RecyclerView/android.widget.RelativeLayout[2]/android.view.ViewGroup/android.view.ViewGroup/android.view.ViewGroup/android.view.ViewGroup[2]/android.view.ViewGroup/android.view.ViewGroup/android.view.ViewGroup/android.view.ViewGroup/android.view.ViewGroup')))
    button.click()          #点击一键领取
except TimeoutException:
    # 处理找不到元素的情况
    print("超时没找一键领取按钮")
    

sleep(2)

driver.quit()