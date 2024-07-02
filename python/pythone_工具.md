# 特别的方法

- win下python打包成exe
    pyinstaller.exe --onefile .\xxx.py

# 分解文件路径和文件名
``` python
import os

file_path = "G:/Downloads/2.xlsx"
# 使用os.path.dirname()获取目录路径
directory_path = os.path.dirname(file_path) #directory_path = G:/Downloads
# 使用os.path.basename()获取文件名
file_name = os.path.basename(file_path) # file_name = 2.xlsx

```

# win下
``` python

import tkinter as tk
from tkinter import filedialog
from tkinter import simpledialog
from tkinter import messagebox

# 创建一个Tkinter根窗口（必须创建一个根窗口才能使用filedialog）
root = tk.Tk()
root.withdraw()  # 隐藏根窗口

# 定义文件类型过滤器，只显示扩展名为txt和csv的文件
filetypes = [("Excel files", "*.xlsx")]
# 弹出文件选择框
file_path = filedialog.askopenfilename(title="请选择需要处理的数据文件",filetypes=filetypes)
# 用户选择文件后，会返回文件路径，如果用户取消选择则返回空字符串
if file_path:
    print("选择的文件路径:", file_path)
else:
    print("用户取消了选择文件")
    exit()

# 弹出输入框
user_input = simpledialog.askstring("输入", "请输入内容:")

if user_input is not None:
    print("用户输入的内容:", user_input)
else:
    print("用户取消了输入")
    exit()

# 弹出错误框
messagebox.showerror("错误", "保存文件错误，文件已经被打开或者没有权限")

# 弹出另存为框
file_path = filedialog.asksaveasfilename(defaultextension=".xlsx", filetypes=[("Excel files", "*.xlsx")])
if file_path:
    print("用户选择的文件路径和文件名:", file_path)
else:
    print("用户取消了输入")

# 弹出确认框
response = messagebox.askyesno("确认", "是否继续执行操作？")
# 判断用户选择的结果
if response:
    print("用户选择继续执行操作")
else:
    print("用户选择取消执行操作")

# 弹出提示框
messagebox.showinfo("完成", "操作已完成！")

```
