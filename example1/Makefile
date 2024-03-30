NAME		=	btc
CC			=	c++
CFLAGS 		=	-Wall -Werror -Wextra -std=c++98 # -g -fsanitize=address
SRC			=	main.cpp
INCLUDES	=	-I.
OBJ			=	$(SRC:.cpp=.o)

%.o: %.cpp
	@${CC} ${CFLAGS}  ${INCLUDES} -c $< -o $@

all: $(NAME)

$(NAME): ${OBJ}
		${CC} ${CFLAGS} ${OBJ} ${INCLUDES} -o ${NAME}

clean:
		@rm -rf ${OBJ}

fclean: clean
		@rm -rf ${NAME}

sender:
	${CC} ${CFLAGS} sender.cpp ${INCLUDES} -o sender

receiver:
	${CC} ${CFLAGS} receiver.cpp ${INCLUDES} -o receiver

re: fclean ${NAME}