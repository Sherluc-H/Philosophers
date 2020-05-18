/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_write_state.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lhuang <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/19 23:07:41 by lhuang            #+#    #+#             */
/*   Updated: 2020/05/18 17:14:04 by lhuang           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include "ft_write_state.h"
#include "philo_two.h"

int	ft_get_final_to_write(t_philo_infos *p_infos, char *to_write,
	char *end_str, int philo_num)
{
	int		i;
	long	current_time;

	i = 0;
	if ((current_time = ft_get_current_time()) == -1)
		return (-1);
	ft_add_nbr_to_str(current_time - p_infos->starting_time, to_write, &i);
	ft_add_str_to_str(" ", to_write, &i);
	ft_add_nbr_to_str(philo_num + 1, to_write, &i);
	ft_add_str_to_str(end_str, to_write, &i);
	return (i);
}

int	ft_write_state(t_philo_status *p_status, char *end_str, int lenght)
{
	char	to_write[1024];

	if (p_status->infos->end)
		return (-1);
	if ((sem_wait(p_status->write_sem)) == -1)
		return (-1);
	if ((lenght = ft_get_final_to_write(p_status->infos, to_write,
		end_str, p_status->philo_num)) == -1)
	{
		sem_post(p_status->write_sem);
		return (-1);
	}
	if ((p_status->infos->end) || ((write(1, to_write, lenght)) == -1))
	{
		sem_post(p_status->write_sem);
		return (-1);
	}
	if (p_status->infos->nb_philo == p_status->infos->nb_philo_finished)
		p_status->infos->end = 1;
	if ((sem_post(p_status->write_sem)) == -1)
		return (-1);
	if (p_status->infos->end)
		return (-1);
	return (0);
}
