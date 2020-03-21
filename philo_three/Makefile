CC = gcc
CFLAGS = -Wall -Werror -Wextra
RM = rm -f
NAME = philo_three
SRCS = philo_three.c \
		ft_philo_utils.c \
		ft_run_process.c \
		ft_write_state.c \
		ft_wait.c \
		ft_free.c
OBJS = $(SRCS:.c=.o)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

all: $(NAME)

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
