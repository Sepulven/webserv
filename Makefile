NAME		=	webserv
CXX			=	c++
CXXFLAGS 		=	-Wall -Werror -Wextra -std=c++98 -g #-fsanitize=address
SRC			=	srcs/main.cpp srcs/Server.cpp srcs/Req.cpp srcs/WebServer_utils.cpp \
				srcs/WebServer.cpp srcs/parser/Lexer.cpp srcs/parser/Parser.cpp \
				srcs/parser/ParserUtils.cpp

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