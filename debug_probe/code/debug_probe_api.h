#pragma once

#include <stddef.h>

typedef int *(cb_dp_probe)(void* data, size_t len);

int dp_module_init(const char* tcp_ip);

void dp_module_deinit();

int dp_module_add_probe(const char cmd, cb_dp_probe dp_probe);
