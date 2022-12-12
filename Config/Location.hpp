#ifndef LOCATION_HPP
# define LOCATION_HPP

# define ERROR_MSG		"Error: Config File: "
# define UNKNOWN_DIR	ERROR_MSG"Unknown directive"
# define INVALID_ARG	ERROR_MSG"Argument not valid"
# define MISSING_SC		ERROR_MSG"Missing semicolon"
# define DUPLICATE_ARG	ERROR_MSG"Duplicated directive"
# define MISSING_ARG	ERROR_MSG"Missing argument"
# define ERROR_PAGE_NUM	INVALID_ARG" : error number must be between 300 and 599"

# include <iostream>
# include <string>
# include <map>
# include <vector>
# include <algorithm>
# include <utility>
# include <unistd.h>
# include <sstream>
# include <stdexcept>
# include "ParsingAttributes.hpp"

namespace WS {

	class Location {

	public:

		typedef std::vector<std::string>::iterator					it_vect;
		typedef std::vector<std::string>							t_vect;
		typedef std::vector<std::vector<std::string> >::iterator	it_vov;
		typedef std::vector<std::vector<std::string> >				t_vov;
		typedef std::map<std::string, ParsingAttributes>			t_loc_map;
		typedef std::pair<std::string, ParsingAttributes>			loc_map_pair;	

		Location();
		Location(t_vect const bbloc);
		Location(Location const &other);
		virtual ~Location();
		Location& operator=(Location const &other);

		void parse(std::vector<std::string> bloc);
		void initialize_blocs(void);
		void parse_blocs(void);
		void check_blocs(void);
		void clear(void);
		void check(void);
		void print(void);

		t_loc_map			locations;
	
	private:

		ParsingAttributes	*pa;
		t_vect				bloc;
		t_vov 				blocs;
		t_vect				_data;
		it_vect				_it;
		bool				_autoindex_flag;
		bool				_root_flag;
		bool				_upload_flag;
		bool				_listen_flag;
		bool				_return_flag;

		void 				_parseListen(void);
		void 				_parseClientMaxBodySize(void);
		void 				_parseIndex(void);
		void 				_parseAutoIndex(void);
		void 				_parseErrorPage(void);
		void 				_parseRoot(void);
		void 				_parseMethod(void);
		void				_parseUpload(void);
		void				_parseReturn(void);
	
		bool		 		_is_directive(std::string const str);
		bool		 		_is_all_digits(std::string const str);
		bool		 		_is_digits_or_semicolon(std::string const &str);
		void				_parse_strings(t_vect &vector);
		std::string			_parse_single_argument(void);
		void				_parse_multiple_arguments(t_vect &vector);
		void				_insert_new_element(void);
		void				_delete_and_throw(std::string const msg);

}; // class Location

} // namespace WS


#endif