#include "ParsingAttributes.hpp"

namespace WS {

ParsingAttributes::ParsingAttributes() { }
ParsingAttributes::ParsingAttributes(ParsingAttributes const & other) { *this = other; }
ParsingAttributes::~ParsingAttributes() { }
ParsingAttributes &ParsingAttributes::operator=(ParsingAttributes const & other ) {
	if (this != &other)
	{
		this->type = other.type;
		this->ports = other.ports;
		this->ip = other.ip;
		this->names = other.names;
		this->cgi = other.cgi;
		this->name = other.name;
		this->index = other.index;
		this->method = other.method;
		this->error_page = other.error_page;
		this->root = other.root;
		this->upload = other.upload;
		this->autoindex = other.autoindex;
		this->client_max_body_size_str = other.client_max_body_size_str;
		this->client_max_body_size_type = other.client_max_body_size_type;
		this->client_max_body_size_value = other.client_max_body_size_value;
		this->return_num = other.return_num;
		this->return_uri = other.return_uri;
	}
	return (*this);
}

void ParsingAttributes::print(void)
{
	std::cout << "-----" << type << "-----" << std::endl;
	
	// VirtualHost attributes
	if (type == "VirtualHost")
	{
		for (it_vect it = ports.begin(); it != ports.end(); it++)
			std::cout << "[PORTS]		" << *it << std::endl;
		for (it_vect it = ip.begin(); it != ip.end(); it++)
			std::cout << "[IP]		" << *it << std::endl;

		for (it_vect it = names.begin(); it != names.end(); it++)
			std::cout << "[NAMES]		" << *it << std::endl;
		for (it_maps it = cgi.begin(); it != cgi.end(); it++)
			std::cout << "[CGI]		" << it->first << " | " << it->second << std::endl;
	}

	// Location attributes
	if (type == "Location")
		std::cout << "[NAME]		" << name << std::endl;

	// Common attributes
	for (it_vect it = index.begin(); it != index.end(); it++)
		std::cout << "[INDEX]		" << *it << std::endl;
	for (it_vect it = method.begin(); it != method.end(); it++)
		std::cout << "[METHOD]	" << *it << std::endl;
	for (it_mapi it = error_page.begin(); it != error_page.end(); it++)
		std::cout << "[ERROR]		" << it->first << " | " << it->second << std::endl; 
	std::cout << "[ROOT]		" << root << std::endl;
	std::cout << "[UPLOAD]	" << upload << std::endl;
	std::cout << "[AUTOIND]	" << autoindex << std::endl;
	std::cout << "[CMBS]		STR: " << client_max_body_size_str << std::endl;
	std::cout << "[CMBS]		TYPE: " << client_max_body_size_type << std::endl;
	std::cout << "[CMBS]		VAL: " << client_max_body_size_value << std::endl;
	std::cout << "[RETURN]	NUM: " << return_num << std::endl;
	std::cout << "[RETURN]	URI: " << return_uri << std::endl;
	std::cout << std::endl;
}

long long ParsingAttributes::get_body_size(void)
{
	long long ret;
	if (client_max_body_size_type == "G")
		ret = (long long)client_max_body_size_value * 1000000000;
	else if (client_max_body_size_type == "M")
		ret = (long long)client_max_body_size_value * 1000000;
	else if (client_max_body_size_type == "K")
		ret = (long long)client_max_body_size_value * 1000;
	else
		ret = (long long)client_max_body_size_value;
	return (ret);
}

} // namespace WS