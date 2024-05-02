NAME		=	webserv
CXX			=	c++
CXXFLAGS 	=	-Wall -Werror -Wextra -std=c++98 -g # -fsanitize=address

SRCS		=	main.cpp ServerContext.cpp Req.cpp Res.cpp \
				WebServer.cpp ConnStream.cpp

UTILS		= __raw_data_utils.cpp __file_manager_utils.cpp __webserv_utils.cpp

SRC			=	$(addprefix srcs/,$(SRCS)) \
				$(addprefix srcs/utils/,$(UTILS)) 

INCLUDES	=	-I ./inc

#SRC			=	$(addprefix srcs/,$(SRCS))

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
