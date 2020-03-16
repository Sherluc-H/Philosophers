/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_philo_utils.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lhuang <lhuang@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/29 10:28:04 by lhuang            #+#    #+#             */
/*   Updated: 2020/02/29 15:04:40 by lhuang           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PHILO_UTILS_H
# define FT_PHILO_UTILS_H

typedef struct	s_philo_infos
{
	int		nb_philo;
	int		time_td;
	int		time_te;
	int		time_ts;
	int		nb_time_eat;
	int		nb_philo_finished;
	long	starting_time;
	int		end;
	int		dead_philo;
}				t_philo_infos;

typedef enum	e_state
{
	EATING = 1,
	SLEEPING,
	THINKING,
	TAKE,
	DEAD
}				t_state;

int				ft_simple_atoi(char *str);
void			ft_add_nbr_to_str(long nb, char *tab, int *i);
void			ft_add_str_to_str(char *str, char *to_write, int *i);
long			ft_get_current_time(void);
int				ft_get_philo_infos(int argc, char **argv,
					t_philo_infos *p_infos);

#endif
