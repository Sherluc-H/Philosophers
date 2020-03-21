#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>
#include "philo_three.h"
#include "ft_philo_utils.h"

int     ft_unlock_sem(sem_t *sem_one, sem_t *sem_two, int ret)
{
    if (sem_one)
    {
        sem_post(sem_one);
    }
    if (sem_two)
    {
        sem_post(sem_two);
    }
    return (ret);
}

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

int     ft_dont_unlock_if_eat_ok(t_philo_status *p_status, int is_eating)
{
    if (is_eating)
    {
        p_status->eat_count = p_status->eat_count + 1;
        if (p_status->infos->nb_time_eat > 0 && !(p_status->eat_ok) &&
            p_status->eat_count == p_status->infos->nb_time_eat)
        {
            p_status->eat_ok = 1;
            return (-1);
        }
    }
    return (0);
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
        return (ft_unlock_sem(write_sem, NULL, -1));
    if ((lenght = ft_get_final_to_write(p_status->infos,
            to_write, str_end, p_status->philo_num)) == -1)
        return (ft_unlock_sem(write_sem, NULL, -1));
    if (!(p_status->died))
        if ((write(1, to_write, lenght)) == -1)
            return (ft_unlock_sem(write_sem, NULL, -1));
    if (ft_dont_unlock_if_eat_ok(p_status, is_eating) == -1)
        return (-1);
    if (p_status->died)
        return (ft_unlock_sem(write_sem, NULL, -1));
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

static int  ft_announce_death(t_philo_status *p_status)
{
    char    to_write[1024];
    int     lenght;

    p_status->died = 1;
    p_status->infos->end = 1;
    if ((sem_wait(p_status->write_sem)) == -1)
        return (-1);
    if ((lenght = ft_get_final_to_write(p_status->infos,
            to_write, " died\n", p_status->philo_num)) != -1)
        write(1, to_write, lenght);
    return (0);
}

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

void    ft_run_philo_process(t_philo_status *p_status)
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

void    *ft_wait(void *arg)
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

static int  ft_prepare_infos(t_philo_infos *p_infos, t_philo_status *p_status, sem_t *forks_sem, sem_t *write_sem)
{
    int i;

    i = 0;
    while (i < p_infos->nb_philo)
    {
        p_status[i].philo_num = i;
        p_status[i].eat_count = 0;
        p_status[i].eaten_time = p_infos->starting_time;
        p_status[i].eat_ok = 0;
        p_status[i].died = 0;
        p_status[i].ender = 0;
        p_status[i].added = 0;
        p_status[i].infos = p_infos;
        p_status[i].forks_sem = forks_sem;
        p_status[i].write_sem = write_sem;
        if ((sem_post(forks_sem)) == -1)
            return (-1);
        i++;
    }
    return (0);
}

static int  ft_wait_end(t_philo_infos *p_infos, t_philo_status *p_status, sem_t *forks_sem, sem_t *write_sem)
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

static int  ft_create_wait_threads(t_philo_infos *p_infos, t_philo_status *p_status)
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

static int  ft_create_processes_n_wait_threads(t_philo_infos *p_infos, t_philo_status *p_status)
{
    int i;
    int j;

    i = 0;
    while (i < p_infos->nb_philo)
    {
        if ((p_status[i].pid = fork()) == -1)
        {
            j = 0;
            while (j < i)
            {
                kill(p_status[j].pid, SIGKILL);
                j++;
            }
            return (-1);
        }
        if (p_status[i].pid == 0)
            ft_run_philo_process(&(p_status[i]));
        i++;
    }
    return (ft_create_wait_threads(p_infos, p_status));
}

int     ft_unlink_n_close_sem(sem_t *forks_sem, sem_t *write_sem, int ret)
{
    sem_unlink(FORKS_SEM);
    sem_unlink(WRITE_SEM);
    sem_close(forks_sem);
    sem_close(write_sem);
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
    if ((ft_open_sem(&forks_sem, &write_sem, O_CREAT | O_EXCL)) == 1)
        return (1);
    if (!(p_status = malloc(sizeof(*p_status) * p_infos.nb_philo)))
        return (ft_unlink_n_close_sem(forks_sem, write_sem, 1));
    if ((ft_prepare_infos(&p_infos, p_status, forks_sem, write_sem)) == -1)
    {
        free(p_status);
        return (ft_unlink_n_close_sem(forks_sem, write_sem, -1));
    }
    if ((ret = ft_create_processes_n_wait_threads(&p_infos, p_status)) != 0)
    {
        free(p_status);
        return (ft_unlink_n_close_sem(forks_sem, write_sem, ret));
    }
    return (ft_wait_end(&p_infos, p_status, forks_sem, write_sem));
}
