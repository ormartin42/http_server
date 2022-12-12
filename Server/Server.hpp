#ifndef SERVER_HPP
# define SERVER_HPP

# define TIME_OUT -1
# define BUFF_SIZE 4096


# define _RED "\033[31m"
# define _BRED "\033[1;31m"
# define _GREEN "\033[32m"
# define _YELLOW "\033[33m"
# define _BLUE "\033[34m"
# define _PURPLE "\033[35m"
# define _BPURPLE "\033[1;35m"
# define _CYAN "\033[36m"
# define _BCYAN "\033[1;36m"
# define _END "\033[0m"

# define KILOBYTE 1000
# define MEGABYTE 1000000
# define GIGABYTE 1000000000

# define MAX_HEADER_SIZE 4096
# define MAX_BODY_SIZE 4000000000


# define COMPLETE 1


# include "../Config/Config.hpp"
# include "../Port/Port.hpp"
# include "../Autoindex/Autoindex.hpp"
# include "../Cgi/Cgi.hpp"

# include <sys/poll.h>
# include <vector>
# include <iostream>
# include <string>
# include <sstream>
# include <cstdlib>
# include <stdexcept>
# include <stdio.h>
# include <stdlib.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <dirent.h>

namespace WS
{

class Port;


class Server
{ 

public:

    /* TypeDef */
    typedef std::vector<Port *>						t_port_vect;
    typedef std::vector<struct pollfd>				t_pollfd_vect;
	typedef std::map<std::string, Elems*>			t_conf_struct;
	typedef std::map<std::string, Elems*>::iterator	it_conf;
    typedef t_port_vect::iterator					it_port;
    typedef t_pollfd_vect::iterator					it_poll;

    /* Constructorz */
	Server(void);

    Server(std::string const config_file);

    /* Destructorz */
    ~Server(void);

    /* Otherz */

	void			parseConfig(void);
	void			launchVirtualHosts(Port *p);

    void			addPort(Port *new_port);
    int				polling(void);
    void			add_polling(int fd);
    void			add_polling_out(int fd);
    void            remove_fd_poll(int fd);
    void			set_revent(void);
	void			close_connection(std::vector<Client>::iterator &clt, Port *port);
    //void			close_connection(Client *clt, Port *port);
	void			close_all_connections(Port *port);
	void			clean_closed_clients(void);
	void			reset_revents(void);
	void			accept_incoming_connections(Port *port);

    void            bridge_req_res(Client *cli, Elems *el);
    ParsingAttributes   conf_to_res(Client *cli, Elems *el);
    int	            open_good_file(Client *cli, Elems *el, ParsingAttributes pa);
    std::string     extension_type(Client *cli);
    int            process_response(Client *cli);
    int             process_response_cgi(Client *cli);

    int		        receive(Client *cli, std::vector<std::string> cgi_ext);
    int		        snd(Client *cli);
    void            reset_post_response(std::vector<Client>::iterator &cli, Port *port);
    void            remove_req_cgi(Client *cli);


    /* Getterz */
    Elems           *get_elems(Client *cli, Port *port);
	t_port_vect		&get_port_vect(void);
    t_pollfd_vect	&get_poll_vect(void);
	int				&get_client_pollfd(WS::Client &clt);
	short 			&get_client_event(WS::Client &clt);
	short 			&get_client_revent(WS::Client &clt);
    short 			&get_fd_revent(int fd);
    short           &get_fd_event(int fd);
    int 			get_run(void); 
    char            *get_error_page(int statuscode, ParsingAttributes pa);
	int				get_port_vect_size(void);
	std::vector<std::string> get_cgi_extensions(int port);
	int             is_in_pollvect(int fd);
    std::string     get_revent_string(int revent);
    void	        check_fd(int ret);

    /* Setterz */
    void 			set_run(int n);

private:
	Config			_config;
	t_conf_struct	_c;
	t_port_vect		_portvect;
    t_pollfd_vect	_pollvect;
    int				_run;
	bool			_cfds;
    short           _zero;
    int             _zzero;

}; // class Server

} // namespace WS


#endif