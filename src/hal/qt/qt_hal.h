#pragma once

#include <stdint.h>
#include <time.h>

#include <QMutex>
#include <QWaitCondition>

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

/**
 * Mutex type used by Tiny Protocol implementation.
 * The type declaration depends on platform.
 */
typedef QMutex tiny_mutex_t;

/**
 * Events group type used by Tiny Protocol implementation.
 * The type declaration depends on platform.
 */
typedef struct
{
    uint8_t bits;
    uint16_t waiters;
    QMutex mutex;
    QWaitCondition cond;
} tiny_events_t;

#endif
