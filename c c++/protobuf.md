# 编译安装
    sudo apt-get install autoconf automake libtool curl make g++ unzip
    https://github.com/protocolbuffers/protobuf/releases 下载最新releases cpp包
    ./autogen.sh
    ./configure
    make -j4
    sudo make install
    sudo ldconfig
    protoc --version

# 使用
    创建消息配置：
``` json
syntax = "proto3";

package Data;


//GetFileData
//PutFile
//PutAttr
message file_info {
  uint64    creation_time = 1;
  uint64    change_time = 2;
  uint64    file_size = 3;
  int64   file_attributes = 4;
  string    sha2 = 5;
  string    url = 6;
  string    file_name = 7;
}

//    GetFile
//    Delete
message GetFileRequest{
    string    url = 1;
    string    sha2 = 2;
}


//GetAllFile
message MoveUserAllFileRequest{
    repeated string sha2 = 1;
}

enum ResultStatus
{
    Ok = 0;
    Fail = 1;
};

//Register
//Login
//Logout
//ChangeName
//PutFile
//PutAttr
//Delete 
//Snapshot 
//SnapRestore 
message CommonResult{
    int32 status = 1;
    string msg = 2;
}

//GetFileList
message FileInfoResult{
    repeated file_info file_list = 1;
}

```
protoc -I=$SRC_DIR --cpp_out=$DST_DIR $SRC_DIR/address.proto
-I指定proto文件所在的位置
--cpp_out表示生成文件为C++文件,生成目录在DST_DIR