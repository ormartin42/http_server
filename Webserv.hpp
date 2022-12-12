#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <stdio.h>
# include <stdlib.h>
# include <iostream>

//# define BUFF_SIZE 500
# define DEFAULT_CONFIG "./www/conf/default.conf"
// METHOD
# define NOT_SPEC -1
# define OTHER 0
# define GET 1
# define POST 2
# define DELETE 3

# define _RED "\033[31m"
# define _BRED "\033[1;31m"
# define _GREEN "\033[32m"
# define _YELLOW "\033[33m"
# define _BLUE "\033[34m"
# define _PURPLE "\033[35m"
# define _BPURPLE "\033[1;35m"
# define _CYAN "\033[36m"
# define _BCYAN "\033[1;36m"
# define _END "\033[0m"



# include "./Server/Server.hpp"
# include <stdexcept>

#endif
