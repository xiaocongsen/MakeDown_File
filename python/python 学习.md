# 模块位置
    python自带模块 /usr/lib/python2.7/urllib2.py
    pip install安装的位置 /usr/local/lib/python2.7/sit-packages
# 特殊语法
## with  
```python
    with open("/tmp/foo.txt") as file:
        data = file.read()
```
    with 先执行open("/tmp/foo.txt")的__enter__()函数返回值赋给 as后面的变量 执行下面的代码块 结束后调用__exit__()方法

## eval()
    eval（）将字符串string对象转化为有效的表达式参与求值运算返回计算结果

    语法上：调用的是：eval（expression，globals=None, locals=None）返回的是计算结果
    expression:是一个参与计算的python表达式
    globals:是可选的参数，如果设置属性不为None的话，就必须是dictionary对象了
    locals:也是一个可选的对象，如果设置属性不为None的话，可以是任何map对象了
    python是用命名空间来记录变量的轨迹的，命名空间是一个dictionary，键是变量名，值是变量值。
    当一行代码要使用变量 x 的值时，Python 会到所有可用的名字空间去查找变量，按照如下顺序:
    1）局部名字空间 - 特指当前函数或类的方法。如果函数定义了一个局部变量 x, 或一个参数 x，Python 将使用它，然后停止搜索。
    2）全局名字空间 - 特指当前的模块。如果模块定义了一个名为 x 的变量，函数或类，Python 将使用它然后停止搜索。
    3）内置名字空间 - 对每个模块都是全局的。作为最后的尝试，Python 将假设 x 是内置函数或变量。
    python的全局名字空间存储在一个叫globals()的dict对象中；局部名字空间存储在一个叫locals()的dict对象中。我们可以用print (locals())来查看该函数体内的所有变量名和变量值。
## dict 字典

## list 列表
    列表和字符串的转换
```python
    #列表转字符串
    lis = ["123","456","789"]
    print(" ".join(lis))    #123 456 789
    print("".join(lis))     #123456789
    #字符串转列表
    Str = "123 456 789"
    Str.split(" ")  #["123","456","789"]
```
## set 集合

## tuple 元组

## map

## string 字符串
    .lstrip()   去掉左边空格
    .rstrip()   去掉右边空格
    .strip()    去掉左右空格

## ssl
    import ssl
    context = ssl._create_unverified_context() #表示忽略请求中的SSL证书认证
    response = urllib2.urlopen(request, context = context) # 添加context参数，表示忽略证书认证

## 字符转码
    在html的head中查看编码格式
    html_gbk.decode("gbk") 返回unicode编码格式
    html_utf8 = html_gbk.decode("gbk").encode("utf-8") #gbk转utf-8

## 输出
``` python
    print("python \n \t \t python")     #带转义字符的字符串 
    print(r"python \n \t \t python")    #纯粹的字符串
```

## 函数
``` python
#判断item变量是不是TencentInfo这个类的对象
    if isinstance(item,TencentInfo):
        print(u"是这个类的")
    else:
        print(u"不是这个类的")
```



# python正则表达式库
``` python
    import re
    #创建正则引擎对象
    pattern = re.compile(r"\d+")
    pattern.findall()
```
## Pattern 对象的一些常用方法主要有：
    match 方法：从起始位置开始查找，一次匹配
    search 方法：从任何位置开始查找，一次匹配
    findall 方法：全部匹配，返回列表
    finditer 方法：全部匹配，返回迭代器
    split 方法：分割字符串，返回列表
    sub 方法：替换

``` python
    import re
    #创建正则引擎对象
    pattern = re.compile(r"\d+")
    m = pattern.match("abcd1234bcdgh56543")
    print(m)        #None
    m = pattern.match("abcd1234bcdgh56543",4)
    print(m.group())        #1234
    m = pattern.search("abcd1234bcdgh56543")
    print(m.group())        #1234
    m = pattern.findall("abcd1234bcdgh56543")
    print(m)        #['1234','56543']
    pattern = re.compile(r"[\s\.,;]+")
    s = "a ,; bb,    . ; cd   ; .d;"
    p.split(s)  #['a', 'bb', 'cd', 'd']
    p = re.compile(r"(\w+)\s(\w+)")
    s = "hell e32, g43t t432, 435hgr"
    p.sub("hello word",s)   #hello word,hello word, 435hgr
    p.sup(r"\1 \2 \1",s)    #hell e32 hell, g43t t432 g43t,435hgr

    #中文处理
    s = u"hello world, 你好 世界"  
    re.compile(ur"[\u4e00-\u9fa5]")
    p.findall(s)

```


