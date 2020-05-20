/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_wait.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lhuang <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/20 00:04:09 by lhuang            #+#    #+#             */
/*   Updated: 2020/05/20 15:45:53 by lhuang           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "ft_wait.h"
#include "ft_run_process.h"

static void	*ft_wait(void *arg)
{
	t_philo_status		*p_status;
	int					stat;

	p_status = (t_philo_status*)arg;
	waitpid(p_status->pid, &stat, 0);
	if (WEXITSTATUS(stat) == EAT_ENOUGH)
	{
		p_status->infos->nb_philo_finished =
			p_status->infos->nb_philo_finished + 1;
		if (p_status->infos->nb_philo == p_status->infos->nb_philo_finished)
			p_status->infos->end = 1;
	}
	else if (WEXITSTATUS(stat) == PHILO_DIED || WEXITSTATUS(stat) == 1)
	{
		p_status->infos->end = 1;
		p_status->ender = 1;
	}
	return (p_status);
}

int			ft_wait_end(t_philo_infos *p_infos, t_philo_status *p_status)
{
	int i;

	while (1)
	{
		if (p_infos->end == 1)
		{
			usleep(1000 * 50);
			i = 0;
			while (i < p_infos->nb_philo)
			{
				if (!(p_status[i].ender))
					kill(p_status[i].pid, SIGKILL);
				i++;
			}
			return (0);
		}
	}
	return (0);
}

int			ft_create_wait_threads(t_philo_infos *p_infos,
	t_philo_status *p_status)
{
	int i;
	int ret;

	i = 0;
	ret = 0;
	while (i < p_infos->nb_philo)
	{
		if ((ret = pthread_create(&(p_status[i].wait_thread), NULL,
				ft_wait, &(p_status[i]))))
			return (ret);
		i++;
	}
	ft_wait_end(p_infos, p_status);
	i = 0;
	while (i < p_infos->nb_philo)
	{
		if ((ret = pthread_join(p_status[i].wait_thread, NULL)))
			return (ret);
		i++;
	}
	return (ret);
}
