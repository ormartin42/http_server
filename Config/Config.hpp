#ifndef CONFIG_HPP
# define CONFIG_HPP

/* ERRORS */
# define ERROR_MSG		"Error: Config File: "
# define UNKNOWN_DIR	ERROR_MSG"Unknown directive"
# define INVALID_ARG	ERROR_MSG"Argument not valid"
# define INVALID_EXT	ERROR_MSG"Invalid file extension"
# define MISSING_SC		ERROR_MSG"Missing semicolon"
# define MISSING_BR		ERROR_MSG"Missing brace"
# define SYN_ERR		ERROR_MSG"Syntax error"
# define DUPLICATE_ARG	ERROR_MSG"Duplicated directive"
# define MISSING_ARG	ERROR_MSG"Missing argument"
# define MISSING_LS		ERROR_MSG"Missing listen or server"
# define ERROR_PAGE_NUM	INVALID_ARG" : error number must be between 300 and 599"

# include <iostream>
# include <fstream>
# include <sstream>
# include <string>
# include <map>
# include <stdexcept>
# include <cstring>
# include <algorithm>
# include <utility>

# include "VirtualHost.hpp"

namespace WS {

	class Config {

		public:
	
			typedef std::map<std::string, Elems*>	t_config_struct;
			typedef std::map<std::string, Elems*>::iterator					it_conf;
			typedef std::vector<std::string>::iterator						it_vect;
			typedef std::string::iterator									it_str;

			Config(void);
			Config(std::string const config_file);
			Config(Config const &other);
			virtual ~Config();
			Config &operator=(Config const &other);

			void launch_parsing(void);
			void print();

			bool portExists(std::string const port_nbr);
			
			t_config_struct const getConfigStruct(void) const;
			Elems *	getPortConfig(std::string const port_nbr) const;

		private:

			std::string const						_config_filename;
			std::vector<VirtualHost>				_server_vect;
			std::string 							_data;
			std::size_t								_data_length;
			std::vector<std::string>				_data_vect;
			t_config_struct							_config;
			std::vector<std::vector<std::string> > 	_location_blocs;
	
			void									_check_extension(void);
			void									_read_file(void);
			void									_parse_file(void);
			void									_init_config_struct(void);
			std::vector<std::string>				_split(const std::string &str,const std::string &delim);
			bool									_is_digits_or_semicolon(std::string const &str);
			bool									_is_listen_format_ok(std::string const &str);
			std::string								_parse_listening_port(it_vect it, std::string *ip);
			void									_add_virtualhost(std::string &listen_port, std::vector<std::string> &server_bloc,
																		std::vector<std::vector<std::string> > &location_blocs, std::string ip);

}; // class Config

} // namespace WS

#endif