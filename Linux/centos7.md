# 安装vscode

    [参考](https://code.visualstudio.com/docs/setup/linux#_rhel-fedora-and-centos-based-distributions)

    sudo rpm --import https://packages.microsoft.com/keys/microsoft.asc

    sudo sh -c 'echo -e "[code]\nname=Visual Studio Code\nbaseurl=https://packages.microsoft.com/yumrepos/vscode\nenabled=1\ngpgcheck=1\ngpgkey=https://packages.microsoft.com/keys/microsoft.asc" > /etc/yum.repos.d/vscode.repo'

    yum check-update
    sudo yum install code

