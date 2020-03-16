/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_philo_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lhuang <lhuang@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/29 10:27:48 by lhuang            #+#    #+#             */
/*   Updated: 2020/02/29 14:16:34 by lhuang           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <sys/time.h>
#include "ft_philo_utils.h"

int		ft_simple_atoi(char *str)
{
	int		i;
	long	nb;

	i = 0;
	nb = 0;
	while (str[i] >= '0' && str[i] <= '9')
	{
		nb *= 10;
		nb += str[i] - 48;
		if (nb > 2147483647)
			return (0);
		i++;
	}
	if (str[i] != '\0')
		nb = 0;
	return (nb);
}

void	ft_add_nbr_to_str(long nb, char *tab, int *i)
{
	char c;

	if (nb > 9)
	{
		ft_add_nbr_to_str(nb / 10, tab, i);
		c = 48 + nb % 10;
		tab[*i] = c;
		*i = *i + 1;
	}
	else
	{
		c = 48 + nb;
		tab[*i] = c;
		*i = *i + 1;
	}
}

void	ft_add_str_to_str(char *str, char *to_write, int *i)
{
	int j;

	j = 0;
	while (str[j])
	{
		to_write[*i] = str[j];
		j++;
		*i = *i + 1;
	}
	to_write[*i] = '\0';
}

long	ft_get_current_time(void)
{
	struct timeval	tv;
	long			time_ms;

	if ((gettimeofday(&tv, NULL)) == -1)
		return (-1);
	time_ms = tv.tv_sec * 1000;
	time_ms += tv.tv_usec / 1000;
	return (time_ms);
}

int		ft_get_philo_infos(int argc, char **argv, t_philo_infos *p_infos)
{
	p_infos->nb_philo = ft_simple_atoi(argv[1]);
	p_infos->time_td = ft_simple_atoi(argv[2]);
	p_infos->time_te = ft_simple_atoi(argv[3]);
	p_infos->time_ts = ft_simple_atoi(argv[4]);
	if (p_infos->nb_philo == 0 || p_infos->time_td == 0 ||
		p_infos->time_te == 0 || p_infos->time_ts == 0)
		return (-1);
	if (argc == 6)
	{
		p_infos->nb_time_eat = ft_simple_atoi(argv[5]);
		if (p_infos->nb_time_eat == 0)
			return (-1);
	}
	else
		p_infos->nb_time_eat = 0;
	p_infos->nb_philo_finished = 0;
	p_infos->starting_time = ft_get_current_time();
	p_infos->end = 0;
	p_infos->dead_philo = -1;
	return (0);
}
