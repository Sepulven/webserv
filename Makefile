NAME		=	webserv
CXX			=	c++
CXXFLAGS	=	-Wall -Werror -Wextra -std=c++98 #-g -fsanitize=address
INCLUDES	=	-I./inc

SRCS		=	main.cpp Server.cpp WebServer_utils.cpp WebServer.cpp
SRC			=	$(addprefix srcs/,$(SRCS))

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