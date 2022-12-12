#ifndef PARSINGATTRIBUTES_HPP
# define PARSINGATTRIBUTES_HPP

# include <iostream>
# include <string>
# include <map>
# include <vector>

namespace WS {

	class ParsingAttributes {

		public:

			typedef std::vector<std::string>::iterator				it_vect;
			typedef std::map<std::string, std::string>::iterator	it_maps;
			typedef std::map<int, std::string>::iterator			it_mapi;

			ParsingAttributes();
			ParsingAttributes(ParsingAttributes const & other);
			~ParsingAttributes();
			ParsingAttributes& operator=(ParsingAttributes const & other);

			void print(void);
			long long	get_body_size(void);

			std::string							type;

			// VirtualHost attributes
			std::vector<std::string>			ports;
			std::vector<std::string>			ip;
			std::vector<std::string>			names;
			std::map<std::string, std::string>	cgi;

			// Location attributes
			std::string							name;

			// Common attributes
			std::vector<std::string>			index;
			std::vector<std::string> 			method;
			std::map<int, std::string>			error_page;
			std::string							root; 
			std::string 						upload;
			std::string							autoindex;	
			std::string 						client_max_body_size_str; 
			std::string 						client_max_body_size_type; 
			int									client_max_body_size_value;
			long long							client_long_max_body_size_value;
			int									return_num;
			std::string							return_uri;

	}; // class ParsingAttributes

} // namespace WS


#endif