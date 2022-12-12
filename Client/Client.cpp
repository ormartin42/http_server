#include "./Client.hpp"
#include <iostream>

namespace WS {

Client::Client() { }
Client::Client(Client const & other) { *this = other; }
Client::~Client() { }
Client &Client::operator=(Client const & other) {
	if (this != &other)
	{
		this->_fd = other._fd;
		this->_ip =  other._ip;
		this->_port = other._port;
		this->_req = other._req;
		this->_res = other._res;
	}
	return (*this);
}

Client::Client(int cli_fd, sockaddr_in *cli_addr)
{
    _fd = cli_fd;
	_fullreq = 0;
    if (_fd < 0)
		throw std::invalid_argument("Error con Client socket : accept() failed");
	_ip = inet_ntoa(cli_addr->sin_addr);
	std::stringstream p;
	p << ntohs(cli_addr->sin_port);
	_port = p.str();
	_req.init_tmpfile();
	_res.init_tmpfile();
}

void    Client::req_to_res(void)
{
    get_res().set_method(get_req().get_method());
	get_res().set_status_code(get_req().get_status_code());
	get_res().set_connection(get_req().get_connection());	
    if (get_res().get_method() == -1 && (get_res().get_status_code() == 0 || get_res().get_status_code() == 200))
	{
	   get_req().set_status_code(400);
 	   get_res().set_status_code(400);
	}
	else if (get_res().get_method() == 0 && (get_res().get_status_code() == 0 ||get_res().get_status_code() == 200))
	{
	   get_req().set_status_code(501);
 	   get_res().set_status_code(501);
	}
	if (get_req().get_status_code() == 0)
		get_res().set_status_code(200);
	get_res().set_upload(get_req().get_upload_ok());
	get_res().set_uri(get_req().get_uri());
	get_res().set_cgi(get_req().get_cgi());
}

int 			Client::get_fd(void) const { return (_fd); }
int 			Client::get_fullreq(void) const { return (_fullreq); }
std::string 	Client::get_ip(void) const { return (_ip); }
std::string 	Client::get_port(void) const { return (_port); }
Request 		&Client::get_req(void) { return (_req); }
Response 		&Client::get_res(void) { return (_res); }

void			Client::set_fullreq(int flag) { _fullreq = flag ; }

void	Client::print_request(void) { _req.print(); }
void	Client::print_response(void) { 
	
	std::cout << _PURPLE << "	Sending response to client [" << _fd << "]" << _END << std::endl;
	_res.print(); }

} // namespace WS
