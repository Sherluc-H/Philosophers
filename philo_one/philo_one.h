/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_one.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lhuang <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/19 20:40:24 by lhuang            #+#    #+#             */
/*   Updated: 2020/04/19 20:42:48 by lhuang           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_ONE_H
# define PHILO_ONE_H

# include <pthread.h>
# include "ft_philo_utils.h"

typedef struct	s_philo_status
{
	int				philo_num;
	int				eat_count;
	long			eaten_time;
	int				eat_ok;
	t_philo_infos	*infos;
	pthread_t		thread;
	pthread_mutex_t	*m_forks;
	pthread_mutex_t	*m_write;
}				t_philo_status;

#endif
