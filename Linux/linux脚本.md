
# 获取某个应用的依赖，并重新打包
```shell
#!/bin/bash
debname = $1
# 获取直接依赖和推荐的包名，使用更宽松的正则表达式以尽可能准确地匹配包名
depends=$(sudo apt-cache depends $debname | grep -oP '^  依赖: \K([\w\+\.:_-]+)')
recommends=$(sudo apt-cache depends $debname | grep -oP '^  推荐: \K([\w\+\.:_-]+)')

# 合并依赖和推荐列表，去除重复项
packages=($(echo "$depends $recommends" | tr ' ' '\n' | sort -u))

# 合并依赖和推荐列表，去除重复项
packages=($(echo "$depends $recommends" | tr ' ' '\n' | sort -u))

# 为每个包重新打包
for pkg in "${packages[@]}"; do
    echo "正在重新打包: $pkg"
        sudo dpkg-repack $pkg
        echo "$pkg 重新打包成功"
    else
        echo "警告: 未找到 $pkg 的原始 .deb 文件，跳过重新打包"
    fi
done
```



```shell
#!/bin/bash

# 要复制的目标文件
TARGET=xxx

# 目标目录根
DEST_DIR_ROOT=/path/to/destination/directory

# 使用 ldd 列出依赖库并逐个复制
ldd "$TARGET" | awk '{if (match($3, "/")) print $3}' | while read -r lib; do
    # 获取软链接的目标（如果存在）
    if [ -L "$lib" ]; then
        lib=$(readlink -f "$lib")  # 解析软链接到实际路径
    fi
    
    # 构建在目标目录中的相对路径
    relative_path="${lib#$PREFIX/}"  # 假设$PREFIX是lib的共同前缀，这里简化处理为从根开始
    dest_dir="$DEST_DIR_ROOT/$relative_path"
    
    # 在目标目录中创建相应目录结构
    mkdir -p "$(dirname "$dest_dir")"
    
    # 复制库文件到目标目录
    cp "$lib" "$dest_dir"
done

echo "All dependencies have been copied to $DEST_DIR_ROOT maintaining directory structure"

```

```Shell
#!/bin/bash

# 第一个参数是$1
name="$1"
if [[ $name =~ "/" ]]; then
    echo "$name 是一个包含路径的文件名。"
else
    echo "$name 不包含路径分隔符，可能是一个相对路径或简单文件名。"
fi
# 使用查询参数执行dpkg -S命令，并提取包名
variable_name=$(dpkg -S "$name" | awk -F "'" '{print $2}')

if [ -n "$variable_name" ]; then
    echo "The package providing the input query ('$name') is: $variable_name"
else
    echo "No package found for the input query ('$name')."
fi
```