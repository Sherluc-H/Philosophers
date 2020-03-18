#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "philo_one.h"
#include "ft_philo_utils.h"
#include "ft_run_threads.h"
#include "ft_free.h"

static int  ft_check_args(int argc, char **argv, t_philo_infos *p_infos)
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

static int  ft_prepare_infos(t_philo_infos *p_infos, t_philo_status *p_status,
    pthread_mutex_t *m_forks, pthread_mutex_t *m_write)
{
    int i;
    int ret;

    i = 0;
    ret = 0;
    if ((ret = pthread_mutex_init(m_write, NULL)) != 0)
        return (ret);
    while (i < p_infos->nb_philo)
    {
        p_status[i].philo_num = i;
        p_status[i].eat_count = 0;
        p_status[i].eaten_time = p_infos->starting_time;
        p_status[i].eat_ok = 0;
        p_status[i].infos = p_infos;
        p_status[i].m_forks = m_forks;
        p_status[i].m_write = m_write;
        if ((ret = pthread_mutex_init(&(p_status[i].m_forks[i]), NULL)) != 0)
            return (ft_free_mutex(m_forks, m_write, i, ret));
        i++;
    }
    return (0);
}

static int ft_create_threads(t_philo_infos *p_infos, t_philo_status *p_status)
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
    return (0);
}

int         main(int argc, char **argv)
{
    t_philo_infos   p_infos;
    t_philo_status  *p_status;
    pthread_mutex_t *m_forks;
    pthread_mutex_t m_write;
    int             ret;

    ret = 0;
    if (ft_check_args(argc, argv, &p_infos) == 1)
        return (1);
    if (!(p_status = malloc(sizeof(*p_status) * p_infos.nb_philo)))
        return (1);
    if (!(m_forks = malloc(sizeof(*m_forks) * p_infos.nb_philo)))
    {
        free(p_status);
        return (1);
    }
    if ((ret = ft_prepare_infos(&p_infos, p_status, m_forks, &m_write)) != 0)
    {
        free(p_status);
        free(m_forks);
        return (ret);
    }
    ret = ft_create_threads(&p_infos, p_status);
    return (ft_free_all(p_status, m_forks, &m_write, ret));
}
