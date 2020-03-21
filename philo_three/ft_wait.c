#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "ft_wait.h"
#include "ft_run_process.h"

static int  ft_restart_eat_ok_process(t_philo_status *p_status)
{
    if ((sem_post(p_status->write_sem)) == -1)
        return (-1);
    p_status->eat_ok = 1;
    p_status->added = 1;
    p_status->eat_count = p_status->infos->nb_time_eat;
    if ((p_status->eaten_time = ft_get_current_time()) == -1)
        return (-1);
    if ((p_status->pid = fork()) == -1)
        return (-1);
    if (p_status->pid == 0)
        ft_run_philo_process(p_status);
    return (0);
}

static void *ft_wait(void *arg)
{
    t_philo_status      *p_status;
    int                 stat;

    p_status = (t_philo_status*)arg;
    waitpid(p_status->pid, &stat, 0);
    if (WEXITSTATUS(stat) == EAT_ENOUGH)
    {
        p_status->infos->nb_philo_finished = p_status->infos->nb_philo_finished + 1;
        if (p_status->infos->nb_philo_finished == p_status->infos->nb_philo ||
                ft_restart_eat_ok_process(p_status) == -1)
        {
            p_status->infos->end = 1;
            p_status->ender = 1;
        }
        else
            if ((pthread_create(&(p_status->wait_thread), NULL, ft_wait, p_status)))
                p_status->infos->end = 1;
    }
    else if (WEXITSTATUS(stat) == PHILO_DIED || WEXITSTATUS(stat) == 1)
    {
        p_status->infos->end = 1;
        p_status->ender = 1;
    }
    return p_status;
}

int         ft_wait_end(t_philo_infos *p_infos, t_philo_status *p_status, sem_t *forks_sem, sem_t *write_sem)
{
    int i;

    while (1)
    {
        if (p_infos->end == 1)
        {
            usleep(1000 * 50);
            i = 0;
            while (i < p_infos->nb_philo)
            {
                if (!(p_status[i].ender))
                    kill(p_status[i].pid, SIGKILL);
                i++;
            }
            sem_unlink(FORKS_SEM);
            sem_unlink(WRITE_SEM);
            sem_close(forks_sem);
            sem_close(write_sem);
            free(p_status);
            return (0);
        }
    }
    return (0);
}

int         ft_create_wait_threads(t_philo_infos *p_infos, t_philo_status *p_status)
{
    int i;
    int ret;

    i = 0;
    ret = 0;
    while (i < p_infos->nb_philo)
    {
        if ((ret = pthread_create(&(p_status[i].wait_thread), NULL, ft_wait, &(p_status[i]))))
            return (ret);
        i++;
    }
    i = 0;
    while (i < p_infos->nb_philo)
    {
        if ((ret = pthread_detach(p_status[i].wait_thread)))
            return (ret);
        i++;
    }
    return (ret);
}
