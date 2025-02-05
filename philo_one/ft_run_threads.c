/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_run_threads.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lhuang <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/19 21:08:38 by lhuang            #+#    #+#             */
/*   Updated: 2020/05/21 10:56:28 by lhuang           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include "ft_run_threads.h"
#include "philo_one.h"
#include "ft_write_state.h"
#include "ft_free.h"

static int	ft_philo_eats(t_philo_status *p_status,
	pthread_mutex_t *fork_one, pthread_mutex_t *fork_two)
{
	if ((p_status->eaten_time = ft_get_current_time()) == -1)
		return (ft_unlock_mutex(fork_one, fork_two, -1));
	p_status->eat_count = p_status->eat_count + 1;
	if (p_status->infos->nb_time_eat > 0 && !p_status->eat_ok &&
		p_status->eat_count == p_status->infos->nb_time_eat)
	{
		p_status->eat_ok = 1;
	}
	if ((ft_write_state(p_status, " is eating\n", 0)) == -1)
		return (ft_unlock_mutex(fork_one, fork_two, -1));
	if ((usleep(1000 * p_status->infos->time_te)) == -1)
		return (ft_unlock_mutex(fork_one, fork_two, -1));
	if ((pthread_mutex_unlock(fork_one)))
		return (ft_unlock_mutex(fork_two, NULL, -1));
	if ((pthread_mutex_unlock(fork_two)))
		return (-1);
	if (p_status->eat_ok)
	{
		p_status->infos->nb_philo_finished =
			p_status->infos->nb_philo_finished + 1;
		return (-1);
	}
	return (0);
}

static int	ft_do_all_state(t_philo_status *p_status,
	pthread_mutex_t *fork_one, pthread_mutex_t *fork_two)
{
	if ((pthread_mutex_lock(fork_one)))
		return (-1);
	if (p_status->infos->nb_philo == 1)
	{
		ft_write_state(p_status, " has taken a fork\n", 0);
		return (ft_unlock_mutex(fork_one, NULL, -1));
	}
	if ((pthread_mutex_lock(fork_two)))
		return (ft_unlock_mutex(fork_one, NULL, -1));
	if ((ft_write_state(p_status, " has taken a fork\n", 0)) == -1)
		return (ft_unlock_mutex(fork_one, fork_two, -1));
	if ((ft_write_state(p_status, " has taken a fork\n", 0)) == -1)
		return (ft_unlock_mutex(fork_one, fork_two, -1));
	if ((ft_philo_eats(p_status, fork_one, fork_two)) == -1)
		return (-1);
	if ((ft_write_state(p_status, " is sleeping\n", 0)) == -1)
		return (-1);
	if ((usleep(1000 * p_status->infos->time_ts)) == -1)
		return (-1);
	if ((ft_write_state(p_status, " is thinking\n", 0)) == -1)
		return (-1);
	if (p_status->infos->nb_philo % 2 == 1)
		usleep(1000);
	return (0);
}

void		*ft_philo_thread(void *arg)
{
	t_philo_status	*p_status;
	int				l_fork_num;
	int				r_fork_num;
	int				pick_one;
	int				pick_two;

	p_status = (t_philo_status*)arg;
	l_fork_num = p_status->philo_num == 0 ?
		p_status->infos->nb_philo - 1 : p_status->philo_num - 1;
	r_fork_num = p_status->philo_num;
	pick_one = l_fork_num;
	pick_two = r_fork_num;
	if (p_status->infos->nb_philo % 2 == 0)
	{
		pick_one = p_status->philo_num % 2 == 0 ? l_fork_num : r_fork_num;
		pick_two = p_status->philo_num % 2 == 0 ? r_fork_num : l_fork_num;
	}
	while (!(p_status->infos->end))
	{
		if ((ft_do_all_state(p_status, &(p_status->m_forks[pick_one]),
				&(p_status->m_forks[pick_two]))) == -1)
			return (p_status);
	}
	return (p_status);
}

static int	ft_check_if_end(t_philo_infos *p_infos,
	t_philo_status *p_status_all, int i)
{
	long	current_time;
	char	to_write[1024];
	int		lenght;

	if (p_infos->nb_philo_finished == p_infos->nb_philo)
	{
		p_infos->end = 1;
		return (1);
	}
	else
	{
		if ((current_time = ft_get_current_time()) == -1)
			return (1);
		if (!(p_status_all[i].eat_ok) &&
			current_time - p_status_all[i].eaten_time > p_infos->time_td + 5)
		{
			p_infos->end = 1;
			if ((lenght = ft_get_final_to_write(p_infos,
					to_write, " died\n", i)) != -1)
				write(1, to_write, lenght);
			return (1);
		}
	}
	return (0);
}

void		*ft_end_thread(void *arg)
{
	t_philo_status	*p_status_all;
	t_philo_infos	*p_infos;
	int				i;

	p_status_all = (t_philo_status*)arg;
	p_infos = p_status_all[0].infos;
	while (1)
	{
		i = 0;
		while (i < p_infos->nb_philo)
		{
			if ((ft_check_if_end(p_infos, p_status_all, i)) == 1)
				return (p_status_all);
			i++;
		}
	}
	return (p_infos);
}
