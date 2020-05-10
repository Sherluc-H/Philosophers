/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_run_process.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lhuang <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/19 23:42:16 by lhuang            #+#    #+#             */
/*   Updated: 2020/05/10 18:22:01 by lhuang           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include "ft_run_process.h"
#include "ft_write_state.h"
#include "ft_free.h"

static void	ft_check_if_died(t_philo_status *p_status)
{
	long			current_time;

	while (1)
	{
		if (p_status->added == 0 && p_status->eat_ok == 1 &&
				p_status->eat_count == p_status->infos->nb_time_eat)
		{
			p_status->infos->end = 1;
			exit(EAT_ENOUGH);
		}
		else
		{
			if ((current_time = ft_get_current_time()) == -1)
				exit(1);
			if (current_time - p_status->eaten_time >
				p_status->infos->time_td + 5)
			{
				ft_announce_death(p_status);
				exit(PHILO_DIED);
			}
		}
	}
}

static int	ft_eat_ok_begin(t_philo_status *p_status)
{
	if ((usleep(1000 * p_status->infos->time_te)) == -1)
		return (-1);
	if ((sem_post(p_status->forks_sem)) == -1)
		return (-1);
	sem_post(p_status->forks_sem);
	if ((ft_write_state(p_status, " is sleeping\n",
			p_status->write_sem, 0)) == -1)
		return (-1);
	if ((usleep(1000 * p_status->infos->time_ts)) == -1)
		return (-1);
	return (0);
}

static int	ft_take_forks_n_eat(t_philo_status *p_status)
{
	if ((sem_wait(p_status->forks_sem)))
		return (-1);
	if ((ft_write_state(p_status, " has taken a fork\n",
			p_status->write_sem, 0)) == -1)
		return (ft_unlock_sem(p_status->forks_sem, NULL, -1));
	if ((sem_wait(p_status->forks_sem)))
		return (ft_unlock_sem(p_status->forks_sem, NULL, -1));
	if ((ft_write_state(p_status, " has taken a fork\n",
			p_status->write_sem, 0)) == -1)
		return (ft_unlock_sem(p_status->forks_sem, p_status->forks_sem, -1));
	if ((p_status->eaten_time = ft_get_current_time()) == -1)
		return (ft_unlock_sem(p_status->forks_sem, p_status->forks_sem, -1));
	if ((ft_write_state(p_status, " is eating\n",
			p_status->write_sem, 1)) == -1)
		return (ft_unlock_sem(p_status->forks_sem, p_status->forks_sem, -1));
	if ((usleep(1000 * p_status->infos->time_te)) == -1)
		return (ft_unlock_sem(p_status->forks_sem, p_status->forks_sem, -1));
	if ((sem_post(p_status->forks_sem)) == -1)
		return (-1);
	sem_post(p_status->forks_sem);
	return (0);
}

static void	*ft_philo_thread(void *arg)
{
	t_philo_status	*p_status;

	p_status = (t_philo_status*)arg;
	if (p_status->eat_ok)
	{
		if ((ft_eat_ok_begin(p_status)) == -1)
			return (p_status);
	}
	while (!(p_status->died))
	{
		if ((ft_write_state(p_status, " is thinking\n",
				p_status->write_sem, 0)) == -1)
			return (p_status);
		if ((ft_take_forks_n_eat(p_status)) == -1)
			return (p_status);
		if ((ft_write_state(p_status, " is sleeping\n",
				p_status->write_sem, 0)) == -1)
			return (p_status);
		if ((usleep(1000 * p_status->infos->time_ts)) == -1)
			return (p_status);
	}
	return (p_status);
}

void		ft_run_philo_process(t_philo_status *p_status)
{
	pthread_t	philo_thread;

	if ((p_status->forks_sem = sem_open(FORKS_SEM, 0)) == SEM_FAILED)
		exit(1);
	if ((p_status->write_sem = sem_open(WRITE_SEM, 0)) == SEM_FAILED)
		exit(1);
	if ((pthread_create(&(philo_thread), NULL, ft_philo_thread, p_status)))
		exit(1);
	if ((pthread_detach(philo_thread)))
		exit(1);
	ft_check_if_died(p_status);
}
