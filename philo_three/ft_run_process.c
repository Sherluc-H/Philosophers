/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_run_process.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lhuang <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/19 23:42:16 by lhuang            #+#    #+#             */
/*   Updated: 2020/05/20 16:54:46 by lhuang           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include "ft_run_process.h"
#include "ft_write_state.h"
#include "ft_free.h"

static void	ft_check_if_died(t_philo_status *p_status,
	t_philo_status *p_status_all)
{
	long			current_time;

	while (1)
	{
		if (p_status->added == 0 && p_status->eat_ok == 1 &&
				p_status->eat_count == p_status->infos->nb_time_eat)
		{
			p_status->infos->end = 1;
			ft_unlink_n_close_sem(p_status->forks_sem, p_status->write_sem,
				p_status->take_sem, 0);
			free(p_status_all);
			exit(EAT_ENOUGH);
		}
		if ((current_time = ft_get_current_time()) == -1)
			exit(1);
		if (current_time - p_status->eaten_time >
			p_status->infos->time_td)
		{
			ft_announce_death(p_status);
			ft_unlink_n_close_sem(p_status->forks_sem, p_status->write_sem,
				p_status->take_sem, 0);
			free(p_status_all);
			exit(PHILO_DIED);
		}
	}
}

static int	ft_philo_eats(t_philo_status *p_status)
{
	int ret;

	if ((p_status->eaten_time = ft_get_current_time()) == -1)
		return (ft_unlock_sem(p_status->forks_sem, p_status->forks_sem, -1));
	if ((ret = ft_write_state(p_status, " is eating\n",
			p_status->write_sem, 1)) == -1)
		return (ft_unlock_sem(p_status->forks_sem, p_status->forks_sem, -1));
	if (ret == -2)
		return (-1);
	if ((usleep(1000 * p_status->infos->time_te)) == -1)
		return (ft_unlock_sem(p_status->forks_sem, p_status->forks_sem, -1));
	if ((sem_post(p_status->forks_sem)) == -1)
		return (-1);
	sem_post(p_status->forks_sem);
	return (0);
}

static int	ft_take_forks_n_eat(t_philo_status *p_status)
{
	if ((sem_wait(p_status->take_sem)))
		return (-1);
	if ((sem_wait(p_status->forks_sem)))
		return (ft_unlock_sem(p_status->take_sem, NULL, -1));
	if (p_status->infos->nb_philo == 1)
	{
		ft_write_state(p_status, " has taken a fork\n", p_status->write_sem, 0);
		return (ft_unlock_sem(p_status->forks_sem, p_status->take_sem, -1));
	}
	if ((sem_wait(p_status->forks_sem)))
		return (ft_unlock_sem(p_status->forks_sem, p_status->take_sem, -1));
	if ((sem_post(p_status->take_sem)))
		return (ft_unlock_sem(p_status->forks_sem, p_status->forks_sem, -1));
	if ((ft_write_state(p_status, " has taken a fork\n",
			p_status->write_sem, 0)) == -1)
		return (ft_unlock_sem(p_status->forks_sem, p_status->forks_sem, -1));
	if ((ft_write_state(p_status, " has taken a fork\n",
			p_status->write_sem, 0)) == -1)
		return (ft_unlock_sem(p_status->forks_sem, p_status->forks_sem, -1));
	return (ft_philo_eats(p_status));
}

static void	*ft_philo_thread(void *arg)
{
	t_philo_status	*p_status;

	p_status = (t_philo_status*)arg;
	while (!(p_status->died))
	{
		if ((ft_take_forks_n_eat(p_status)) == -1)
			return (p_status);
		if ((ft_write_state(p_status, " is sleeping\n",
				p_status->write_sem, 0)) == -1)
			return (p_status);
		if ((usleep(1000 * p_status->infos->time_ts)) == -1)
			return (p_status);
		if ((ft_write_state(p_status, " is thinking\n",
				p_status->write_sem, 0)) == -1)
			return (p_status);
	}
	return (p_status);
}

void		ft_run_philo_process(t_philo_status *p_status,
	t_philo_status *p_status_all)
{
	pthread_t	philo_thread;

	if ((pthread_create(&(philo_thread), NULL, ft_philo_thread, p_status)))
		exit(1);
	if ((pthread_detach(philo_thread)))
		exit(1);
	ft_check_if_died(p_status, p_status_all);
}
