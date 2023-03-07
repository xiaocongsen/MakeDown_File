#/bin/python3
# -*- coding: utf-8 -*-
# 通过appium启动手机上所有安装的能打开的应用，获取应用的包名和启动activity

import subprocess
from appium import webdriver
from time import sleep

# 启动应用
def launch_app(app_package, app_activity):
    desired_caps = {
        'platformName': 'Android',
        'platformVersion': '13',
        'deviceName': 'M2012K11AC',
        'appPackage': app_package,
        'appActivity': app_activity,
        'unicodeKeyboard': True,
        'resetKeyboard': True,
        'noReset': True
    }
    driver = webdriver.Remote('http://localhost:4723/wd/hub', desired_caps)
    sleep(2)
    driver.quit()

def get_all_packages():
    package_list = []
    packages = subprocess.check_output(['adb', 'shell', 'pm', 'list', 'packages']).splitlines()
    for package in packages:
        package_name = package.decode('utf-8').split(':')[1]
        package_list.append(package_name)
    return package_list


def get_app_info(package_name):

    app_activity = subprocess.check_output(["adb", "shell", "cmd", "package", "resolve-activity", "--brief", package_name]).decode().strip().split("/")[-1]
    return app_activity


if __name__ == '__main__':
    packages = get_all_packages()
    index = 0
    for package in packages:
        try:
            if index < 400:
                index += 1
                continue
            app_activity = get_app_info(package)
            if app_activity == "No activity found":
                continue
            print(package, app_activity)
            launch_app(package, app_activity)
        except Exception as e:
            print(e, package)
            continue