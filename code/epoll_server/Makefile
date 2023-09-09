NAME_MULTI_SERVER = ./multi_server_kqueue
NAME_MULTI_CLIENT = ./multi_client

CXX = c++

SRCS_MULTI_SERVER = ./multi_flexing_server_kqueue.cpp
SRCS_MULTI_CLIENT = ./echo_client_multi.cpp

OBJS_MULTI_SERVER = $(SRCS_MULTI_SERVER:%.cpp=%.o)

OBJS_MULTI_CLIENT = $(SRCS_MULTI_CLIENT:%.cpp=%.o)

RM = rm -rf

all : $(NAME_MULTI_SERVER) $(NAME_MULTI_CLIENT)

%.o: %.cpp
	$(CXX) -c $^ -o $@ 

$(NAME_MULTI_SERVER): $(OBJS_MULTI_SERVER)
	$(CXX) $^ -o $@

$(NAME_MULTI_CLIENT): $(OBJS_MULTI_CLIENT)
	$(CXX) $^ -o $@

clean :
	$(RM) $(OBJS_MULTI_SERVER) $(OBJS_MULTI_CLIENT)

fclean : clean
	$(RM) $(NAME_MULTI_SERVER) $(NAME_MULTI_CLIENT)

.PHONY : all clean fclean re bonus