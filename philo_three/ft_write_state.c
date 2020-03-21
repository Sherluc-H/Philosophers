#include <unistd.h>
#include "ft_write_state.h"
#include "ft_free.h"

int         ft_get_final_to_write(t_philo_infos *p_infos, char *to_write, char *str_end, int philo_num)
{
    long    current_time;
    int     i;

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

static int  ft_dont_unlock_if_eat_ok(t_philo_status *p_status, int is_eating)
{
    if (is_eating)
    {
        p_status->eat_count = p_status->eat_count + 1;
        if (p_status->infos->nb_time_eat > 0 && !(p_status->eat_ok) &&
            p_status->eat_count == p_status->infos->nb_time_eat)
        {
            p_status->eat_ok = 1;
            return (-1);
        }
    }
    return (0);
}

int         ft_write_state(t_philo_status *p_status, char *str_end, sem_t *write_sem, int is_eating)
{
    char to_write[1024];
    int lenght;

    if (p_status->died)
        return (-1);
    if ((sem_wait(write_sem)) == -1)
        return (-1);
    if (p_status->died)
        return (ft_unlock_sem(write_sem, NULL, -1));
    if ((lenght = ft_get_final_to_write(p_status->infos,
            to_write, str_end, p_status->philo_num)) == -1)
        return (ft_unlock_sem(write_sem, NULL, -1));
    if (!(p_status->died))
        if ((write(1, to_write, lenght)) == -1)
            return (ft_unlock_sem(write_sem, NULL, -1));
    if (ft_dont_unlock_if_eat_ok(p_status, is_eating) == -1)
        return (-1);
    if (p_status->died)
        return (ft_unlock_sem(write_sem, NULL, -1));
    if ((sem_post(write_sem)) == -1)
        return (-1);
    if (p_status->died)
        return (-1);
    return (0);
}

int         ft_announce_death(t_philo_status *p_status)
{
    char    to_write[1024];
    int     lenght;

    p_status->died = 1;
    p_status->infos->end = 1;
    if ((sem_wait(p_status->write_sem)) == -1)
        return (-1);
    if ((lenght = ft_get_final_to_write(p_status->infos,
            to_write, " died\n", p_status->philo_num)) != -1)
        write(1, to_write, lenght);
    return (0);
}