# Requests库
    Requests 是以 PEP 20 的箴言为中心开发的:
    Beautiful is better than ugly.(美丽优于丑陋)
    Explicit is better than implicit.(直白优于含蓄)
    Simple is better than complex.(简单优于复杂)
    Complex is better than complicated.(复杂优于繁琐)
    Readability counts.(可读性很重要)

## 安装
    pip install requests 或者 easy_install requests 或者  Python installation guide

## GET请求
``` python
import requests
#response = requests.get("http://www.baidu.com/")
#也可以这么写
#response = requests.request("get", "http://www.baidu.com/")
kw = {'wd':'长城'}
headers = {"User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/54.0.2840.99 Safari/537.36"}
# params 接收一个字典或者字符串的查询参数，字典类型自动转换为url编码，不需要urlencode()
response = requests.get("http://www.baidu.com/s?", params = kw, headers = headers)
# 查看响应内容，response.text 返回的是Unicode格式的数据
print response.text
# 查看响应内容，response.content返回的字节流数据
print respones.content
# 查看完整url地址
print response.url
# 查看响应头部字符编码
print response.encoding
# 查看响应码
print response.status_code
```

## POST请求
``` python
import requests
# response = requests.post("http://www.baidu.com/", data = data)
formdata = {
    "type":"AUTO",
    "i":"i love python",
    "doctype":"json",
    "xmlVersion":"1.8",
    "keyfrom":"fanyi.web",
    "ue":"UTF-8",
    "action":"FY_BY_ENTER",
    "typoResult":"true"
}
url = "http://fanyi.youdao.com/translate?smartresult=dict&smartresult=rule&smartresult=ugc&sessionFrom=null"
headers={ "User-Agent": "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.103 Safari/537.36"}
response = requests.post(url, data = formdata, headers = headers)
print response.text
# 如果是json文件可以直接显示
print response.json()
```

## 代理

 

# JSON
## dump dumps
    将字符串转换成json格式 默认使用ascii编码
    ensure_assic = False 来禁用ascii编码
    dump和磁盘做交互 
    dumps和内存做交互

## load loads
    将json格式转换成字符串,如果传入参数不是utf-8,需要指定encoding encoding="GBK"
    
    load和磁盘做交互
    loads和内存做交互



# Scrapy框架
## 安装
    sudo apt install python-dev python-pip libxml2-dev libxslt1-dev zlib1g-dev libffi-dev libssl-dev
    sudo pip install scrapy
## 命令
    bench           运行快速基准测试
    check           检查爬虫合同
    crawl           跑爬虫
    edit            编辑爬虫
    fetch           使用Scrapy下载程序获取URL的响应
    genspider       使用预定义的模板生成新的爬虫
    list            列出可用的爬虫
    parse           解析URL（使用其爬虫）并打印结果
    runspider       运行一个独立的爬虫（不创建项目）
    settings        获取设置值
    shell           交互式控制台中测试运行
    startproject    创建新项目
    version         打印Scrapy版本
    view            在浏览器中打开URL，如Scrapy所示

    列如:
    scrapy bench "http:www.baidu.com"           测试新能
    scrapy fetch "http:www.baidu.com"           测试下载一个页面数据
    scrapy shell "http:www.baidu.com"           在py终端中执行测试
    scrapy startproject XXX                     创建Scrapy新项目项目名XXX
    scrapy genspider xxx "xxxx.com"             创建xxx爬虫 一个项目下可以存在多个爬虫
    scrapy genspider -t crawl xxx "xxxx.com"    创建CrawlSpider模板的爬虫 继承scrapy.CrawlSpider(全栈爬虫)
    scrapy crawl xxx                            执行xxx爬虫
    scrapy list                                 查看当前项目下所有爬虫
    scrapy crawl xxx -o name.json               输出的结果按指定的格式文件保存(支持json csv xml jsonlines jl pickle marshal) 返回结果不是item格式不能达到管道

