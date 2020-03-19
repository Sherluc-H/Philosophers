#include <unistd.h>
#include "philo_three.h"
#include "ft_philo_utils.h"

int     ft_check_args(int argc, char *argv[], t_philo_infos *p_infos)
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

int     main(int argc, char *argv[])
{
    t_philo_infos   p_infos;

    if ((ft_check_args(argc, argv, &p_infos)) == 1)
        return (1);
    return (0);
}