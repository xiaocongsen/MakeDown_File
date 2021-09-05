activate            激活环境
conda --version      查看版本号
conda update conda      更新conda
conda create --name <环境名> <环境中的包名>        创建环境 列如:conda create -n python3 python=3.5 numpy pandas，即创建一个名为 “python3” 的环境，环境中安装版本为 3.5 的 Python，同时也安装了 numpy 和 pandas
conda activate <env_name>   # 切换环境
conda env list 或者 conda info -e 或者 conda info --envs  #列出全部环境可以是
conda create --name <复制之后新环境的名称> --clone <复制/克隆环境名>        #复制环境
conda remove --name <env_name> --all                #删除环境
conda env export > environment.yaml                 #导出当前环境的包信息
conda update --all                          #更新所有包
conda update <package_name>                 #更新指定包
set CONDA_FORCE_32BIT=1                 #切换成32位环境