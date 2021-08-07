# django框架
## 安装
    pip install django == 2.1.4

## 创建项目
    django-admin startproject web_book
## 建立子应用
    cd web_book
    python manage.py startapp book
## 注册子应用
    修改文件settings.py中 INSTALLED_APPS字段 在后面添加
    'book.apps.BookConfig',
    book为子应用文件夹名
    apps为子应用入口文件名
    BookConfig为子应用入口文件的入口类名


## 关联mysql数据库
1. 创建book数据库 
    CREATE DATABASE book DEFAULT CHARACTER SET utf8;
2. vi web_book/web_book/__init__.py 用下面内容覆盖
``` py
    from pymysql import install_as_MySQLdb
    install_as_MySQLdb()
```
3. 修改文件settings.py中 DATABASES 字段
``` py
DATABASES = {
    'default': {
        'ENGINE': 'django.db.backends.mysql',
        'HOST':'192.168.1.135',
        'PORT':3306,
        'USER':'root',
        'PASSWORD':'passord',
        'NAME':'book',
    }
}
```
4. 执行 
    pip install Pillow
5. 修改子应用目录中的models.py
``` py
# 1.创建模型类
class BookInfoData(models.Model):
    # 2.定义字段  属性
    # 大分类
    category = models.CharField(max_length=50, verbose_name='大类', default='大类')
    small_category = models.CharField(max_length=50,verbose_name='小类',default='小类')
    name = models.CharField(max_length=200, default="无", verbose_name="书名")
    author = models.CharField(max_length=50, default="无", verbose_name="作者")
    store = models.CharField(max_length=100, default="无", verbose_name="出版社")
    price = models.DecimalField(decimal_places=2, max_digits=10, default="0.00", verbose_name="价格")
    default_image = models.ImageField(null=True, verbose_name="图片")

    class Meta:
        verbose_name = "图书"
        verbose_name_plural = verbose_name

    # 魔法函数
    def __str__(self):
        return self.name
```
6. 数据迁移
    python manage.py makemigrations
    python manage.py migrate
7. 创建后台管理用户
    python manage.py createsuperuser
8. 更改页面语言
    修改文件settings.py:
```py
    LANGUAGE_CODE = 'zh-hans'
    TIME_ZONE = 'Asia/Shanghai'
```

## 启动项目
    python manage.py runserver localhost:8000
    即可运行前端：http://localhost:8000/
    后台管理： http://localhost:8000/admin
    ctrl + c 退出