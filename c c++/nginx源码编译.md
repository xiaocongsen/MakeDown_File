# ubuntu编译安装nginx
    git clone https://github.com/nginx/nginx.git
# nginx 提示错误:./configure: error: the HTTP XSLT module requires the libxml2/libxslt
    sudo apt-get install libxml2 libxml2-dev libxslt-dev