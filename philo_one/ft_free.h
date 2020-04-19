/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_free.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lhuang <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/19 21:04:22 by lhuang            #+#    #+#             */
/*   Updated: 2020/04/19 21:06:58 by lhuang           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_FREE_H
# define FT_FREE_H

# include <pthread.h>
# include "philo_one.h"

int	ft_unlock_mutex(pthread_mutex_t *mutex_one,
	pthread_mutex_t *mutex_two, int ret);
int	ft_free_mutex(pthread_mutex_t *m_forks, pthread_mutex_t *m_write,
	int nb_init, int ret);
int	ft_free_all(t_philo_status *p_status, pthread_mutex_t *m_forks,
	pthread_mutex_t *m_write, int ret);

#endif
