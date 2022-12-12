#ifndef VIRTUALHOST_HPP
# define VIRTUALHOST_HPP

# include <iostream>
# include <string>
# include <map>
# include <vector>
# include <algorithm>
# include <utility>
# include <unistd.h>
# include <sstream>
# include <stdexcept>

# include "Location.hpp"
# include "ParsingAttributes.hpp"

namespace WS {

	struct Elems {

		ParsingAttributes pa;
		std::map<std::string, ParsingAttributes> locations;
	
	}; // struct Elems

	class VirtualHost {

		public:

			typedef std::vector<std::string>							t_vect;
			typedef std::vector<std::string>::iterator 					it_vect;
			typedef std::vector<std::vector<std::string> > 				t_vov;
			typedef std::vector<std::vector<std::string> >::iterator	it_vov;
			typedef std::map<std::string, ParsingAttributes>			t_loc_map;
			typedef std::map<std::string, ParsingAttributes>::iterator	it_map;
			typedef std::pair<std::string, ParsingAttributes>			loc_map_pair;

			VirtualHost();
			VirtualHost(std::string const port, t_vect const data, t_vov location_blocs, std::string ip);
			VirtualHost(VirtualHost const &other);
			virtual ~VirtualHost();
			VirtualHost &operator=(VirtualHost const &other);

			void parse(void);
			void parse_locations(void);
			void check(void);
			void print(void);
			
			Elems		*e;

		private:
			
			it_vect		_it;
			t_vect		_data;
			t_vov		_location_blocs;
			bool		_autoindex_flagg;
			bool		_root_flag;
			bool		_upload_flag;
			bool		_listen_flag;
			bool		_return_flag;
			bool		_location_flag;
			
			void 		_parseServerName(void);
			void 		_parseListen(void);
			void 		_parseClientMaxBodySize(void);
			void 		_parseIndex(void);
			void 		_parseAutoIndex(void);
			void 		_parseErrorPage(void);
			void 		_parseRoot(void);
			void 		_parseMethod(void);
			void 		_parseCgi(void);
			void 		_parseUpload(void);
			void 		_parseReturn(void);

			bool 		_is_directive(std::string const str);
			bool 		_is_all_digits(std::string const str);
			bool 		_is_digits_or_semicolon(std::string const &str);
			bool		_is_listen_format_ok(std::string const &str);
			void 		_parse_strings(t_vect &vector);
			std::string	_parse_single_argument(void);
			void 		_parse_multiple_arguments(t_vect &vector);
			void		_delete_and_throw(std::string const msg);

	}; // class VirtualHost

} // namespace WS


#endif