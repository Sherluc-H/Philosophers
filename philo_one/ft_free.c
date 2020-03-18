#include <stdlib.h>
#include "ft_free.h"

int  ft_free_mutex(pthread_mutex_t *m_forks, pthread_mutex_t *m_write,
    int nb_init, int ret)
{
    int i;

    i = 0;
    pthread_mutex_destroy(m_write);
    while (i < nb_init)
    {
        pthread_mutex_destroy(&(m_forks[i]));
        i++;
    }
    return (ret);
}

int  ft_free_all(t_philo_status *p_status, pthread_mutex_t *m_forks,
    pthread_mutex_t *m_write, int ret)
{
    int nb_philo;
    int i;
    int ret2;

    nb_philo = p_status[0].infos->nb_philo;
    i = 0;
    ret2 = 0;
    if ((ret2 = pthread_mutex_destroy(m_write)))
        ret = ret == 0 ? ret2 : ret;
    while (i < nb_philo)
    {
        if ((ret2 = pthread_mutex_destroy(&(m_forks[i]))))
            ret = ret == 0 ? ret2 : ret;
        i++;
    }
    free(p_status);
    free(m_forks);
    return (ret);
}
