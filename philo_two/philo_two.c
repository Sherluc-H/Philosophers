#include <stdlib.h>
#include <unistd.h>
#include "philo_two.h"
#include "ft_philo_utils.h"

int     ft_get_final_to_write(t_philo_infos *p_infos, char *to_write, char *end_str, int philo_num)
{
    int     i;
    long    current_time;

    i = 0;
    if ((current_time = ft_get_current_time()) == -1)
        return (-1);
    ft_add_nbr_to_str(current_time - p_infos->starting_time, to_write, &i);
    ft_add_str_to_str(" ", to_write, &i);
    ft_add_nbr_to_str(philo_num + 1, to_write, &i);
    ft_add_str_to_str(end_str, to_write, &i);
    return (i);
}

int     ft_write_state(t_philo_status *p_status, char *end_str, int lenght)
{
    char    to_write[1024];

    if (p_status->infos->end)
        return (-1);
    if ((sem_wait(p_status->write_sem)) == -1)
        return (-1);
    if ((lenght = ft_get_final_to_write(p_status->infos, to_write,
        end_str, p_status->philo_num)) == -1)
    {
        sem_post(p_status->write_sem);
        return (-1);
    }
    if (!(p_status->infos->end))
    {
        if ((write(1, to_write, lenght)) == -1)
        {
            sem_post(p_status->write_sem);
            return (-1);
        }
    }
    if ((sem_post(p_status->write_sem)) == -1)
        return (-1);
    if (p_status->infos->end)
        return (-1);
    return (0);
}

void    *ft_philo_thread(void *arg)
{
    t_philo_status  *p_status;
    t_philo_infos   *p_infos;

    p_status = (t_philo_status*)arg;
    p_infos = p_status->infos;
    while (!(p_infos->end))
    {
        if ((ft_write_state(p_status, " is thinking\n", 0)) == -1)
            return p_status;
        sem_wait(p_status->forks_sem);
        if ((ft_write_state(p_status, " has taken a fork\n", 0)) == -1)
            return p_status;
        sem_wait(p_status->forks_sem);
        if ((ft_write_state(p_status, " has taken a fork\n", 0)) == -1)
            return p_status;
        if ((p_status->eaten_time = ft_get_current_time()) == -1)
            return p_status;
        if ((ft_write_state(p_status, " is eating\n", 0)) == -1)
            return p_status;
        p_status->eat_count = p_status->eat_count + 1;
        if (p_status->infos->nb_time_eat > 0 && !(p_status->eat_ok) &&
            p_status->eat_count == p_status->infos->nb_time_eat)
        {
            p_status->eat_ok = 1;
            p_status->infos->nb_philo_finished = p_status->infos->nb_philo_finished + 1;
        }
        usleep(1000 * p_status->infos->time_te);
        sem_post(p_status->forks_sem);
        sem_post(p_status->forks_sem);
        if ((ft_write_state(p_status, " is sleeping\n", 0)) == -1)
            return p_status;
        usleep(1000 * p_status->infos->time_ts);
    }
    return p_status;
}

void    *ft_end_thread(void *arg)
{
    t_philo_status  *p_status_all;
    t_philo_infos   *p_infos;
    int             i;
    long            current_time;
    char            to_write[1024];
    int             lenght;

    lenght = 0;
    p_status_all = (t_philo_status*)arg;
    p_infos = p_status_all[0].infos;
    while (1)
    {
        i = 0;
        while (i < p_infos->nb_philo)
        {
            if (p_infos->nb_philo_finished == p_infos->nb_philo)
            {
                p_infos->end = 1;
                return p_status_all;
            }
            if ((current_time = ft_get_current_time()) == -1)
                return p_status_all;
            if (current_time - p_status_all[i].eaten_time > p_infos->time_td)
            {
                p_infos->end = 1;
                if ((lenght = ft_get_final_to_write(p_infos, to_write, " died\n", i)) != -1)
                    write(1, to_write, lenght);
                return p_status_all;
            }
            i++;
        }
    }
    return p_status_all;
}

static int  ft_check_args(int argc, char *argv[], t_philo_infos *p_infos)
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

static int  ft_open_sem(sem_t **forks_sem, sem_t **write_sem)
{
    if ((*forks_sem = sem_open(FORKS_SEM, O_CREAT | O_EXCL,
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, 0)) == SEM_FAILED)
    {
        if ((sem_unlink(FORKS_SEM)) == -1)
            return (1);
        if ((*forks_sem = sem_open(FORKS_SEM, O_CREAT | O_EXCL,
            S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, 0)) == SEM_FAILED)
            return (1);
    }
    if ((*write_sem = sem_open(WRITE_SEM, O_CREAT | O_EXCL,
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, 1)) == SEM_FAILED)
    {
        if ((sem_unlink(WRITE_SEM)) == -1)
            return (1);
        if ((*write_sem = sem_open(WRITE_SEM, O_CREAT | O_EXCL,
            S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, 1)) == SEM_FAILED)
            return (1);
    }
    return (0);
}

static int  ft_prepare_infos(t_philo_infos *p_infos, t_philo_status *p_status,
    sem_t *forks_sem, sem_t *write_sem)
{
    int i;

    i = 0;
    while (i < p_infos->nb_philo)
    {
        if ((sem_post(forks_sem)) == -1)
            return (1);
        p_status[i].philo_num = i;
        p_status[i].eat_count = 0;
        p_status[i].eaten_time = p_infos->starting_time;
        p_status[i].eat_ok = 0;
        p_status[i].infos = p_infos;
        p_status[i].forks_sem = forks_sem;
        p_status[i].write_sem = write_sem;
        i++;
    }
    return (0);
}

static int  ft_create_threads(t_philo_infos *p_infos, t_philo_status *p_status)
{
    pthread_t   end_thread;
    int         i;
    int         ret;

    i = 0;
    ret = 0;
    while (i < p_infos->nb_philo)
    {
        if ((ret = pthread_create(&(p_status[i].thread), NULL, ft_philo_thread, &(p_status[i]))) != 0)
            return (ret);
        usleep(50);
        i++;
    }
    if ((ret = pthread_create(&end_thread, NULL, ft_end_thread, p_status)) != 0)
        return (ret);
    if ((ret = pthread_detach(end_thread)) != 0)
        return (ret);
    i = 0;
    while (i < p_infos->nb_philo)
    {
        if ((ret = pthread_join(p_status[i].thread, NULL)) != 0)
            return (ret);
        i++;
    }
    return (ret);
}

int     main(int argc, char *argv[])
{
    t_philo_infos   p_infos;
    t_philo_status  *p_status;
    sem_t           *forks_sem;
    sem_t           *write_sem;
    int             ret;

    if ((ft_check_args(argc, argv, &p_infos)) == 1)
        return (1);
    if ((ft_open_sem(&forks_sem, &write_sem)) == 1)
        return (1);
    if (!(p_status = malloc(sizeof(*p_status) * p_infos.nb_philo)))
        return (1);
    if ((ft_prepare_infos(&p_infos, p_status, forks_sem, write_sem)) == 1)
    {
        free(p_status);
        return (1);
    }
    ret = ft_create_threads(&p_infos, p_status);
    free(p_status);
    sem_unlink(FORKS_SEM);
    sem_unlink(WRITE_SEM);
    sem_close(forks_sem);
    sem_close(write_sem);
    return (ret);
}