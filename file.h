#include <iostream>
#include <vector>
#include <sys/types.h>
#include <dirent.h>  
#include <cstring>

void GetFileName(std::string path, std::vector<std::string> &files) {
    DIR *pDir;   //  是头文件<dirent.h>的类型
    struct dirent *ptr;  // opendir、readdir这些都是头文件dirent.h
    if (!(pDir = opendir(path.c_str()))) return;
    while ((ptr = readdir(pDir)) != 0) {
        // strcmp是C语言里的，只导入string,然后std::strcmp都是没有的
        if (strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0) {
            files.push_back(path + "/" + ptr->d_name);  // 可以只保留名字
        }
    }
    closedir(pDir);
}