## 函数
    response.body   查看响应的文本
    response.text   查看响应的文本
    response.headers    查看请求报头
    request     获取请求信息
    response    响应的信息    
    response.xpath("//title/test() | //a/test()").extract()[0]  通过自带的xpath提取响应得到data数据取出来
    response.xpath("//a[@class='noactive' and @id='next']").extract()[0]  
        xpath()     传入xpath表达式,返回该表达式所对应的所有节点的selector list列表
        extract()   序列化该节点为Unicode字符串并返回list
    response.css()              通过自带的css来提取数据
    response.re()   根据传入的正则表达式对数据进行提取,返回Unicode字符串list列表

## 目录结构
    scrapy.cfg      scrapy项目远程部署和监控的配置
    spider/         存储爬虫代码的目录
    items.py        定义存储的字段,当字典用
    middlewares.py  中间件的处理函数编写目录
    pipelines.py    管道文件,处理items.py中定义的数据的文件,存储等
    settings.py     项目的配置文件 

## 配置介绍
``` python
# -*- coding: utf-8 -*-

# Scrapy settings for Test project  Stest项目的Scrapy设置
#
# For simplicity, this file contains only settings considered important or
# commonly used. You can find more settings consulting the documentation: 
#为简单起见，此文件仅包含被视为重要或常用的设置。 您可以在咨询文档时找到更多设置：
#
#     https://doc.scrapy.org/en/latest/topics/settings.html
#     https://doc.scrapy.org/en/latest/topics/downloader-middleware.html
#     https://doc.scrapy.org/en/latest/topics/spider-middleware.html
#BOT_NAME项目名称
BOT_NAME = 'Test'
#SPIDER_MODULES爬虫的目录
SPIDER_MODULES = ['Test.spiders']
NEWSPIDER_MODULE = 'Test.spiders'

# Crawl responsibly by identifying yourself (and your website) on the user-agent    
#通过在用户代理上识别您自己（和您的网站）来负责任地抓取
USER_AGENT = "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.103 Safari/537.36"

# Obey robots.txt rules 服从robots.txt规则
ROBOTSTXT_OBEY = False

# Configure maximum concurrent requests performed by Scrapy (default: 16)
#配置Scrapy执行的最大并发请求数（默认值：16）
#CONCURRENT_REQUESTS = 32

# Configure a delay for requests for the same website (default: 0)
# 为同一网站的请求配置延迟（默认值：0）
# See https://doc.scrapy.org/en/latest/topics/settings.html#download-delay
# See also autothrottle settings and docs
DOWNLOAD_DELAY = 3
# The download delay setting will honor only one of:    下载延迟设置仅包括以下之一：
#CONCURRENT_REQUESTS_PER_DOMAIN = 16
#CONCURRENT_REQUESTS_PER_IP = 16

# Disable cookies (enabled by default)  禁用cookie（默认启用）
#COOKIES_ENABLED = False

# Disable Telnet Console (enabled by default)   禁用Telnet控制台（默认启用）
#TELNETCONSOLE_ENABLED = False

# Override the default request headers: 覆盖默认请求标头：
DEFAULT_REQUEST_HEADERS = {
  'Accept': 'text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8',
  'Accept-Language': 'en',
}

# Enable or disable spider middlewares 启用或禁用爬虫中间件
# See https://doc.scrapy.org/en/latest/topics/spider-middleware.html
# 数值是代表优先级 值越小 优先级越高 0～1000之间
#SPIDER_MIDDLEWARES = {
#    'Test.middlewares.TestSpiderMiddleware1': 100,
#    'Test.middlewares.TestSpiderMiddleware2': 543,
#}

# Enable or disable downloader middlewares  启用下载中间件
# See https://doc.scrapy.org/en/latest/topics/downloader-middleware.html
# DOWNLOADER_MIDDLEWARES = {
#    'Test.middlewares.TestDownloaderMiddleware': 543,
# }

# Enable or disable extensions  启用或禁用扩展程序
# See https://doc.scrapy.org/en/latest/topics/extensions.html
#EXTENSIONS = {
#    'scrapy.extensions.telnet.TelnetConsole': None,
#}

# Configure item pipelines 配置项目管道组件 可以多个 优先级一样
# See https://doc.scrapy.org/en/latest/topics/item-pipeline.html
#ITEM_PIPELINES = {
#    'Test.pipelines.TestPipeline': 100,
#    'Test.pipelines.TestPipeline': 200,
#}

# Enable and configure the AutoThrottle extension (disabled by default)
#启用并配置AutoThrottle扩展（默认情况下禁用）
# See https://doc.scrapy.org/en/latest/topics/autothrottle.html
#AUTOTHROTTLE_ENABLED = True
# The initial download delay    初始下载延迟
#AUTOTHROTTLE_START_DELAY = 5
# The maximum download delay to be set in case of high latencies    在高延迟的情况下要设置的最大下载延迟
#AUTOTHROTTLE_MAX_DELAY = 60
# The average number of requests Scrapy should be sending in parallel to    Scrapy应该并行发送的平均请求数
# each remote server    每个远程服务器
#AUTOTHROTTLE_TARGET_CONCURRENCY = 1.0
# Enable showing throttling stats for every response received:  启用显示收到的每个响应的限制统计信息：
#AUTOTHROTTLE_DEBUG = False

# Enable and configure HTTP caching (disabled by default)   启用并配置HTTP缓存（默认情况下禁用）
# See https://doc.scrapy.org/en/latest/topics/downloader-middleware.html#httpcache-middleware-settings
#HTTPCACHE_ENABLED = True
#HTTPCACHE_EXPIRATION_SECS = 0
#HTTPCACHE_DIR = 'httpcache'
#HTTPCACHE_IGNORE_HTTP_CODES = []
#HTTPCACHE_STORAGE = 'scrapy.extensions.httpcache.FilesystemCacheStorage'
```
## 爬虫文件介绍
``` python
# -*- coding: utf-8 -*-
import scrapy
from 项目名称.items import 类名
import sys
reload(sys) #从新设置当前系统环境
sys.setdefaultencoding("utf-8") #设置系统环境的默认编码
#scrapy.Spider 是所有爬虫的父类,最基础的类
#scrapy.CrawlSpider 所有爬虫的父类
class BaiduSpider(scrapy.Spider):
    # 表示爬虫名称 执行指定的名字 scrapy crawl baidu
    name = 'baidu'
    # 抓取网页允许的域名范围 不写则不限制
    allowed_domains = ['baidu.com']
    # 起始Url地址, 爬虫启动的入口url 并发数量由配置的CONCURRENT_REQUESTS来限制
    start_urls = ['http://baidu.com/']
    #start_urls里的响应默认处理函数
    def parse(self, response):
        item = 类名()
        item['name'] = name
        yield item  #返回管道可以处理的数据
        yield scrapy.Request(stat_urls+str(10),meta = {"position": item, "s":s}, callback = self.parse)   #返回下一个请求 meta是传参(可以多个参数),response.meta["position"]获取参数, callback是回调函数
```
## 管道文件介绍
``` python
# -*- coding: utf-8 -*-

# Define your item pipelines here
#
# Don't forget to add your pipeline to the ITEM_PIPELINES setting
# See: https://doc.scrapy.org/en/latest/topics/item-pipeline.html
import sys
reload(sys) #从新设置当前系统环境
sys.setdefaultencoding("utf-8") #设置系统环境的默认编码
class TestPipeline(object):
    #参数初始化
    def __init__(self):
        pass
        
    #爬虫启动时候会调用
    def open_spider(self,spider):
        self.f = open("test.json","w")

    # 每次数据来都调用
    def process_item(self, item, spider):
        coutent = json.dumps(dict(item),ensure_ascii = False)
        self.f.write(content)
        return item

    #爬虫结束会调用
    def close_spider(self,spider):
        self.f.close()

#需要去settings.py设置启用管道
ITEM_PIPELINES = {
   'Test.pipelines.TestPipeline': 100,
}
#文件生成在执行命名的目录下 
```

## 流程
编写Scrapy爬虫的一般流程：
1. 创建项目：scrapy startproject XXX
2. 创建爬虫: scrapy genspider xxx
3. 编写items.py：定义保存数据的字段 name = scrapy.Field()
4. 编写爬虫xxx.py：提取响应文件的数据，yield item -> 管道处理；yield Request -> 调度器处理
5. 编写pipelines.py：存储item数据
6. 编写settings.py：启用 ITEM_PIPELINES，设置相关配置信息（如USER_AGENT等...）
7. 执行爬虫：scrapy crawl xxx
