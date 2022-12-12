#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "../Request/Request.hpp"
# include "../Response/Response.hpp"

# include <sys/socket.h>
# include <sys/types.h>
# include <unistd.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sstream>
# include <iostream>

namespace WS {

class Client {

	private:
	
		int 						_fd;
		int							_fullreq;
		std::string					_ip;
		std::string					_port;
	    WS::Request					_req;
	    WS::Response				_res;
		std::vector<std::string>	_cgi;
		Client();

	public:
		
		Client(Client const & other);
		Client(int cli_fd, sockaddr_in *cli_addr);
		Client &operator=(Client const & other);
	    ~Client(void);
	
	    void    		req_to_res(void);
	    
	    int				get_fd(void) const;
		int 			get_fullreq(void) const;
		std::string 	get_ip(void) const;
		std::string		get_port(void) const;
		WS::Request 	&get_req(void);
	    WS::Response	&get_res(void);

		void			set_fullreq(int flag);

		void			print_request(void);
		void			print_response(void);		
	
	}; // class Client

} // namespace WS

#endif