NAME		=	webserv
CXX			=	c++
CXXFLAGS 		=	-Wall -Werror -Wextra -std=c++98 -g #-fsanitize=address
SRCS			=	main.cpp ServerContext.cpp Req.cpp Res.cpp WebServer_utils.cpp \
				WebServer.cpp ConnStream.cpp  FileManager.cpp __raw_data_utils.cpp

SRC			=	$(addprefix srcs/,$(SRCS)) 

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