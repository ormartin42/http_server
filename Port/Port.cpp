#include "./Port.hpp"


namespace WS 
{

Port::Port() { }
Port::Port(int r_port) { _port = r_port; }
Port::Port(Port const & other) { *this = other; }
Port::~Port(void) { }
Port &Port::operator=(Port const & other) {
	if (this != &other)
	{
		this->_port = other._port;
		this->_fd = other._fd;
		this->_port_address = other._port_address;
		this->_clients = other._clients;
	}
	return (*this);
}

void Port::init_port(std::string const port_nbr, std::vector<std::string> ports, std::vector<std::string> ip)
{
    _port = port_nbr;
    _port_vect = ports;
    _ip_vect = ip;
    set_right_ip();
 
	if (connect_port() == -1 || binding() == -1 || listening() == -1)
        return;
}

int Port::connect_port(void)
{
    int on = 1;
    std::string error;

    if ((_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {  
        error = strerror(errno);
        std::cout << "Error: socket() failed: " << error << " (port: " << _port << ")" << std::endl;
        return (-1);
    }
    if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) < 0)
    {
        error = strerror(errno);
        std::cout << "Error: setsockopt() failed: " << error << " (port: " << _port << ")" << std::endl;
        close(_fd);
        return (-1);
    }
    if (fcntl(_fd, F_SETFL, O_NONBLOCK) < 0)
    {
        error = strerror(errno);
        std::cout << "Error: fnctl() failed: " << error << " (port: " << _port << ")" << std::endl;
        close(_fd);
        return (-1);
    }
    return (0);
}

int Port::binding(void)
{
    std::string error;

    memset((char *)&_port_address, 0, sizeof(_port_address));
    _port_address.sin_family = AF_INET;
    //_port_address.sin_addr.s_addr = htonl(INADDR_ANY);
    inet_pton(AF_INET, _ip.c_str(), &(_port_address.sin_addr.s_addr));
    _port_address.sin_port = htons(std::atoi(_port.c_str()));
    memset(_port_address.sin_zero, '\0', sizeof(_port_address.sin_zero));

    if (::bind(_fd, (struct sockaddr *)&_port_address, sizeof(_port_address)) < 0)
    {
        error = strerror(errno);
        std::cout << "Error: bind() failed: " << error << " (port: " << _port << ")" << std::endl;
        close(_fd);
        return (-1);
    }
    return (0);
}

int Port::listening(void)
{
    std::string error;

    if (::listen(_fd, BACK_LOG) < 0)
    {
        error = strerror(errno);
        std::cout << "Error: bind() failed: " << error << " (port: " << _port << ")" << std::endl;
        close(_fd);
        return (-1);
    }
    return (0);
}

int	Port::accept_client(void)
{
	int					new_socket = 0;
	struct sockaddr_in	cli_addr;
	socklen_t			addrlen;

	memset((char*)&cli_addr, 0, sizeof(struct sockaddr_in));
	cli_addr.sin_family = AF_INET;
	//cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    inet_pton(AF_INET, _ip.c_str(), &(_port_address.sin_addr.s_addr));
    _port_address.sin_addr.s_addr = htonl(INADDR_ANY);
	cli_addr.sin_port = htons(std::atoi(_port.c_str()));
	addrlen = sizeof(cli_addr);
	new_socket = accept(_fd, (struct sockaddr *)&cli_addr, (socklen_t*)&addrlen);
	if (new_socket < 0)
	{
		if (errno != EAGAIN)
		 	perror("Error : In accept()");
		return(-1);
	}
	Client newClient(new_socket, &cli_addr);
	_clients.push_back(newClient);
	return (new_socket);
}

int 				 Port::get_port(void) { return (std::atoi(_port.c_str())); }
int 				 Port::get_listenfd(void) { return (_fd); }
struct sockaddr_in 	 Port::get_port_address(void) { return (_port_address); }
Port::t_client_vect	&Port::get_clients_vect(void) { return (_clients); }
Client				&Port::get_client(int cli_fd)
{
    t_client_vect::iterator itclt;
    itclt = get_clients_vect().begin();
    for (; itclt != get_clients_vect().end(); itclt++)
    {
        if (itclt->get_fd() == cli_fd)
            return (*itclt);
    }
    return (*get_clients_vect().end());
}

void     Port::set_right_ip(void)
{
    int i = 0;

    for (std::vector<std::string>::iterator it = _port_vect.begin(); it != _port_vect.end();)
    {
        if (_port == *it)
            break;
        ++it;
        ++i;
    }

    _ip = _port_vect[i];
}

} // namespace WS
