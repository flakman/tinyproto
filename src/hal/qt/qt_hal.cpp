#include <stdint.h>
#include <errno.h>

#include <QThread>
#include <QElapsedTimer>
#include <QDeadlineTimer>

#include "qt_hal.h"

void tiny_mutex_create(tiny_mutex_t *mutex)
{
}

void tiny_mutex_destroy(tiny_mutex_t *mutex)
{
}

void tiny_mutex_lock(tiny_mutex_t *mutex)
{
    mutex->lock();
}

uint8_t tiny_mutex_try_lock(tiny_mutex_t *mutex)
{
    if ( mutex->tryLock() )
        return 1;
    else
        return 0;
}

void tiny_mutex_unlock(tiny_mutex_t *mutex)
{
    mutex->unlock();
}

void tiny_events_create(tiny_events_t *events)
{
    events->bits = 0;
    events->waiters = 0;
}

void tiny_events_destroy(tiny_events_t *events)
{
}

uint8_t tiny_events_wait(tiny_events_t *events, uint8_t bits, uint8_t clear, uint32_t timeout)
{
    QMutexLocker(&events->mutex);
    ++events->waiters;
    uint8_t locked = 0;
    QDeadlineTimer dlt(timeout == 0xFFFFFFFF ? QDeadlineTimer::Forever : timeout);

    while ( (events->bits & bits) == 0 )
    {
        if ( !events->cond.wait(&events->mutex, dlt) )
        {
            locked = events->bits;
            if ( clear )
               { events->bits &= ~bits; }

            break;
        }
    }
    --events->waiters;

    return locked;
}

uint8_t tiny_events_check_int(tiny_events_t *event, uint8_t bits, uint8_t clear)
{
    return tiny_events_wait(event, bits, clear, 0);
}

void tiny_events_set(tiny_events_t *events, uint8_t bits)
{
    QMutexLocker(&events->mutex);
    events->bits |= bits;
    events->cond.wakeAll();
}

void tiny_events_clear(tiny_events_t *events, uint8_t bits)
{
    QMutexLocker(&events->mutex);
    events->bits &= ~bits;
}

void tiny_sleep(uint32_t millis)
{
    QThread::msleep(millis);
}

void tiny_sleep_us(uint32_t us)
{
    // QThread::usleep(us); // on Windows this rounding up to 1 ms inside...

    QDeadlineTimer deadline(Qt::PreciseTimer);
    deadline.setPreciseDeadline(0, us * 1000, Qt::PreciseTimer);

    while ( !deadline.hasExpired() );
}

uint32_t tiny_millis()
{
    static QElapsedTimer timer;
    timer.start();

    // ms since system startup:
    return timer.msecsSinceReference();
}

uint32_t tiny_micros()
{
    static QElapsedTimer timer;
    if(!timer.isValid())
    {
        timer.start();
    }

    const qint64 elapsed_ns = timer.nsecsElapsed();
    const uint32_t elapsed_us = static_cast<uint32_t>(elapsed_ns / 1000 + (((elapsed_ns % 1000) < 500) ? 0 : 1)); 

    // us since start the timer (first call of this function):
    return elapsed_us;
}
