NAME		=	webserv
CXX			=	c++
CXXFLAGS 	=	-Wall -Werror -Wextra -std=c++98 -g # -fsanitize=address

SRCS		=	main.cpp ServerContext.cpp WebServer.cpp \
				/parser/Lexer.cpp /parser/Parser.cpp /parser/ParserUtils.cpp

UTILS		=	__raw_data_utils.cpp __file_manager_utils.cpp __webserv_utils.cpp

CONN		=	Req.cpp Res.cpp ConnStream.cpp

SRC			=	$(addprefix srcs/,$(SRCS)) \
				$(addprefix srcs/utils/,$(UTILS)) \
				$(addprefix srcs/conn/,$(CONN)) 

INCLUDES	=	-I ./inc

OBJ			=	$(SRC:.cpp=.o)
%.o: %.cpp
	@${CXX} ${CXXFLAGS} ${INCLUDES} -c $< -o $@

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
