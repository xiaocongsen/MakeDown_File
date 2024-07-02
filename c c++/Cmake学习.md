# 案例

```c

project(mboxd-api)         //定义CMake项目的开始。它设置了项目的名称为mboxd-api   


set(CMKAE_C_COMPILE clang)
set(CMKAE_CXX_COMPILE clang++)              //设置了C和C++编译器

if (NOT DEFINED target_api)                 //变量不存在
set(target_api mboxd-api)                   //设置一个变量target_api 的值是mboxd-api
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)       //生成compile_commands.json文件，该文件用于与一些IDE（如Visual Studio Code）集成，以提供更好的代码导航和补全。

message(STATUS "Using MLIBS Path : ${target_api}")          //输出变量
include_directories(${PROJECT_SOURCE_DIR}/include)      //设置头文件查找目录

set(project_src                                         //设置一个变量project_src 包含所有源代码文件列表
#    src/main.cpp
    src/mboxd_api.hpp
    src/mboxd_api.cpp
    src/json.hpp
)

if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "aarch64")                       //CMAKE_HOST_SYSTEM_PROCESSOR是CMake自带的预定义变量 用于描述主机系统的处理器架构
include_directories(${PROJECT_SOURCE_DIR}/../mlibs_aarch64/include)     
link_directories(                                                       //设置链接库查找目录
	${PROJECT_SOURCE_DIR}/libs/aarch64
	${PROJECT_SOURCE_DIR}/../mlibs_aarch64/lib
	)
elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "x86_64")
include_directories(${PROJECT_SOURCE_DIR}/../mlibs_x86_64/include)
link_directories(
	${PROJECT_SOURCE_DIR}/libs/x64
	${PROJECT_SOURCE_DIR}/../mlibs_x86_64/lib
	)
endif()


#add_executable(${target_api} ${project_src})   
add_library(${target_api} STATIC ${project_src})

target_link_libraries(${target_api} PUBLIC
    cpr
    curl
    z
    pthread
    fmt
)

```
