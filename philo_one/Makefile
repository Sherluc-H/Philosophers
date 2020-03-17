CC = gcc
CFLAGS = -Wall -Werror -Wextra
RM = rm -f
NAME = philo_one
SRCS = philo_one.c \
		ft_philo_utils.c
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
