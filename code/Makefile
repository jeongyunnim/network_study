NAME_MULTI_SERVER = ./multi_server
NAME_MULTI_CLIENT = ./multi_client

CC = cc

SRCS_MULTI_SERVER = ./multi_flexing_server.c
SRCS_MULTI_CLIENT = ./echo_client_multi.c

OBJS_MULTI_SERVER = $(SRCS_MULTI_SERVER:%.c=%.o)

OBJS_MULTI_CLIENT = $(SRCS_MULTI_CLIENT:%.c=%.o)

RM = rm -rf

all : $(NAME_MULTI_SERVER) $(NAME_MULTI_CLIENT)

%.o: %.c
	$(CC) -c $^ -o $@ 

$(NAME_MULTI_SERVER): $(OBJS_MULTI_SERVER)
	$(CC) $^ -o $@

$(NAME_MULTI_CLIENT): $(OBJS_MULTI_CLIENT)
	$(CC) $^ -o $@

clean :
	$(RM) $(OBJS_MULTI_SERVER) $(OBJS_MULTI_CLIENT)

fclean : clean
	$(RM) $(NAME_MULTI_SERVER) $(NAME_MULTI_CLIENT)

.PHONY : all clean fclean re bonus