/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_two.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lhuang <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/19 22:03:03 by lhuang            #+#    #+#             */
/*   Updated: 2020/04/19 23:01:24 by lhuang           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "philo_two.h"
#include "ft_philo_utils.h"
#include "ft_run_threads.h"
#include "ft_free.h"

static int	ft_check_args(int argc, char *argv[], t_philo_infos *p_infos)
{
	if (argc != 5 && argc != 6)
	{
		write(2, "Wrong number of arguments\n", 26);
		return (1);
	}
	if ((ft_get_philo_infos(argc, argv, p_infos)) == -1)
	{
		write(2, "Wrong arguments\n", 16);
		return (1);
	}
	return (0);
}

static int	ft_open_sem(sem_t **forks_sem, sem_t **write_sem, int mode)
{
	if ((*forks_sem = sem_open(FORKS_SEM, mode, 0660, 0)) == SEM_FAILED)
	{
		if ((sem_unlink(FORKS_SEM)) == -1)
			return (-1);
		if ((*forks_sem = sem_open(FORKS_SEM, mode, 0660, 0)) == SEM_FAILED)
			return (-1);
	}
	if ((*write_sem = sem_open(WRITE_SEM, mode, 0660, 1)) == SEM_FAILED)
	{
		if ((sem_unlink(WRITE_SEM)) == -1)
		{
			sem_unlink(FORKS_SEM);
			sem_close(*forks_sem);
			return (-1);
		}
		if ((*write_sem = sem_open(WRITE_SEM, mode, 0660, 1)) == SEM_FAILED)
		{
			sem_unlink(FORKS_SEM);
			sem_close(*forks_sem);
			return (-1);
		}
	}
	return (0);
}

static int	ft_prepare_infos(t_philo_infos *p_infos, t_philo_status *p_status,
	sem_t *forks_sem, sem_t *write_sem)
{
	int i;

	i = 0;
	while (i < p_infos->nb_philo)
	{
		if ((sem_post(forks_sem)) == -1)
			return (-1);
		p_status[i].philo_num = i;
		p_status[i].eat_count = 0;
		p_status[i].eaten_time = p_infos->starting_time;
		p_status[i].eat_ok = 0;
		p_status[i].infos = p_infos;
		p_status[i].forks_sem = forks_sem;
		p_status[i].write_sem = write_sem;
		i++;
	}
	return (0);
}

static int	ft_create_threads(t_philo_infos *p_infos, t_philo_status *p_status)
{
	pthread_t	end_thread;
	int			i;
	int			ret;

	i = 0;
	ret = 0;
	while (i < p_infos->nb_philo)
	{
		if ((ret = pthread_create(&(p_status[i].thread), NULL, ft_philo_thread,
				&(p_status[i]))) != 0)
			return (ret);
		i++;
	}
	if ((ret = pthread_create(&end_thread, NULL, ft_end_thread, p_status)) != 0)
		return (ret);
	if ((ret = pthread_detach(end_thread)) != 0)
		return (ret);
	i = 0;
	while (i < p_infos->nb_philo)
	{
		if ((ret = pthread_join(p_status[i].thread, NULL)) != 0)
			return (ret);
		i++;
	}
	return (ret);
}

int			main(int argc, char *argv[])
{
	t_philo_infos	p_infos;
	t_philo_status	*p_status;
	sem_t			*forks_sem;
	sem_t			*write_sem;
	int				ret;

	if ((ft_check_args(argc, argv, &p_infos)) == 1)
		return (1);
	if ((ft_open_sem(&forks_sem, &write_sem, O_CREAT | O_EXCL)) == -1)
		return (-1);
	if (!(p_status = malloc(sizeof(*p_status) * p_infos.nb_philo)))
		return (ft_unlink_n_close_sem(forks_sem, write_sem, 1));
	if ((ret = ft_prepare_infos(&p_infos, p_status,
		forks_sem, write_sem)) == -1)
	{
		free(p_status);
		return (ft_unlink_n_close_sem(forks_sem, write_sem, ret));
	}
	ret = ft_create_threads(&p_infos, p_status);
	free(p_status);
	return (ft_unlink_n_close_sem(forks_sem, write_sem, ret));
}
