NAME		=	webserv
CXX			=	c++
CXXFLAGS 		=	-Wall -Werror -Wextra -std=c++98 #-g -fsanitize=address
SRC			=	srcs/main.cpp srcs/Server.cpp srcs/Server_utils.cpp
INCLUDES	=	-I./inc
OBJ			=	$(SRC:.cpp=.o)

%.o: %.cpp
	@${CXX} ${CXXFLAGS}  ${INCLUDES} -c $< -o $@

all: $(NAME)

$(NAME): ${OBJ}
		${CXX} ${CXXFLAGS} ${OBJ} ${INCLUDES} -o ${NAME}

clean:
		@rm -rf ${OBJ}

fclean: clean
		@rm -rf ${NAME}

re: fclean ${NAME}