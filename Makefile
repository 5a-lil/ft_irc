NAME = ircserv

CC = c++

CFLAGS = -Wall -Wextra -Werror -std=c++98 -g3

SRC =	main.cpp  Server.cpp Command.cpp Client.cpp Channel.cpp

OBJS = $(SRC:.cpp=.o)

%.o : %.cpp 
	$(CC) $(CFLAGS) -c $< -o $@

all : $(NAME)

$(NAME):$(OBJS)
		$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

clean:
		rm -rf $(OBJS)

fclean:clean
		rm -rf $(NAME)

re: fclean all

.PHONY: all, clean, fclean, re
