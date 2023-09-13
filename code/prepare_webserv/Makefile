NAME 		=	./server

CXX 		=	c++
CXXFLAGS	=	-Wall -Wextra -Werror -std=c++98 -fsanitize=address

INCS		=	./Colors.hpp\
				./Server.hpp\
				
SRCS		=	./Colors.cpp\
				./minimal_web_server.cpp\
				./Server.cpp\

OBJS		=	$(SRCS:%.cpp=%.o)

RM			=	rm -f

all:	$(NAME)

$(NAME): $(OBJS) $(INCS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

%o:	%.cpp $(INCS)
	$(CXX) $(CXXFLAGS) $(INCS) -c $^ -o $@

clean: 
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re