#pragma once

#include <stdint.h>

#ifndef CONFIG_ENABLE_CHECKSUM
#define CONFIG_ENABLE_CHECKSUM
#endif

#ifndef CONFIG_ENABLE_FCS16
#define CONFIG_ENABLE_FCS16
#endif

#ifndef CONFIG_ENABLE_FCS32
#define CONFIG_ENABLE_FCS32
#endif

#ifndef DOXYGEN_SHOULD_SKIP_THIS


typedef uintptr_t tiny_mutex_t;
typedef uintptr_t tiny_cond_t;

typedef struct
{
    uint8_t bits;
    uint16_t waiters;
    tiny_mutex_t m_mutex;
    tiny_cond_t  m_cond;
} tiny_events_t;

#endif
