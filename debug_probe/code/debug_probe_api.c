/*
* Name: eperfm-base.c
*
* Descriptions:
*
* Date: 2022-09-19
*
* Author: zhangzhen<zhangzhen@china-aicc.cn>
*
* Copyright 2022 – 2024 AICC Inc.
*
* These materials and the intellectual property rights associated therewith
* (“Materials”) are proprietary to AICC Inc. Any and all rights in
* the Materials are reserved by AICC Inc. Nothing contained in
* these Materials shall grant or be deemed to grant to you or anyone else, by
* implication, estoppel or otherwise, any rights in these Materials.
*
*/
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <errno.h>
#include "debug_probe_api.h"
/*
 * definitions
 */
#define SP_VERSION "V1.0.0"

//functions
int dp_module_init(const char* shm_name, unsigned int size, void** shmptr)
{
    void* addr = NULL;
    int fd;
    struct stat st;
    char shm_file_name[32];
    bool to_create = true;
    long long freespace = 0;
    struct statfs shm_statfs;

    //if (access(EPERFM_BASE_DEBUG_MORE_SWITCH, F_OK) == 0) {
    //    debug_info_more = true;
    //}

    memset(shm_file_name, 0, 32);
    sprintf(shm_file_name, "/dev/shm/%s", shm_name);
    if (-1 == stat (shm_file_name, &st)) {
        to_create = true;
        printf("eperfm_node_pool : Create!\n");
    } else {
        to_create = false;
        printf("eperfm_node_pool : Attach!\n");
    }

    if (to_create) {
        if( statfs("/dev/shm", &shm_statfs) >= 0 ) {
            freespace = (((long long)shm_statfs.f_bsize * (long long)shm_statfs.f_bfree) /(long long)(1024));
            if (freespace <= (((long long)size/1024) + 1024)) {
                printf("/dev/shm partation has no enough space for eperfm-base\n");
                return dp_err_no_space;
            }
        }
        shm_unlink(shm_name);
        fd = shm_open(shm_name, O_CREAT | O_EXCL | O_TRUNC | O_RDWR, 0666);  /* 0444->0600*/
        if (fd == -1) {
            perror("shm open failed for perf_mon_node_pool");
            return dp_err_shm_open_fail;
        }

        if (ftruncate(fd, size) == -1) {
            shm_unlink(shm_name);
            perror("shm ftruncate failed for perf_mon_node_pool");
            return dp_err_file_ftruncate;
        }

        addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (addr == MAP_FAILED) {
            shm_unlink(shm_name);
            perror("shm mmp failed for perf_mon_node_pool");
            return dp_err_nmap;
        }
        if (-1 == chmod(shm_file_name,0666)) {
            printf("Chmod failed\n");
        }

        *shmptr = addr;
    } else {
        fd = shm_open(shm_name, O_RDWR, 0);  /* 0444->0600*/
        if (fd == -1) {
            perror("shm attach failed for perf_mon_node_pool");
            return dp_err_shm_open_fail;
        }

        addr = (char*)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (addr == MAP_FAILED) {
            shm_unlink(shm_name);
            perror("shm attach mmap failed for perf_mon_node_pool");
            return dp_err_nmap;
        }

        *shmptr = addr;
    }
    return dp_err_ok;
}

void dp_module_deinit(const char* shm_name, size_t size, void* shmptr)
{
    if (shmptr) {
        munmap((char*)shmptr, size);
        shm_unlink(shm_name);
        shmptr = NULL;
    }
}

