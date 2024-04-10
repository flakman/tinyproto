#include <errno.h>

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QElapsedTimer>
#include <QDeadlineTimer>

void tiny_mutex_create(tiny_mutex_t *mutex)
{
    *mutex = reinterpret_cast<tiny_mutex_t>(new QMutex());
}

void tiny_mutex_destroy(tiny_mutex_t *mutex)
{
    delete reinterpret_cast<QMutex *>(*mutex);
}

void tiny_mutex_lock(tiny_mutex_t *mutex)
{
    reinterpret_cast<QMutex *>(*mutex)->lock();
}

uint8_t tiny_mutex_try_lock(tiny_mutex_t *mutex)
{
    if ( reinterpret_cast<QMutex *>(*mutex)->tryLock() )
        return 1;
    else
        return 0;
}

void tiny_mutex_unlock(tiny_mutex_t *mutex)
{
    reinterpret_cast<QMutex *>(*mutex)->unlock();
}

void tiny_events_create(tiny_events_t *events)
{
    events->bits = 0;
    events->waiters = 0;
    events->m_mutex = reinterpret_cast<tiny_mutex_t>(new QMutex());
    events->m_cond  = reinterpret_cast<tiny_cond_t>(new QWaitCondition());
}

void tiny_events_destroy(tiny_events_t *events)
{
    delete reinterpret_cast<QMutex*>(events->m_mutex);
    delete reinterpret_cast<QWaitCondition*>(events->m_cond);
}

uint8_t tiny_events_wait(tiny_events_t *events, uint8_t bits, uint8_t clear, uint32_t timeout)
{
    QMutex*        e_mutex = reinterpret_cast<QMutex*>(events->m_mutex);
    QWaitCondition* e_cond = reinterpret_cast<QWaitCondition*>(events->m_cond);

    QMutexLocker locker(e_mutex);
    ++(events->waiters);
    uint8_t locked = 0;
    QDeadlineTimer dlt(timeout == 0xFFFFFFFF ? QDeadlineTimer::Forever : timeout);

    while ( (events->bits & bits) == 0 )
    {
        if ( !e_cond->wait(e_mutex, dlt) )
        {
            locked = events->bits;
            if ( clear )
            {
                events->bits &= ~bits;
            }

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
    QMutexLocker locker(reinterpret_cast<QMutex *>(events->m_mutex));
    events->bits |= bits;
    reinterpret_cast<QWaitCondition *>(events->m_cond)->wakeAll();
}

void tiny_events_clear(tiny_events_t *events, uint8_t bits)
{
    QMutexLocker locker(reinterpret_cast<QMutex *>(events->m_mutex));
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
    if ( !timer.isValid() )
    {
        timer.start();
    }

    const qint64 elapsed_ns = timer.nsecsElapsed();
    const uint32_t elapsed_us = static_cast<uint32_t>(elapsed_ns / 1000 + (((elapsed_ns % 1000) < 500) ? 0 : 1));

    // us since start the timer (first call of this function):
    return elapsed_us;
}
