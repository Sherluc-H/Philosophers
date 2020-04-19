/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_free.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lhuang <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/19 23:39:12 by lhuang            #+#    #+#             */
/*   Updated: 2020/04/19 23:40:13 by lhuang           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_free.h"

int	ft_unlock_sem(sem_t *sem_one, sem_t *sem_two, int ret)
{
	if (sem_one)
	{
		sem_post(sem_one);
	}
	if (sem_two)
	{
		sem_post(sem_two);
	}
	return (ret);
}

int	ft_unlink_n_close_sem(sem_t *forks_sem, sem_t *write_sem, int ret)
{
	sem_unlink(FORKS_SEM);
	sem_unlink(WRITE_SEM);
	sem_close(forks_sem);
	sem_close(write_sem);
	return (ret);
}
