# mmap
```c++
    int in_fd = data.fd;
    int out_fd = open(file_put.metadata().sha2().c_str(),O_RDWR|O_CREAT|O_EXCL|O_TRUNC, 0666);
    if(out_fd != -1)
    {
        if(file_put.is_data())
        {
            send_len = file_put.metadata().file_size();
            if(ftruncate(out_fd, count))
            {
                LOG(DEBUG) << "truncate file error size: " << count;
                return rwsErrClose;
            }
            void* p = mmap(nullptr, count+1, PROT_WRITE, MAP_SHARED, out_fd, 0);
            if(p == MAP_FAILED)
            {
                perror("mmap");
                LOG(DEBUG) << "mmap file error size: " << count;
                return rwsErrClose;
            }
            ReadWriteState ret = ReadDataMsg(in_fd,(char *)p,count);
            if (-1==munmap(p, count+1))
            {
                perror("munmap");
            }
        }
    }
```

# sendfile
```c++
    int out_fd = socketfd;
    int in_fd = open(get_file.sha2().c_str(),O_RDWR);
    if(in_fd != -1)
    {
        teskid = get_file.teskid();
        uint64_t send_len = 1024;//get_file.get_size();
        lseek(in_fd, 100, SEEK_SET);
        ssize_t ret = sendfile(out_fd,in_fd,nullptr,send_len);
        if(ret != -1)
        {
            send_len -= ret;
            int index = 0;
            while (send_len)
            {
                LOG(CRITICAL) << "ReadWriteThread HandleGetFileOperation sendfile while TODO";
                if(errno == EINTR || (errno == EAGAIN && size != 0))
                {
                    ++index;
                    if(index >= 5)
                    {
                        res.set_taskid(get_file.teskid());
                        SendCommonResult(data.fd,Pack::GetFile,res);
                        //                            close(data.fd);
                        data.type = 1;
                        goto END;
                    }
                    usleep(5000);
                    continue;
                }
                send_len -= sendfile(out_fd,in_fd,nullptr,send_len);
            }
            close(in_fd);
        }
        else
        {
            //                close(data.fd);
            data.type = 1;
            LOG(CRITICAL) << "ReadWriteThread HandleGetFileOperation sendfile ret = -1";
        }
    }
```

# splice
```c++
    int in_fd = socketfd;
    int pipefd[2];
    pipe(pipefd);
    int out_fd = open(file_put.metadata().sha2().c_str(),O_RDWR|O_CREAT|O_EXCL|O_TRUNC, 0666);
    if(out_fd != -1)
    {
        if(file_put.is_data())
        {
            send_len = file_put.metadata().file_size();
            int size = splice(in_fd,NULL,out_fd,NULL,send_len, SPLICE_F_MORE);
            if(size == -1)
            {
                LOG(WARNING) << "readDataMsg fd close errinfo:" << strerror(errno);
            }
            size = splice(pipefd[0], NULL, , NULL, send_len, SPLICE_F_MORE);
            if(size == -1)
            {
                LOG(WARNING) << "readDataMsg fd close errinfo:" << strerror(errno);
            }
        }
    }
```