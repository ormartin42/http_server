#include "Location.hpp"

namespace WS {

Location::Location() { }
Location::Location(Location const &other) { *this = other; }
Location::~Location() { 
	delete pa;
	pa = NULL;
}
Location &Location::operator=(Location const &other) {
	if (this != &other)
		this->pa = other.pa;
	return (*this);
}

Location::Location(t_vect const bbloc) : pa(new ParsingAttributes())
{
	pa->type = "Location";
	pa->autoindex = "off"; 					
	pa->client_max_body_size_value = 0;
	pa->return_num = 0;

	bloc = bbloc;
	_autoindex_flag = 0;
	_root_flag = 0;
	_upload_flag = 0;
	_return_flag = 0;

	initialize_blocs();	
	parse_blocs();
}

void Location::initialize_blocs(void)
{
	int n = std::count(bloc.begin(), bloc.end(), "location");
	int indent = 0;
	int count;
	int i = 0;

	for (it_vect it = bloc.begin(); it != bloc.end(); it++)
	{
		if (*it == "location")
		{
			++indent;
			++it;
		}
		if (indent == n)
		{
			it_vect start = it;
			while (*it != "}")
			{
				++count;
				++it;
			}
			std::vector<std::string> new_bloc(start, it);
			blocs.push_back(new_bloc);
			bloc.erase(start, it);
			it = bloc.begin();
			count = 0;
			indent = 0;
			if (n == 1 && i == 0)
			{
				parse(new_bloc);
				return;
			}
			--n;
		}
		++i;
	}
	check_blocs();
}

void Location::check_blocs(void)
{
	std::reverse(blocs.begin(), blocs.end());
	
	it_vov tmp;
	std::string name1;
	std::string name2;

	for (it_vov itv = blocs.begin(); itv != blocs.end();)
	{
		it_vect it = itv->begin();
		for (int i = 0; it != itv->end() && i < 1; i++)
			name1 = *it;
		++itv;
		if (itv != blocs.end())
		{
			it_vect itt = itv->begin();
			for (int i = 0; itt != itv->end() && i < 1; i++)
				name2 = *itt;
			//if (!name2.empty() && (name2.find(name1) != 0))
			//	_delete_and_throw(ERROR_MSG"wrong sub loc name");
			if (name1 == name2)
				blocs.erase(--itv);
		}
	}
}

void Location::parse_blocs(void)
{
	for (it_vov it = blocs.begin(); it != blocs.end(); it++)
	{
		t_vect tmp = *it;
		for (int i = 0; tmp.begin() != tmp.end() && i < 1; i++)
			parse(tmp);
	}
}

void Location::parse(std::vector<std::string> bloc)
{
	_data = bloc;
	_it = bloc.begin();
	pa->name = *_it;
	for (_it += 2; _it != bloc.end();)
	{
		if (*_it == "client_max_body_size")
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
		else if (*_it == "upload")
			_parseUpload();
		else if (*_it == "return")
			_parseReturn();
		else if (*_it == "location")
			;
		else
			_delete_and_throw(UNKNOWN_DIR);
		++_it;
	}
	_insert_new_element();
}

void Location::_insert_new_element(void)
{
	check();
	locations.insert(loc_map_pair(pa->name, ParsingAttributes(*pa)));
	clear();
}

void Location::clear(void)
{
	_data.clear();
	pa->name.clear();
	pa->index.clear();
	pa->autoindex = "off";
	pa->method.clear();
	pa->error_page.clear();
	pa->cgi.clear();
	pa->root.clear();
	pa->upload.clear();
	pa->client_max_body_size_str.clear();
	pa->client_max_body_size_type.clear();
	pa->client_max_body_size_value = 0;
	pa->return_num = 0;
	pa->return_uri.clear();
	_autoindex_flag = 0;
	_root_flag = 0;
	_upload_flag = 0;
	_return_flag = 0;
}

void Location::check(void)
{
	if (pa->method.size() == 0)
		pa->method.push_back("GET");
	if (pa->client_max_body_size_str.empty())
	{
		pa->client_max_body_size_str = "1M";
		pa->client_max_body_size_value = 1;
		pa->client_max_body_size_type = "M";
	}
	pa->client_long_max_body_size_value = pa->get_body_size();
}

void Location::_parseClientMaxBodySize(void)
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
	pa->client_max_body_size_str = tmp; 
	pa->client_max_body_size_value = n;
	pa->client_max_body_size_type = type;
}

void Location::_parseIndex(void)
{
	++_it;
	_parse_strings(pa->index);
}

void Location::_parseAutoIndex(void)
{
	++_it;
	if (!_autoindex_flag)
		pa->autoindex = _parse_single_argument();
	if (pa->autoindex != "on" && pa->autoindex != "off")
		_delete_and_throw(INVALID_ARG" : autoindex");
	if (_autoindex_flag)
		_delete_and_throw(DUPLICATE_ARG" : autoindex");
	_autoindex_flag = 1;
}

void Location::_parseErrorPage(void)
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
		if (pa->error_page.find(*it) == pa->error_page.end())
			pa->error_page.insert(std::pair<int, std::string>(*it, path));
	}
}

void Location::_parseRoot(void)
{
	if (_root_flag)
		_delete_and_throw(DUPLICATE_ARG" : root");
	++_it;
	pa->root = _parse_single_argument();
	_root_flag = 1;
}

void Location::_parseMethod(void)
{
	++_it;
	_parse_strings(pa->method);
	for (it_vect it = pa->method.begin(); it != pa->method.end(); it++)
	{
		if (*it != "GET" && *it != "POST" && *it != "DELETE")
			_delete_and_throw(INVALID_ARG" : method");
	}
}

void Location::_parseUpload(void)
{
	if (_upload_flag)
		_delete_and_throw(DUPLICATE_ARG" : upload");
	++_it;
	pa->upload = _parse_single_argument();
	_upload_flag = 1;
}

void Location::_parseReturn(void)
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
		pa->return_num = std::atoi(tmp.c_str());
	if (count)
	{
		++_it;
		if (!_return_flag)
			pa->return_uri = _parse_single_argument();
	}
	_return_flag = 1;
}

void Location::_parse_strings(t_vect &vector)
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

std::string Location::_parse_single_argument(void)
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

void Location::_parse_multiple_arguments(t_vect &vector)
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

bool Location::_is_all_digits(std::string const str)
{
	return (str.find_first_not_of("0123456789") == std::string::npos);
}

bool Location::_is_digits_or_semicolon(std::string const &str)
{
	return (str.find_first_not_of("0123456789;") == std::string::npos);
}

bool Location::_is_directive(std::string const str)
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

void Location::_delete_and_throw(std::string const msg)
{
	delete pa;
	throw std::invalid_argument(msg);
}

void Location::print(void)
{
	for (t_loc_map::iterator it = locations.begin(); it != locations.end(); it++)
		it->second.print();
}

} // namespace WS