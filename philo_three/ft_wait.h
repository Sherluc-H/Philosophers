/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_wait.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lhuang <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/20 00:30:21 by lhuang            #+#    #+#             */
/*   Updated: 2020/05/20 14:23:14 by lhuang           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_WAIT_H
# define FT_WAIT_H

# include "philo_three.h"

int	ft_wait_end(t_philo_infos *p_infos, t_philo_status *p_status);
int	ft_create_wait_threads(t_philo_infos *p_infos, t_philo_status *p_status);

#endif
