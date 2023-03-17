#pragma once

#include <stddef.h>

enum {
    dp_err_ok = 0,
    dp_err_no_space,
    dp_err_shm_open_fail,
    dp_err_file_ftruncate,
    dp_err_nmap,
};


int dp_module_init(const char* shm_name, unsigned int size, void** shmptr);

void dp_module_deinit(const char* shm_name, size_t size, void* shmptr);

