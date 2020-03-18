#ifndef PHILO_TWO_H
# define PHILO_TWO_H

#include <pthread.h>
#include <semaphore.h>
#include "ft_philo_utils.h"

# define FORKS_SEM "/forks_sem"
# define WRITE_SEM "/write_sem"

typedef struct  s_philo_status
{
    int             philo_num;
    int             eat_count;
    long            eaten_time;
    int             eat_ok;
    t_philo_infos   *infos;
    pthread_t       thread;
    sem_t           *forks_sem;
    sem_t           *write_sem;
}               t_philo_status;

#endif
