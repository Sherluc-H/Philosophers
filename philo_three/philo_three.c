#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "philo_three.h"
#include "ft_run_process.h"
#include "ft_wait.h"
#include "ft_free.h"

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
        {
            sem_unlink(FORKS_SEM);
            sem_close(*forks_sem);
            return (1);
        }
        if ((*write_sem = sem_open(WRITE_SEM, mode, 0660, 1)) == SEM_FAILED)
        {
            sem_unlink(FORKS_SEM);
            sem_close(*forks_sem);
            return (1);
        }
    }
    return (0);
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
