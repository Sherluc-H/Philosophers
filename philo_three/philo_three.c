#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>
#include "philo_three.h"
#include "ft_philo_utils.h"

int     ft_get_final_to_write(t_philo_infos *p_infos, char *to_write, char *str_end, int philo_num)
{
    long    current_time;
    int     i;

    i = 0;
    if ((current_time = ft_get_current_time()) == -1)
        return (-1);
    ft_add_nbr_to_str(current_time - p_infos->starting_time, to_write, &i);
    ft_add_str_to_str(" ", to_write, &i);
    ft_add_nbr_to_str(philo_num + 1, to_write, &i);
    ft_add_str_to_str(str_end, to_write, &i);
    to_write[i] = '\0';
    return (i);
}

int     ft_write_state(t_philo_status *p_status, char *str_end, sem_t *write_sem, int is_eating)
{
    char to_write[1024];
    int lenght;

    if (p_status->died)
        return (-1);
    if ((sem_wait(write_sem)) == -1)
        return (-1);
    if (p_status->died)
    {
        sem_post(write_sem);
        return (-1);
    }
    if ((lenght = ft_get_final_to_write(p_status->infos, to_write, str_end, p_status->philo_num)) != -1)
    {
        if (!(p_status->died))
            write(1, to_write, lenght);
    }
    if (is_eating)
    {
        p_status->eat_count = p_status->eat_count + 1;
        if (p_status->infos->nb_time_eat > 0 && !(p_status->eat_ok) && p_status->eat_count == p_status->infos->nb_time_eat)
        {
            p_status->eat_ok = 1;
            return (-1);
        }
    }
    if (p_status->died)
    {
        sem_post(write_sem);
        return (-1);
    }
    if ((sem_post(write_sem)) == -1)
        return (-1);
    if (p_status->died)
        return (-1);
    return (0);
}

int     ft_check_args(int argc, char *argv[], t_philo_infos *p_infos)
{
    if (argc != 5 && argc != 6)
    {
        write(2, "Wrong number of arguments\n", 26);
        return (1);
    }
    if ((ft_get_philo_infos(argc, argv, p_infos)) == -1)
    {
        write(2, "Wrong arguments\n", 16);
        return (1);
    }
    return (0);
}

static void *ft_check_if_died(void *arg)
{
    t_philo_status  *p_status;
    long    current_time;
    char    to_write[1024];
    int     lenght;

    p_status = (t_philo_status*)arg;
    while (1)
    {
        if (p_status->added == 0 && p_status->eat_ok == 1 && p_status->eat_count == p_status->infos->nb_time_eat)
        {
            p_status->infos->end = 1;
            return p_status;
        }
        if ((current_time = ft_get_current_time()) == -1)
        {
            return p_status;
        }
        if (current_time - p_status->eaten_time > p_status->infos->time_td + 5)
        {
            p_status->died = 1;
            p_status->infos->end = 1;
            sem_wait(p_status->write_sem);
            if ((lenght = ft_get_final_to_write(p_status->infos, to_write, " died\n", p_status->philo_num)) != -1)
                write(1, to_write, lenght);
            return p_status;
        }
    }
    return p_status;
}

static void *ft_philo_thread(void *arg)
{
    t_philo_status  *p_status;

    p_status = (t_philo_status*)arg;
    if (p_status->eat_ok)
    {
        usleep(1000 * p_status->infos->time_te);
        sem_post(p_status->forks_sem);
        sem_post(p_status->forks_sem);
        if ((ft_write_state(p_status, " is sleeping\n", p_status->write_sem, 0)) == -1)
            exit(PHILO_DIED);
        usleep(1000 * p_status->infos->time_ts);
    }
    while (!(p_status->died))
    {
        if ((ft_write_state(p_status, " is thinking\n", p_status->write_sem, 0)) == -1)
            return p_status;
        sem_wait(p_status->forks_sem);
        if ((ft_write_state(p_status, " has taken a fork\n", p_status->write_sem, 0)) == -1)
        {
            sem_post(p_status->forks_sem);
            return p_status;
        }
        sem_wait(p_status->forks_sem);
        if ((ft_write_state(p_status, " has taken a fork\n", p_status->write_sem, 0)) == -1)
        {
            sem_post(p_status->forks_sem);
            sem_post(p_status->forks_sem);
            return p_status;
        }
        p_status->eaten_time = ft_get_current_time();
        if ((ft_write_state(p_status, " is eating\n", p_status->write_sem, 1)) == -1)
            return p_status;
        usleep(1000 * p_status->infos->time_te);
        sem_post(p_status->forks_sem);
        sem_post(p_status->forks_sem);
        if ((ft_write_state(p_status, " is sleeping\n", p_status->write_sem, 0)) == -1)
            return p_status;
        usleep(1000 * p_status->infos->time_ts);
    }
    return p_status;
}

