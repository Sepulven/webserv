NAME		=	webserv
CXX			=	c++
CXXFLAGS 	=	-Wall -Werror -Wextra #-std=c++98 -g # -fsanitize=address
INCLUDES	=	-I ./inc


## * File folder defintion
PARSER		=	Parser.cpp ParserUtils.cpp Lexer.cpp

UTILS		=	__raw_data_utils.cpp __file_manager_utils.cpp __webserv_utils.cpp

CONN		=	Req.cpp Res.cpp ConnStream.cpp HttpError.cpp

SERVER_ENG	=	ServerContext.cpp WebServer.cpp ServerError.cpp

SRC			=	$(addprefix srcs/__utils/,$(UTILS)) \
				$(addprefix srcs/server_engine/,$(SERVER_ENG)) \
				$(addprefix srcs/parser/,$(PARSER)) \
				$(addprefix srcs/conn/,$(CONN)) \
				$(addprefix srcs/,main.cpp) \

OBJ			=	$(SRC:.cpp=.o)

## * Compilation phase
%.o: %.cpp
	@${CXX} ${CXXFLAGS} ${INCLUDES} -c $< -o $@

## * Makefile standart rules
all: $(NAME)

$(NAME): ${OBJ}
		${CXX} ${CXXFLAGS} ${OBJ} ${INCLUDES} -o ${NAME}

clean:
		@rm -rf ${OBJ}

fclean: clean
		@rm -rf ${NAME}

re: fclean ${NAME}

run: re
	./${NAME}
