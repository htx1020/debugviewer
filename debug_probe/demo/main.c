#include <stdio.h>
#include <unistd.h>
#include "debug_probe_api.h"

const char shmname[] = "github-worker";
const size_t shmsize = 1024;
int main()
{
    printf("Hello world\n");
    int pid = fork();
    if(pid > 0) {
        void* ptr = NULL;
        int ret = dp_module_init(shmname, shmsize, &ptr);
        if(ret != 0) {
            return -1;
        }
        char* str = (char*)ptr;
        int i = 0;
        while(1) {
            snprintf(str, shmsize, "shm work counter %d ... \n", i++);
            sleep(1);
        }
    } else if(pid == 0) {
        void* ptr = NULL;
        int ret = dp_module_init(shmname, shmsize, &ptr);
        if(ret != 0) {
            return -1;
        }
        char* str = (char*)ptr;
        while(1) {
            sleep(1);
            printf("Read: %s\n", str);
        }
    } else {
        printf("error\n");
    }
    return 0;
}

