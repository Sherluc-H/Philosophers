#ifndef FT_WRITE_STATE_H
# define FT_WRITE_STATE_H

# include "philo_one.h"

int  ft_get_final_to_write(t_philo_infos *p_infos, char *to_write, char *str_end, int philo_num);
int  ft_write_state(t_philo_status *p_status, char *str_end, int lenght);

#endif
