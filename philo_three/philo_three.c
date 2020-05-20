/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_three.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lhuang <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/19 23:24:09 by lhuang            #+#    #+#             */
/*   Updated: 2020/05/20 17:19:33 by lhuang           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include "philo_three.h"
#include "ft_run_process.h"
#include "ft_wait.h"
#include "ft_free.h"

int			ft_check_args(int argc, char *argv[], t_philo_infos *p_infos)
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

static int	ft_open_sem(sem_t **forks_sem, sem_t **write_sem,
	sem_t **take_sem, int oflag)
{
	if ((*forks_sem = sem_open(FORKS_SEM, oflag, 0660, 0)) == SEM_FAILED)
	{
		if ((sem_unlink(FORKS_SEM)) == -1)
			return (-1);
		if ((*forks_sem = sem_open(FORKS_SEM, oflag, 0660, 0)) == SEM_FAILED)
			return (-1);
	}
	if ((*write_sem = sem_open(WRITE_SEM, oflag, 0660, 1)) == SEM_FAILED)
	{
		if ((sem_unlink(WRITE_SEM)) == -1)
			return (ft_unlink_n_close_sem(*forks_sem, NULL, NULL, -1));
		if ((*write_sem = sem_open(WRITE_SEM, oflag, 0660, 1)) == SEM_FAILED)
			return (ft_unlink_n_close_sem(*forks_sem, NULL, NULL, -1));
	}
	if ((*take_sem = sem_open(TAKE_SEM, oflag, 0660, 1)) == SEM_FAILED)
	{
		if ((sem_unlink(TAKE_SEM)) == -1)
			return (ft_unlink_n_close_sem(*forks_sem, *write_sem, NULL, -1));
		if ((*take_sem = sem_open(TAKE_SEM, oflag, 0660, 1)) == SEM_FAILED)
			return (ft_unlink_n_close_sem(*forks_sem, *write_sem, NULL, -1));
	}
	return (0);
}

static int	ft_prepare_infos(t_philo_infos *p_infos, t_philo_status *p_status,
	t_all_sem *all_sem)
{
	int i;

	i = 0;
	while (i < p_infos->nb_philo)
	{
		p_status[i].philo_num = i;
		p_status[i].eat_count = 0;
		p_status[i].eaten_time = p_infos->starting_time;
		p_status[i].eat_ok = 0;
		p_status[i].died = 0;
		p_status[i].ender = 0;
		p_status[i].added = 0;
		p_status[i].infos = p_infos;
		p_status[i].forks_sem = all_sem->forks_sem;
		p_status[i].write_sem = all_sem->write_sem;
		p_status[i].take_sem = all_sem->take_sem;
		if ((sem_post(all_sem->forks_sem)) == -1)
			return (-1);
		i++;
	}
	return (0);
}

static int	ft_create_processes_n_wait_threads(t_philo_infos *p_infos,
	t_philo_status *p_status)
{
	int i;
	int j;

	i = 0;
	if ((p_infos->starting_time = ft_get_current_time()) == -1)
		return (-1);
	while (i < p_infos->nb_philo)
	{
		p_status[i].eaten_time = p_infos->starting_time;
		if ((p_status[i].pid = fork()) == -1)
		{
			j = 0;
			while (j < i)
			{
				kill(p_status[j].pid, SIGKILL);
				j++;
			}
			return (-1);
		}
		if (p_status[i].pid == 0)
			ft_run_philo_process(&(p_status[i]), p_status);
		i++;
	}
	return (ft_create_wait_threads(p_infos, p_status));
}

int			main(int argc, char *argv[])
{
	t_philo_infos	p_infos;
	t_philo_status	*p_status;
	t_all_sem		all_sem;
	int				ret;

	if ((ft_check_args(argc, argv, &p_infos)) == 1)
		return (1);
	if ((ft_open_sem(&(all_sem.forks_sem), &(all_sem.write_sem),
			&(all_sem.take_sem), O_CREAT | O_EXCL)) == -1)
		return (-1);
	if (!(p_status = malloc(sizeof(*p_status) * p_infos.nb_philo)))
	{
		return (ft_unlink_n_close_sem(all_sem.forks_sem, all_sem.write_sem,
			all_sem.take_sem, 1));
	}
	if ((ft_prepare_infos(&p_infos, p_status, &all_sem)) == -1)
	{
		free(p_status);
		return (ft_unlink_n_close_sem(all_sem.forks_sem, all_sem.write_sem,
			all_sem.take_sem, -1));
	}
	ret = ft_create_processes_n_wait_threads(&p_infos, p_status);
	free(p_status);
	return (ft_unlink_n_close_sem(all_sem.forks_sem, all_sem.write_sem,
			all_sem.take_sem, ret));
}
