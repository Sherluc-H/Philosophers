#ifndef FT_WAIT_H
# define FT_WAIT_H

# include "philo_three.h"

int         ft_wait_end(t_philo_infos *p_infos, t_philo_status *p_status, sem_t *forks_sem, sem_t *write_sem);
int         ft_create_wait_threads(t_philo_infos *p_infos, t_philo_status *p_status);

#endif
