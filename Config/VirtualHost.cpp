#include "VirtualHost.hpp"

namespace WS {

VirtualHost::VirtualHost() { }
VirtualHost::VirtualHost(VirtualHost const &other) {  *this = other; }
VirtualHost::~VirtualHost() { }
VirtualHost &VirtualHost::operator=(VirtualHost const &other)
{
	if (this != &other)
		this->e = other.e;
	return (*this);
}

VirtualHost::VirtualHost(std::string const port, t_vect const data, t_vov location_blocs, std::string ip)
{
	e = new Elems();

	e->pa.type = "VirtualHost";
	e->pa.ports.push_back(port);
	e->pa.ip.push_back(ip);
	e->pa.autoindex = "off"; 					
	e->pa.client_max_body_size_value = 0;
	e->pa.return_num = 0;
	_data = data;
	_location_blocs = location_blocs;
	_autoindex_flagg = 0;
	_root_flag = 0;
	_upload_flag = 0;
	_listen_flag = 0;
	_return_flag = 0;
	_location_flag = 0;

	parse();
	parse_locations();
	check();
}

void VirtualHost::parse(void)
{
	int loc = 0;
	int br = 0;

	for (_it = _data.begin() ; _it != _data.end();)
	{
		if (*_it == "server_name")
			_parseServerName();
		else if (*_it == "client_max_body_size")
			_parseClientMaxBodySize();
		else if (*_it == "index")
			_parseIndex();
		else if (*_it == "autoindex")
			_parseAutoIndex();
		else if (*_it == "error_page")
			_parseErrorPage();
		else if (*_it == "root")
			_parseRoot();
		else if (*_it == "method")
			_parseMethod();
		else if (*_it == "cgi")
			_parseCgi();
		else if (*_it == "upload")
			_parseUpload();
		else if (*_it == "return")
			_parseReturn();
		else if (*_it == "location") 
		{
			++loc;
			++_it;
			while (loc != br)
			{
				if (*_it == "location")
					++loc;
				if (*_it == "}")
					++br;
				++_it;
			}
			--_it;
		}
		else if (*_it == "listen") 
			_parseListen();
		else if (*_it == "}")
			continue;
		else
			_delete_and_throw(UNKNOWN_DIR);
		++_it;
	}
}

void VirtualHost::parse_locations(void)
{
	for (it_vov itv = _location_blocs.begin(); itv != _location_blocs.end(); itv++)
	{
		Location locs(*itv);
		t_loc_map tmp = locs.locations;

		for (it_map it = tmp.begin(); it != tmp.end(); it++)
		{
			bool res = e->locations.insert(loc_map_pair(it->first, it->second)).second;
			if (res == false)
				_delete_and_throw(DUPLICATE_ARG" : location");
		}
	}
}

void VirtualHost::check(void)
{
	if (e->pa.names.size() == 0)
		e->pa.names.push_back("localhost");		
	if (e->pa.root.empty())
	{
		char *buf = NULL;
		buf = getcwd(buf, 0);
		if (buf != NULL)
		{
			e->pa.root = buf;
			e->pa.root += "/www";
		}
		free(buf);
	}
	else
	{
		char *buf = NULL;
		buf = getcwd(buf, 0);
		std::string tmp = e->pa.root;
		if (buf != NULL)
		{
			e->pa.root = buf;
			e->pa.root += "/www";
			e->pa.root += tmp;
		}
		free(buf);
	}
	if (e->locations.size() > 0)
	{
		std::string tmp; 

		for (it_map it = e->locations.begin(); it != e->locations.end(); it++)
		{
			if (it->second.root == "")
				it->second.root = e->pa.root;
			else
			{
				tmp = it->second.root;
				it->second.root.clear();
				it->second.root = e->pa.root;
				it->second.root += tmp;
			}
		}
	}
	if (e->pa.method.size() == 0)
		e->pa.method.push_back("GET");
	if (e->pa.client_max_body_size_str.empty())
	{
		e->pa.client_max_body_size_str = "1M";
		e->pa.client_max_body_size_value = 1;
		e->pa.client_max_body_size_type = "M";
	}
	for (it_map it = e->locations.begin(); it != e->locations.end(); it++)
	{
		if (it->second.error_page.size() == 0)
			it->second.error_page = e->pa.error_page;
	}
	e->pa.client_long_max_body_size_value = e->pa.get_body_size();
}

void VirtualHost::_parseServerName(void)
{
	++_it;
	_parse_strings(e->pa.names);
}

void VirtualHost::_parseListen(void)
{

std::string ip;

	if (!_listen_flag)
	{
		++_it;
		if ((*_it)[(*_it).length() - 1] != ';')
			_it++;
		_listen_flag = 1;
		return;
	}

	++_it;
	if (_is_listen_format_ok(*_it) == false)
		_delete_and_throw(INVALID_ARG" : listen");
	std::string new_port = *_it;
	bool resize = 1;
	if (new_port[new_port.length() - 1] != ';')
	{
		++_it;
		if (*_it == ";")
		{
			++_it;
			resize = 0;
		}
		else
		_delete_and_throw(MISSING_SC" : listen");
	}
	if (resize)
		new_port.resize(new_port.size() - 1);
	int pos = new_port.find(":");
	if (pos != -1)
	{
		ip = new_port.substr(0, pos);
		new_port = new_port.substr(pos + 1);
	}
	else
	{
		if (new_port.find(".") != std::string::npos)
		{
			ip = new_port;
			new_port = "8080";
		}
		else 
			ip = "127.0.0.1";
	}
	
	long unsigned int i = 0;
	if (e->pa.ports.size() > 0)
	{
		for (std::vector<std::string>::iterator it = e->pa.ports.begin(); it != e->pa.ports.end();)
		{
			if (*it == new_port)
			{
				if (e->pa.ip[i] == ip)
					_delete_and_throw(DUPLICATE_ARG" : listen");
			}
			++it;
			++i;
		}
	}
	e->pa.ports.push_back(new_port);
	e->pa.ip.push_back(ip);		
}

void VirtualHost::_parseClientMaxBodySize(void)
{
	++_it;
	std::string tmp = _parse_single_argument();

	std::string num;
	std::string type;
	
	int i = 0;
	while (isdigit(tmp[i]))
		num += tmp[i++];
	while (isalpha(tmp[i]))
		type += tmp[i++];
	
	int n = std::atoi(num.c_str());
	if ((tmp[i] && tmp[i] != ';')
			|| (type != "G" && type != "M" && type != "K" && type != "B")
			|| n <= 0)
		_delete_and_throw(INVALID_ARG" : client_max_body_size");
	if ((type == "G" && n > 8)
			|| (type == "M" && n > 999)
			|| (type == "K" && n > 999999)
			|| (type == "K" && n > 999999999)) 
		_delete_and_throw(INVALID_ARG" : client_max_body_size");
	e->pa.client_max_body_size_str = tmp; 
	e->pa.client_max_body_size_value = n;
	e->pa.client_max_body_size_type = type;
}

void VirtualHost::_parseIndex(void)
{
	++_it;
	_parse_strings(e->pa.index);
}

void VirtualHost::_parseAutoIndex(void)
{
	++_it;
	if (!this->_autoindex_flagg)
		e->pa.autoindex = _parse_single_argument();
	if (e->pa.autoindex != "on" && e->pa.autoindex != "off")
		_delete_and_throw(INVALID_ARG" : autoindex");
	if (this->_autoindex_flagg == true)
		_delete_and_throw(DUPLICATE_ARG" : autoindex");
	this->_autoindex_flagg = 1;
}


void VirtualHost::_parseErrorPage(void)
{
	std::vector<int> nums;
	std::string path;
	std::string num_str;
	int	num;

	++_it;
	while ((*_it)[(*_it).length() - 1] != ';') 
	{
		*_it += ";";
		num_str = _parse_single_argument();
		if (!_is_all_digits(num_str))
			_delete_and_throw(INVALID_ARG" : error_page");
		num = std::atoi(num_str.c_str());
		if (num < 300 || num > 599)
			_delete_and_throw(ERROR_PAGE_NUM);
		nums.push_back(num);
		++_it;
	}
	path = _parse_single_argument();

	for (std::vector<int>::iterator it = nums.begin(); it != nums.end(); ++it)
	{
		if (e->pa.error_page.find(*it) == e->pa.error_page.end())
			e->pa.error_page.insert(std::pair<int, std::string>(*it, path));
	}
}

void VirtualHost::_parseRoot(void)
{
	if (_root_flag)
		_delete_and_throw(DUPLICATE_ARG" : root");
	++_it;
	e->pa.root = _parse_single_argument();
	_root_flag = 1;
}

void VirtualHost::_parseMethod(void)
{
	++_it;
	_parse_strings(e->pa.method);
	for (it_vect it = e->pa.method.begin(); it != e->pa.method.end(); it++)
	{
		if (*it != "GET" && *it != "POST" && *it != "DELETE")
			_delete_and_throw(INVALID_ARG" : method");
	}
}

void VirtualHost::_parseCgi(void)
{
	++_it;
	std::string extension;
	std::string path;
	if ((*_it)[0] != '.' && (*_it)[(*_it).length() - 1] != ';')
		_delete_and_throw(INVALID_ARG" : cgi extension");
	extension = *_it;
	++_it;
	path =_parse_single_argument();
	if (extension.empty() || path.empty()) 
		_delete_and_throw(MISSING_ARG" : cgi path");

	if (!e->pa.cgi.count(extension))
		e->pa.cgi.insert(std::pair<std::string, std::string>(extension, path));
}

void VirtualHost::_parseUpload(void)
{
	if (_upload_flag)
		_delete_and_throw(DUPLICATE_ARG" : upload");
	++_it;
	e->pa.upload = _parse_single_argument();
	_upload_flag = 1;
}

void VirtualHost::_parseReturn(void)
{
	++_it;
	int count = 0;
	while ((*_it)[(*_it).length() - 1] != ';')
	{
		++count;
		++_it;
	}
	_it -= count;
	std::string tmp = *_it;
	tmp.resize(tmp.size());
	if (count > 1 || !_is_all_digits(tmp))
		_delete_and_throw(INVALID_ARG" : return");
	if (!_return_flag)
		e->pa.return_num = std::atoi(tmp.c_str());
	if (count)
	{
		++_it;
		if (!_return_flag)
			e->pa.autoindex = _parse_single_argument();
	}
	_return_flag = 1;
}

void VirtualHost::_parse_strings(t_vect &vector)
{
	std::string arg;
	
	if ((*_it)[(*_it).length() - 1] == ';') 
	{
		arg = _parse_single_argument();
		vector.push_back(arg);
	}
	else
		_parse_multiple_arguments(vector);
}

std::string VirtualHost::_parse_single_argument(void)
{
	if (_is_directive(*_it))
		_delete_and_throw(INVALID_ARG);
	std::string arg = *_it;
	if ((*_it).find_first_of(";") != ((*_it).length() - 1))
	{
		++_it;
		if (*_it == ";")
			return (arg);
		else
			_delete_and_throw(MISSING_SC);
	}
	arg.resize(arg.size() - 1);
	return (arg);
}

void VirtualHost::_parse_multiple_arguments(t_vect &vector)
{
	std::string arg;
	while ((*_it)[(*_it).length() - 1] != ';') 
	{
		*_it += ";";
		arg = _parse_single_argument();
		vector.push_back(arg);
		++_it;
	}
	arg = _parse_single_argument();
	vector.push_back(arg);
}

bool VirtualHost::_is_all_digits(std::string const str)
{
	return (str.find_first_not_of("0123456789") == std::string::npos);
}

bool VirtualHost::_is_digits_or_semicolon(std::string const &str)
{
	return (str.find_first_not_of("0123456789;") == std::string::npos);
}

bool VirtualHost::_is_listen_format_ok(std::string const &str)
{
	return (str.find_first_not_of("0123456789;.:") == std::string::npos);
}

bool VirtualHost::_is_directive(std::string const str)
{
	std::string const directive[12] = {"server_name", "listen", "index", \
										"autoindex", "client_max_body_size", \
										"error_page", "location", "root", \
										"method", "upload", "cgi", "return" };
	for (int i = 0; i < 12; i++)
	{
		if (str == directive[i])
			return (true);
	}
	return (false);
}

void VirtualHost::_delete_and_throw(std::string const msg)
{
	delete e;
	throw std::invalid_argument(msg);
}

void VirtualHost::print(void)
{
	e->pa.print();
	for (it_map it = e->locations.begin(); it != e->locations.end(); it++)
		it->second.print();
	std::cout << std::endl;
}

} // namespace WS