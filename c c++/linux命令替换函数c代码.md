# 是否运行
```c
int LinuxCmdUtil::is_running(const HString &process_name)
{
    struct dirent **entries;
    int num_entries = scandir("/proc", &entries, NULL, alphasort);
    if (num_entries < 0)
    {
        return 0;
    }
    char exe_link[PATH_MAX];
    char exe_path[PATH_MAX];
    int run_id = 0;
    int i = 0;

    for (; i < num_entries; ++i)
    {
        run_id = strtol(entries[i]->d_name, NULL, 10);
        if (run_id != 0)
        {
            snprintf(exe_link, sizeof(exe_link), "/proc/%ld/exe", run_id);
            // 读取符号链接
            ssize_t len = readlink(exe_link, exe_path, sizeof(exe_path));
            if (len != -1)
            {
                exe_path[len] = '\0';
                char *base_name = basename(exe_path);
                if (strstr(base_name, process_name.get_str().c_str()) != NULL)
                {
                    break;
                }
            }
        }

        free(entries[i]);
    }
    for (; i < num_entries; ++i)
    {
        free(entries[i]);
    }
    free(entries);
    return run_id;
}
```