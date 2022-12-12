#ifndef PORT_HPP
# define PORT_HPP

# define BACK_LOG 10

# include "../Client/Client.hpp"

# include <netinet/in.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <fcntl.h>
# include <cstring>
# include <vector>
# include <cstdio>
# include <errno.h>

namespace WS {

	class Client;

	class Port {

		public:

		    typedef	std::vector<Client>			t_client_vect;
			typedef t_client_vect::iterator		it_clt;

			Port();
			Port(int r_port);
			Port(Port const & other);
			~Port(void);
			
			Port &operator=(Port const & other);

			void				init_port(std::string const port_nr, std::vector<std::string> ports, std::vector<std::string> ip);
		    int 				connect_port(void);
		    int 				binding(void);
		    int 				listening(void);
		    int 				accept_client(void);


    		int 				get_port(void);
    		int 				get_listenfd(void);
    		struct sockaddr_in	get_port_address(void);
    		t_client_vect		&get_clients_vect(void);
    		Client				&get_client(int cli_fd);
			void				set_right_ip(void);

		private:
    		
			std::string			_port;
			std::string			_ip;
			int 				_fd;
 			struct sockaddr_in	_port_address;
			t_client_vect 		_clients;
			std::vector<std::string> _port_vect;
			std::vector<std::string> _ip_vect;


}; // class Port

} // namespace WS

#endif