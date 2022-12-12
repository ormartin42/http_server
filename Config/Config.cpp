#include "./Config.hpp"

namespace WS {

Config::Config(void) { }
Config::Config(Config const &other) { *this = other; }
Config::Config(std::string const config_file) : _config_filename(config_file) { }
Config &Config::operator=(Config const &other) {
	if (this != &other)
		this->_config = other._config;;
	return (*this);
}

Config::~Config()
{
	std::vector<VirtualHost>::iterator it;
	for (it = _server_vect.begin(); it != _server_vect.end(); it++)
	{
		if (it->e != NULL)
			delete it->e;
		it->e = NULL;
	}	
} 

void Config::launch_parsing(void)
{
	_check_extension();
	_read_file();
	_parse_file();
	_init_config_struct();
}

void Config::_check_extension(void)
{
	size_t pos = _config_filename.rfind(".conf");
	size_t len = _config_filename.length();
	
	if (pos == std::string::npos || (len - pos) != 5)
		throw std::invalid_argument(INVALID_EXT);
}

void Config::_read_file(void)
{
	std::ifstream	inFile(_config_filename.c_str(), std::ifstream::in);
	std::string		buff;

	if (!inFile.is_open())
		throw std::invalid_argument(ERROR_MSG"open()");
	
	if (inFile.peek() == std::ifstream::traits_type::eof())
		throw std::invalid_argument(ERROR_MSG"config file is empty");

	while (getline(inFile, buff))
	{
		_data.append(buff);
		_data.append("\n");
	}
	_data_length = _data.length();
	inFile.close();

	char *token;
	char delim[] = " \t\n\v\f\r";
	std::string tok_str;


	token = strtok(const_cast<char *>(_data.c_str()), delim);
	while (token != NULL)
	{
		tok_str = token;
		_data_vect.push_back(tok_str);
		token = strtok(NULL, delim);
		tok_str.clear();
	}

	if (_data_vect.size() == 0)
		throw std::invalid_argument(ERROR_MSG"only white spaces in config file");

	std::string first_elem = _data_vect.front();

	
	for (it_vect it = _data_vect.begin(); it != _data_vect.end(); it++)
	{
		if (first_elem == "}" && it->size() > 1)
		{
			if ((it->find_first_not_of("}")) == std::string::npos)
			{
				int len = it->size();
				*it = "}";
				++it;
				for (int i = 1; i < len; i++)
					_data_vect.insert(it, "}");
			}
		}	
	}

}

void Config::_parse_file(void)
{
	if (!_data.find("listen") && !_data.find("server"))
		throw std::invalid_argument(MISSING_LS);

	int right_braces = 0; 
	int left_braces = 0;
	
	for (it_str it = _data.begin(); it != _data.end(); ++it)
	{
		if (*it == '{')
			++left_braces;
		if (*it == '}')
			++right_braces;
	}

	if (!right_braces || !left_braces || right_braces != left_braces)
		throw std::invalid_argument(MISSING_BR);

	int 		server_flag = 0;
	int 		location_flag = 0;
	int			listen_flag = 0;
	it_vect		server_start;
	it_vect		location_start;
	std::string	listen_port;
	std::string	location_name;
	std::string ip = "";
	left_braces = 0;

	if ((_data_vect.front().compare("server")))
		throw std::invalid_argument(SYN_ERR" : server not in first position");

	for (it_vect it = _data_vect.begin(); it != _data_vect.end(); )
	{
		if (*it == "server")
		{
			if (server_flag)
				throw std::invalid_argument(SYN_ERR" : server within server");
			++it;
			if (*it == "{")
			{
				server_flag = 1;
				++it;
				server_start = it;
				continue;
			}
			else
				throw std::invalid_argument(SYN_ERR" : server not followed by brace");
		}
		else if (*it == "listen")
		{
			if (!server_flag || location_flag)
				throw std::invalid_argument(SYN_ERR" : listen outside server scope");
			if (!listen_flag)
				listen_port = _parse_listening_port(it, &ip);
			listen_flag = 1;
		}
		else if (*it == "location")
		{
			if (!server_flag)
				throw std::invalid_argument(SYN_ERR" : location outside server scope");
			it+=2;
			if (*it != "{")
				throw std::invalid_argument(MISSING_ARG" : location");
			if (!location_flag)
			{
				location_flag = 1;
				location_name = *(--it);
				location_start = ++it;
			}
			else if (location_flag)
			{
				++location_flag;
			}
		}
		else if (*it == "}")
		{
			if (!location_flag)
			{
				if (!listen_flag)
					throw std::invalid_argument(MISSING_LS);
				std::vector<std::string> server_bloc(server_start, it);
				_add_virtualhost(listen_port, server_bloc, _location_blocs, ip);
				--server_flag;
				--listen_flag;
				listen_port.clear();
				_location_blocs.clear();
				ip.clear();
			}
			else
				++left_braces;
			if (location_flag && location_flag == left_braces)
			{
				std::vector<std::string> location_bloc(location_start - 2, ++it);
				_location_blocs.push_back(location_bloc);
				location_flag = 0;
				left_braces = 0;
				--it;
			}
		}
		else if (*it == "{")
			throw std::invalid_argument(MISSING_BR);
		++it;
	}
}

void	Config::_add_virtualhost(std::string &listen_port, std::vector<std::string> &server_bloc,
								 std::vector<std::vector<std::string> > &location_blocs, std::string ip)
{
	VirtualHost vh(listen_port, server_bloc, location_blocs, ip);
	VirtualHost tmp = vh;
	_server_vect.push_back(tmp);
}

void Config::_init_config_struct(void)
{
	std::vector<VirtualHost>::iterator itv;

	for (itv = _server_vect.begin(); itv != _server_vect.end(); itv++)
	{
		VirtualHost tmp = *itv;
		for (it_vect it = (tmp.e->pa.ports).begin(); it != (tmp.e->pa.ports).end(); it++)
		{
			bool res = _config.insert(std::pair<std::string, Elems*>(*it, tmp.e)).second;
			if (res == false)
			{
				static char i = 1;
				int ii = i;
				std::ostringstream conv;
				conv << ii;
				std::string add(conv.str());
				*it += "-";
				*it += add;
				++i;
				_config.insert(std::pair<std::string, Elems*>(*it, tmp.e));
			}
		}
	}
}

std::string Config::_parse_listening_port(it_vect it, std::string *ip)
{
	++it;
	if (_is_listen_format_ok(*it) == false)
	{
		std::cout << *it << std::endl;
		throw std::invalid_argument(INVALID_ARG" : listening port");
	}

	std::string new_port = *it;
	bool resize = 1;
	if (new_port[new_port.length() - 1] != ';')
	{
		++it;
		if (*it == ";")
		{
			++it;
			resize = 0;
		}
		else
			throw std::invalid_argument(MISSING_SC" : listen");
	}
	if (resize)
		new_port.resize(new_port.size() - 1);
	int pos = new_port.find(":");
	if (pos != -1)  
	{
		*ip = new_port.substr(0, pos);
		new_port = new_port.substr(pos + 1);
	}
	else
	{
		if (new_port.find(".") != std::string::npos)
		{
			*ip = new_port;
			new_port = "8080";
		}
		else 
		{
			*ip = "127.0.0.1";
		}
	}
	return (new_port);
}

bool Config::_is_digits_or_semicolon(std::string const &str)
{
	return (str.find_first_not_of("012345689;") == std::string::npos);
}

bool Config::_is_listen_format_ok(std::string const &str)
{
	return (str.find_first_not_of("0123456789;.:") == std::string::npos);
}

Config::t_config_struct const Config::getConfigStruct(void) const { return (this->_config); }

bool Config::portExists(std::string const port_nbr)
{
	for (it_conf it = _config.begin(); it != _config.end(); it++)
	{
		if (it->first == port_nbr)
			return (true);
	}
	return (false);
}

Elems *Config::getPortConfig(std::string const port_nbr) const
{
	for (t_config_struct::const_iterator it = _config.begin(); it != _config.end(); it++)
	{
		if (it->first == port_nbr)
			return (it->second);
	}
	return (NULL);
}

void Config::print(void)
{
	std::map<std::string, ParsingAttributes>::iterator it2;
	for (it_conf it = _config.begin(); it != _config.end(); it++)
	{
		//std::cout << "[SERVER PORT] " << it->first << std::endl;
		it->second->pa.print();
		for (it2 = it->second->locations.begin(); it2!= it->second->locations.end(); it2++)
			it2->second.print();
	}
}

} // namespace WS