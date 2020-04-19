/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_three.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lhuang <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/19 23:36:15 by lhuang            #+#    #+#             */
/*   Updated: 2020/04/19 23:37:38 by lhuang           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_THREE_H
# define PHILO_THREE_H

# include <semaphore.h>
# include <pthread.h>
# include "ft_philo_utils.h"

# define EAT_ENOUGH 3
# define PHILO_DIED 4
# define FORKS_SEM "/forks_sem"
# define WRITE_SEM "/write_sem"

typedef struct		s_philo_status
{
	int				philo_num;
	int				eat_count;
	long			eaten_time;
	int				eat_ok;
	int				added;
	int				died;
	int				ender;
	t_philo_infos	*infos;
	sem_t			*forks_sem;
	sem_t			*write_sem;
	pid_t			pid;
	pthread_t		wait_thread;
}					t_philo_status;

#endif
