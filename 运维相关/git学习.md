# Git学习

    git init
    git clone
    git add
    git status         -s 参数，以获得简短的结果输出
    AM
    git diff          --cached     / HEAD        /--stat
    git commit  -m/ -a
    git reset HEAD
    git rm   -f   /--cached  / –r
    git mv
    git branch /(branchname)  /-d
    git checkout (branchname)  /-bgit merge
## 删除分支
1. 删除本地分支
    git branch -d 分支名
2. 删除远程分支
    git push origin --delete 分支名

3. 删除未跟踪文件   加上-n打印会删除的文件
    git clean -f
4. 未跟踪的目录也一起删掉
    git clean -fd
5. 连 gitignore 的未跟踪的文件/目录也一起删掉 （慎用，一般这个是用来删掉编译出来的 .o之类的文件用的）
    git clean -xfd
6. 清理本地所有修改
  git checkout . 

## 初始化流程
    git init
    git add -A
    git commit -m "init"
    git remote add origin ssh://git@ds-project.cn:10022/xiaocs/hiRedisTest.git
    git push origin master:master


## 查看提交记录
    git log   //最新的在最前面

## 加分支流程
    git checkout -b 新分支名字
    git add -A
    git commit -m "init"
    git push origin 新分支名字:新分支名字

## 切换远程分支
    git branch -a       查看所有分支
     git checkout -b 命名本地分支 远程分支名  

## 合并分支流程
    当前分支没有修改
    git merge 需要合并过来的版本名

## 放弃本地修改
    git status 查看当前文件状态
1. 未使用 git add 缓存代码
    git checkout -- filepathname
2. 已经使用了  git add 缓存了代码
    git reset HEAD filepathname
3. 已经用 git commit  提交了代码
    git reset --hard commitId
    commitId 可以通过git log输出    (HEAD -> master)为当前所在分支


## git的http切ssh
    git remote -v                                       # 查看当前远程仓库配置
    git remote remove origin                            # 如果origin是HTTP形式，先删除它
    git remote add origin git@hostname:path/to/repo.git # 添加新的SSH形式的远程仓库地址


## git 本地添加私钥
    eval "$(ssh-agent -s)"
    ssh-add ~/.ssh/id_rsa

## git 设置本地追踪远程分支
    git branch --set-upstream-to=origin/xcs xcs-local

# git问题处理
## git clone报错 server certificate verification failed. CAfile: none CRLfile: none
    git config --global http.sslverify false
    git config --global https.sslverify false
    export GIT_SSL_NO_VERIFY=1