void    ft_run_philo_process(t_philo_status *p_status)
{
    sem_t   *forks_sem;
    sem_t   *write_sem;
    pthread_t   philo_thread;
    pthread_t   thread_alive;

    if ((forks_sem = sem_open(FORKS_SEM, 0)) == SEM_FAILED)
        exit(1);
    if ((write_sem = sem_open(WRITE_SEM, 0)) == SEM_FAILED)
        exit(1);
    p_status->forks_sem = forks_sem;
    p_status->write_sem = write_sem;
    pthread_create(&(thread_alive), NULL, ft_check_if_died, p_status);
    pthread_create(&(philo_thread), NULL, ft_philo_thread, p_status);
    pthread_detach(philo_thread);
    pthread_join(thread_alive, NULL);
    if (p_status->died)
        exit(PHILO_DIED);
    else if (p_status->added == 0 && p_status->eat_ok &&
            p_status->eat_count == p_status->infos->nb_time_eat)
        exit(EAT_ENOUGH);
    exit(1);
}

static int  ft_open_sem(sem_t **forks_sem, sem_t **write_sem, int mode)
{
    if ((*forks_sem = sem_open(FORKS_SEM, mode, 0660, 0)) == SEM_FAILED)
    {
        if ((sem_unlink(FORKS_SEM)) == -1)
            return (1);
        if ((*forks_sem = sem_open(FORKS_SEM, mode, 0660, 0)) == SEM_FAILED)
            return (1);
    }
    if ((*write_sem = sem_open(WRITE_SEM, mode, 0660, 1)) == SEM_FAILED)
    {
        if ((sem_unlink(WRITE_SEM)) == -1)
            return (1);
        if ((*write_sem = sem_open(WRITE_SEM, mode, 0660, 1)) == SEM_FAILED)
            return (1);
    }
    return (0);
}

void    *ft_wait(void *arg)
{
    t_philo_status      *p_status;
    int                 stat;

    p_status = (t_philo_status*)arg;
    waitpid(p_status->pid, &stat, 0);
    if (WEXITSTATUS(stat) == EAT_ENOUGH)
    {
        p_status->infos->nb_philo_finished = p_status->infos->nb_philo_finished + 1;
        if (p_status->infos->nb_philo_finished == p_status->infos->nb_philo)
        {
            p_status->infos->end = 1;
            p_status->ender = 1;
        }
        else
        {
            sem_post(p_status->write_sem);
            p_status->eat_ok = 1;
            p_status->added = 1;
            p_status->eat_count = p_status->infos->nb_time_eat;
            p_status->eaten_time = ft_get_current_time();
            if ((p_status->pid = fork()) == -1)
            {
                p_status->infos->end = 1;
                p_status->ender = 1;
            }
            if (p_status->pid == 0)
            {
                ft_run_philo_process(p_status);
            }
        }
    }
    else if (WEXITSTATUS(stat) == PHILO_DIED || WEXITSTATUS(stat) == 1)
    {
        p_status->infos->end = 1;
        p_status->ender = 1;
    }
    return p_status;
}

int     main(int argc, char *argv[])
{
    t_philo_infos   p_infos;
    t_philo_status  *p_status;
    int             i;
    sem_t           *forks_sem;
    sem_t           *write_sem;

    if ((ft_check_args(argc, argv, &p_infos)) == 1)
        return (1);
    if (!(p_status = malloc(sizeof(*p_status) * p_infos.nb_philo)))
        return (1);
    if ((ft_open_sem(&forks_sem, &write_sem, O_CREAT | O_EXCL)) == 1)
        return (1);
    i = 0;
    while (i < p_infos.nb_philo)
    {
        p_status[i].philo_num = i;
        p_status[i].eat_count = 0;
        p_status[i].eaten_time = p_infos.starting_time;
        p_status[i].eat_ok = 0;
        p_status[i].died = 0;
        p_status[i].ender = 0;
        p_status[i].added = 0;
        p_status[i].infos = &p_infos;
        p_status[i].forks_sem = forks_sem;
        p_status[i].write_sem = write_sem;
        sem_post(forks_sem);
        i++;
    }
    i = 0;
    while (i < p_infos.nb_philo)
    {
        if ((p_status[i].pid = fork()) == -1)
            return (1);
        if (p_status[i].pid == 0)
            ft_run_philo_process(&(p_status[i]));
        i++;
    }
    i = 0;
    while (i < p_infos.nb_philo)
    {
        pthread_create(&(p_status[i].wait_thread), NULL, ft_wait, &(p_status[i]));
        i++;
    }
    while (1)
    {
        if (p_infos.end == 1)
        {
            usleep(1000 * 50);
            i = 0;
            while (i < p_infos.nb_philo)
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
