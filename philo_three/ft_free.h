#ifndef FT_FREE_H
# define FT_FREE_H

# include "philo_three.h"

int     ft_unlock_sem(sem_t *sem_one, sem_t *sem_two, int ret);
int     ft_unlink_n_close_sem(sem_t *forks_sem, sem_t *write_sem, int ret);

#endif
