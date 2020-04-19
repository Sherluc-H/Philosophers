/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_write_state.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lhuang <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/19 23:12:22 by lhuang            #+#    #+#             */
/*   Updated: 2020/04/19 23:13:19 by lhuang           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_WRITE_STATE_H
# define FT_WRITE_STATE_H

# include "philo_two.h"

int	ft_get_final_to_write(t_philo_infos *p_infos, char *to_write,
	char *end_str, int philo_num);
int	ft_write_state(t_philo_status *p_status, char *end_str, int lenght);

#endif
