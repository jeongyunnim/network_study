NAME_STD_SERVER = ./server
NAME_STD_CLIENT = ./client

CC = cc

SRCS_STD_SERVER = ./echo_stdserv.c
SRCS_STD_CLIENT = ./echo_client.c

OBJS_STD_SERVER = $(SRCS_STD_SERVER:%.c=%.o)

OBJS_STD_CLIENT = $(SRCS_STD_CLIENT:%.c=%.o)

RM = rm -rf

all : $(NAME_STD_SERVER) $(NAME_STD_CLIENT)

%.o: %.c
	$(CC) -c $^ -o $@ 

$(NAME_STD_SERVER): $(OBJS_STD_SERVER)
	$(CC) $^ -o $@

$(NAME_STD_CLIENT): $(OBJS_STD_CLIENT)
	$(CC) $^ -o $@

clean :
	$(RM) $(OBJS_STD_SERVER) $(OBJS_STD_CLIENT)

fclean : clean
	$(RM) $(NAME_STD_SERVER) $(NAME_STD_CLIENT)

.PHONY : all clean fclean re bonus