NAME		=	webserv
CXX			=	c++
CXXFLAGS 		=	-Wall -Werror -Wextra -std=c++98 #-g -fsanitize=address
SRC			=	srcs/main.cpp srcs/Server.cpp srcs/Req.cpp srcs/Req_utils.cpp srcs/WebServer_utils.cpp srcs/WebServer.cpp

INCLUDES	=	-I ./inc

#SRC			=	$(addprefix srcs/,$(SRCS))

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