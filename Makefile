# Colors MAC OS
M_RED = \033[1;31m
M_GREEN = \033[1;32m
M_BLUE = \033[1;34m
M_WHITE = \033[1;37m

# Colors LINUX
_GREY=	$'\e[30m
_RED=	$'\e[31m
_GREEN=	$'\e[32m
_YELLOW=$'\e[33m
_BLUE=	$'\e[34m
_PURPLE=$'\e[35m
_CYAN=	$'\e[36m
_WHITE=	$'\e[37m

################################################################
NAME = webserv
CC = g++
################################################################

################################################################
#SRC
SRC =	./Server/Server.cpp \
		./Port/Port.cpp \
		./Client/Client.cpp \
		./Request/Request.cpp \
		./Response/Response.cpp \
		./Config/Config.cpp \
		./Config/VirtualHost.cpp \
		./Config/Location.cpp \
		./Config/ParsingAttributes.cpp \
		./Autoindex/Autoindex.cpp \
		./Cgi/Cgi.cpp \
		./Webserv.cpp
################################################################

################################################################
#INC
INCLUDE = -I.
################################################################

################################################################
#OBJ
OBJ_DIR = .objects
OBJ = $(SRC:.cpp=.o)
################################################################

################################################################
#DEP
DEP_DIR = .dependencies
DEP = $(OBJ:.o=.d)
################################################################

################################################################
#FLAGS
CFLAGS = -Wall -Wextra -Werror -std=c++98 $(INCLUDE)  #-Werror#-fsanitize=address #-std=c++98
RM		=	rm -rf
################################################################

all: $(NAME) 

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ) -g
#	@mkdir -p $(OBJ_DIR) && mkdir -p $(DEP_DIR)
#	@mv $(OBJ) $(OBJ_DIR) && mv $(DEP) $(DEP_DIR)
	@echo "$(M_GREEN)(0_0) $(NAME) is built$(M_WHITE)"

-include $(DEP)

%.o:%.cpp
	$(CC) -MMD $(CFLAGS) -c $< -o $@ -g 

clean:
	@$(RM) $(OBJ) $(DEP)
	@$(RM) *.gch
	@echo "$(M_BLUE)OBJ AND DEP DELETED MY MENZ$(M_WHITE)"

fclean: clean
	@$(RM) $(NAME)
#	@$(RM) $(OBJ_DIR) && $(RM) $(DEP_DIR)
	@echo "$(M_RED) $(NAME) ne vous embettera plus$(M_WHITE)"

re: fclean all

.PHONY:	all clean fclean re