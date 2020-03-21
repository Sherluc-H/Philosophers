#include <unistd.h>
#include <stdlib.h>
#include "ft_run_process.h"
#include "ft_write_state.h"
#include "ft_free.h"

static void *ft_check_if_died(void *arg)
{
    t_philo_status  *p_status;
    long            current_time;

    p_status = (t_philo_status*)arg;
    while (1)
    {
        if (p_status->added == 0 && p_status->eat_ok == 1 &&
            p_status->eat_count == p_status->infos->nb_time_eat)
        {
            p_status->infos->end = 1;
            return p_status;
        }
        else
        {
            if ((current_time = ft_get_current_time()) == -1)
                return p_status;
            if (current_time - p_status->eaten_time >
                    p_status->infos->time_td + 5)
            {
                ft_announce_death(p_status);
                return p_status;
            }
        }
    }
    return p_status;
}

static int  ft_eat_ok_begin(t_philo_status *p_status)
{
    if ((usleep(1000 * p_status->infos->time_te)) == -1)
        return (-1);
    if ((sem_post(p_status->forks_sem)) == -1)
        return (-1);
    sem_post(p_status->forks_sem);
    if ((ft_write_state(p_status, " is sleeping\n",
            p_status->write_sem, 0)) == -1)
        return (-1);
    if ((usleep(1000 * p_status->infos->time_ts)) == -1)
        return (-1);
    return (0);
}

static int  ft_take_forks_n_eat(t_philo_status *p_status)
{
    if ((sem_wait(p_status->forks_sem)))
        return (-1);
    if ((ft_write_state(p_status, " has taken a fork\n", p_status->write_sem, 0)) == -1)
        return (ft_unlock_sem(p_status->forks_sem, NULL, -1));
    if ((sem_wait(p_status->forks_sem)))
        return (ft_unlock_sem(p_status->forks_sem, NULL, -1));
    if ((ft_write_state(p_status, " has taken a fork\n", p_status->write_sem, 0)) == -1)
        return (ft_unlock_sem(p_status->forks_sem, p_status->forks_sem, -1));
    if ((p_status->eaten_time = ft_get_current_time()) == -1)
        return (ft_unlock_sem(p_status->forks_sem, p_status->forks_sem, -1));
    if ((ft_write_state(p_status, " is eating\n", p_status->write_sem, 1)) == -1)
        return (ft_unlock_sem(p_status->forks_sem, p_status->forks_sem, -1));
    if ((usleep(1000 * p_status->infos->time_te)) == -1)
        return (ft_unlock_sem(p_status->forks_sem, p_status->forks_sem, -1));
    if ((sem_post(p_status->forks_sem)) == -1)
        return (-1);
    sem_post(p_status->forks_sem);
    return (0);
}

static void *ft_philo_thread(void *arg)
{
    t_philo_status  *p_status;

    p_status = (t_philo_status*)arg;
    if (p_status->eat_ok)
    {
        if ((ft_eat_ok_begin(p_status)) == -1)
            return p_status;
    }
    while (!(p_status->died))
    {
        if ((ft_write_state(p_status, " is thinking\n", p_status->write_sem, 0)) == -1)
            return p_status;
        if ((ft_take_forks_n_eat(p_status)) == -1)
            return p_status;
        if ((ft_write_state(p_status, " is sleeping\n", p_status->write_sem, 0)) == -1)
            return p_status;
        if ((usleep(1000 * p_status->infos->time_ts)) == -1)
            return p_status;
    }
    return p_status;
}

void        ft_run_philo_process(t_philo_status *p_status)
{
    sem_t       *forks_sem;
    sem_t       *write_sem;
    pthread_t   philo_thread;
    pthread_t   thread_alive;

    if ((forks_sem = sem_open(FORKS_SEM, 0)) == SEM_FAILED)
        exit(1);
    if ((write_sem = sem_open(WRITE_SEM, 0)) == SEM_FAILED)
        exit(1);
    p_status->forks_sem = forks_sem;
    p_status->write_sem = write_sem;
    if ((pthread_create(&(thread_alive), NULL, ft_check_if_died, p_status)))
        exit(1);
    if ((pthread_create(&(philo_thread), NULL, ft_philo_thread, p_status)))
        exit(1);
    if ((pthread_detach(philo_thread)))
        exit(1);
    if ((pthread_join(thread_alive, NULL)))
        exit(1);
    if (p_status->died)
        exit(PHILO_DIED);
    else if (p_status->added == 0 && p_status->eat_ok &&
            p_status->eat_count == p_status->infos->nb_time_eat)
        exit(EAT_ENOUGH);
    exit(1);
}
