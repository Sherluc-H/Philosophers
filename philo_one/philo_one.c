#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "philo_one.h"
#include "ft_philo_utils.h"

static int  ft_get_final_to_write(t_philo_infos *p_infos, char *to_write, char *str_end, int philo_num)
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

static int  ft_write_state(t_philo_status *p_status, char *str_end, int lenght)
{
    char            to_write[1024];

    if (p_status->infos->end)
        return (-1);
    if ((pthread_mutex_lock(p_status->m_write)))
        return (-1);
    if ((lenght = ft_get_final_to_write(p_status->infos,
            to_write, str_end, p_status->philo_num)) == -1)
    {
        pthread_mutex_unlock(p_status->m_write);
        return (-1);
    }
    if (!(p_status->infos->end))
    {
        if ((write(1, to_write, lenght)) == -1)
        {
            pthread_mutex_unlock(p_status->m_write);
            return (-1);
        }
    }
    if ((pthread_mutex_unlock(p_status->m_write)))
        return (-1);
    if (p_status->infos->end)
        return (-1);
    return (0);
}

static int  ft_philo_eats(t_philo_status *p_status, int pick_one, int pick_two)
{
    if ((p_status->eaten_time = ft_get_current_time()) == -1)
        return (-1);
    if ((ft_write_state(p_status, " is eating\n", 0)) == -1)
    {
        pthread_mutex_unlock(&(p_status->m_forks[pick_one]));
        pthread_mutex_unlock(&(p_status->m_forks[pick_two]));
        return (-1);
    }
    p_status->eat_count = p_status->eat_count + 1;
    if (p_status->infos->nb_time_eat > 0 && !p_status->eat_ok &&
        p_status->eat_count == p_status->infos->nb_time_eat)
    {
        p_status->eat_ok = 1;
        p_status->infos->nb_philo_finished = p_status->infos->nb_philo_finished + 1;
    }
    if ((usleep(1000 * p_status->infos->time_te)) == -1)
        return (-1);
    if ((pthread_mutex_unlock(&(p_status->m_forks[pick_one]))))
    {
        pthread_mutex_unlock(&(p_status->m_forks[pick_two]));
        return (-1);
    }
    if ((pthread_mutex_unlock(&(p_status->m_forks[pick_two]))))
        return (-1);
    return (0);
}

static int  ft_take_forks_n_eat(t_philo_status *p_status, int pick_one, int pick_two)
{
    if ((pthread_mutex_lock(&(p_status->m_forks[pick_one]))))
        return (-1);
    if ((ft_write_state(p_status, " has taken a fork\n", 0)) == -1)
    {
        pthread_mutex_unlock(&(p_status->m_forks[pick_one]));
        return (-1);
    }
    if ((pthread_mutex_lock(&(p_status->m_forks[pick_two]))))
    {
        pthread_mutex_unlock(&(p_status->m_forks[pick_one]));
        return (-1);
    }
    if ((ft_write_state(p_status, " has taken a fork\n", 0)) == -1)
    {
        pthread_mutex_unlock(&(p_status->m_forks[pick_one]));
        pthread_mutex_unlock(&(p_status->m_forks[pick_two]));
        return (-1);
    }
    if ((ft_philo_eats(p_status, pick_one, pick_two)) == -1)
        return (-1);
    return (0);
}

static void *ft_philo_thread(void *arg)
{
    t_philo_status  *p_status;
    int             l_fork_num;
    int             r_fork_num;
    int             pick_one;
    int             pick_two;

    p_status = (t_philo_status*)arg;
    l_fork_num = p_status->philo_num == 0 ?
        p_status->infos->nb_philo - 1 : p_status->philo_num - 1;
    r_fork_num = p_status->philo_num;
    pick_one = p_status->philo_num % 2 == 0 ? l_fork_num : r_fork_num;
    pick_two = p_status->philo_num % 2 == 0 ? r_fork_num : l_fork_num;
    while (!(p_status->infos->end))
    {
        if ((ft_write_state(p_status, " is thinking\n", 0)) == -1)
            return p_status;
        if ((ft_take_forks_n_eat(p_status, pick_one, pick_two)) == -1)
            return p_status;
        if ((ft_write_state(p_status, " is sleeping\n", 0)) == -1)
            return p_status;
        if ((usleep(1000 * p_status->infos->time_ts)) == -1)
            return p_status;
    }
    return p_status;
}

static int  ft_check_if_end(t_philo_infos *p_infos, t_philo_status *p_status_all, int i)
{
    long    current_time;
    char    to_write[1024];
    int     lenght;

    if (p_infos->nb_philo_finished == p_infos->nb_philo)
    {
        p_infos->end = 1;
        return 1;
    }
    else
    {
        if ((current_time = ft_get_current_time()) == -1)
            return 1;
        if (current_time - p_status_all[i].eaten_time > p_infos->time_td + 5)
        {
            p_infos->end = 1;
            if ((lenght = ft_get_final_to_write(p_infos, to_write, " died\n", i)) != -1)
                write(1, to_write, lenght);
            return 1;
        }
    }
    return (0);
}

static void *ft_end_thread(void *arg)
{
    t_philo_status  *p_status_all;
    t_philo_infos   *p_infos;
    int             i;

    p_status_all = (t_philo_status*)arg;
    p_infos = p_status_all[0].infos;
    while (1)
    {
        i = 0;
        while (i < p_infos->nb_philo)
        {
            if ((ft_check_if_end(p_infos, p_status_all, i)) == 1)
                return p_status_all;
            i++;
        }
    }
    return p_infos;
}

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
            return (ret);
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

static int  ft_free_all(int nb_philo, t_philo_status *p_status,
    pthread_mutex_t *m_forks, pthread_mutex_t *m_write)
{
    int i;
    int ret;

    i = 0;
    ret = 0;
    if ((ret = pthread_mutex_destroy(m_write)))
    {
        free(p_status);
        free(m_forks);
        return (ret);
    }
    while (i < nb_philo)
    {
        if ((ret = pthread_mutex_destroy(&(m_forks[i]))))
        {
            free(p_status);
            free(m_forks);
            return (ret);
        }
        i++;
    }
    free(p_status);
    free(m_forks);
    return (0);
}

int main(int argc, char **argv)
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
        return (1);
    if ((ret = ft_prepare_infos(&p_infos, p_status, m_forks, &m_write)) != 0)
    {
        free(p_status);
        free(m_forks);
        return (ret);
    }
    ret = ft_create_threads(&p_infos, p_status);
    return (ft_free_all(p_infos.nb_philo, p_status, m_forks, &m_write));
}