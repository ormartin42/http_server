#ifndef CGI_HPP
# define CGI_HPP

# include <iostream>
# include <string>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>
# include <string.h>
# include <stdio.h>
# include <fstream>
# include <stdlib.h>
# include <map>
# include <cctype>

# include "../Client/Client.hpp"
# include "../Server/Server.hpp"

namespace WS {

class Cgi {

    public:

		typedef std::map<std::string, std::string> t_map;
    
        Cgi();
		Cgi(Client *cli, Elems *el, ParsingAttributes pa);
		Cgi(Cgi const & other);
        ~Cgi();
		Cgi &operator=(Cgi const & other);

		void	init_attributes();
        void    exec_cgi();
        void    init_arg();
        void    init_env();
		void	init_map();
	
	private:
		
		Client				*_cli;
		ParsingAttributes	_serv_conf;
		ParsingAttributes 	_loc_conf;
		std::string			_cgi_ext;
    
	    char   				**_env;
        char  				*_arg[3];
        std::string			_path_bin_cgi;	
		std::string 		_extension;
		std::string			_path_file;
		bool				_loc_flag;
		t_map 				_map;

}; // class Cgi

} // namespace WS

#endif
