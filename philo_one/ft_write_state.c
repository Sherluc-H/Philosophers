/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_write_state.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lhuang <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/19 21:31:26 by lhuang            #+#    #+#             */
/*   Updated: 2020/05/18 14:50:50 by lhuang           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include "ft_write_state.h"

int	ft_get_final_to_write(t_philo_infos *p_infos, char *to_write,
	char *str_end, int philo_num)
{
	long	current_time;
	int		i;

	i = 0;
	if ((current_time = ft_get_current_time()) == -1)
		return (-1);
	ft_add_nbr_to_str(current_time - p_infos->starting_time, to_write, &i);
	ft_add_str_to_str(" ", to_write, &i);
	ft_add_nbr_to_str(philo_num + 1, to_write, &i);
	ft_add_str_to_str(str_end, to_write, &i);
	to_write[i] = '\0';
	return (i);
}

int	ft_write_state(t_philo_status *p_status, char *str_end, int lenght)
{
	char	to_write[1024];

	if (p_status->infos->end)
		return (-1);
	if ((pthread_mutex_lock(p_status->m_write)))
		return (-1);
	if ((lenght = ft_get_final_to_write(p_status->infos,
			to_write, str_end, p_status->philo_num)) == -1)
	{
		pthread_mutex_unlock(p_status->m_write);
		return (-1);
	}
	if ((p_status->infos->end) || ((write(1, to_write, lenght)) == -1))
	{
		pthread_mutex_unlock(p_status->m_write);
		return (-1);
	}
	if (p_status->infos->nb_philo_finished == p_status->infos->nb_philo)
		p_status->infos->end = 1;
	if ((pthread_mutex_unlock(p_status->m_write)))
		return (-1);
	if (p_status->infos->end)
		return (-1);
	return (0);
}
