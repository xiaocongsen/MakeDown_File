# Sub-process /usr/bin/dpkg returned an error code (1)解决方案

    cd /var/lib/dpkg/info/
    ls|xargs sudo rm
    sudo apt-get clean
    sudo apt-get autoremove
    sudo apt-get autoclean
    sudo apt-get install -f
    sudo apt-get update 
