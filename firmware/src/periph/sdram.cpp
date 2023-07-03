#include "periph/sdram.hpp"

#if !defined(TEST)
float __attribute__((section(".sdram_bss"))) sdram_buf[sdram_buf_size];
#else
float sdram_buf[sdram_buf_size];
#endif

MemoryManager memory_manager { .memory_size = sdram_buf_size };
