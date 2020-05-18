/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_free.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lhuang <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/19 22:26:58 by lhuang            #+#    #+#             */
/*   Updated: 2020/05/18 18:06:42 by lhuang           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_FREE_H
# define FT_FREE_H

# include <semaphore.h>

int	ft_unlock_sem(sem_t *sem_one, sem_t *sem_two, int ret);
int	ft_unlink_n_close_sem(sem_t *forks_sem, sem_t *write_sem,
	sem_t *take_sem, int ret);

#endif
