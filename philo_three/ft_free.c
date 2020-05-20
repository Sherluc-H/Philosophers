/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_free.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lhuang <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/19 23:39:12 by lhuang            #+#    #+#             */
/*   Updated: 2020/05/20 14:27:47 by lhuang           ###   ########.fr       */
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

int	ft_unlink_n_close_sem(sem_t *forks_sem, sem_t *write_sem,
	sem_t *take_sem, int ret)
{
	if (forks_sem)
	{
		sem_unlink(FORKS_SEM);
		sem_close(forks_sem);
	}
	if (write_sem)
	{
		sem_unlink(WRITE_SEM);
		sem_close(write_sem);
	}
	if (take_sem)
	{
		sem_unlink(TAKE_SEM);
		sem_close(take_sem);
	}
	return (ret);
}
