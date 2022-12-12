#include "./Webserv.hpp"


#include <unistd.h>
#include <cstring>
#include <stdexcept>
#include <csignal>

using namespace WS;




void	handle_signal(int signum)
{
	(void)signum;
	std::cerr << _RED << "\r...Closing Server... " << _END << std::endl;
	sleep(1);
	throw std::invalid_argument("signal shutdown");
}

void	init_signal(void)
{
	std::signal(SIGINT, handle_signal);
	std::signal(SIGQUIT, handle_signal);
	std::signal(SIGPIPE, handle_signal);
}

int main(int ac, char **av)
{
	int ret = 0;
	std::string conf;

	init_signal();

	if (ac == 2)
		conf = av[1];
	else if (ac == 1)
		conf = "./conf/default.conf";
	else
	{
        std::cout << _RED << "Usage : \'./webserv\' or \'./webserv <configuration file>\'" << _END << std::endl;
		return (1);
	}

	Server ws(conf);

	try
	{
		ws.parseConfig();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}

	Port	p[ws.get_port_vect_size()];
	
	try
	{
		ws.launchVirtualHosts(p);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}
	
	try
	{
    	while (ws.get_run())
    	{
			deb:
			ws.clean_closed_clients();
			ws.reset_revents();

			int poll = ws.polling();
			switch (poll)
			{
				case 0:
					perror("Timeout : poll() timed out");
					ws.close_all_connections(p);
					ws.set_run(0);
					break;
				case -1:
					perror("Error: poll() failed");
					ws.close_all_connections(p);
					ws.set_run(0);
					break;
				default:
				{
					// ws.check_fd(poll);
					for (Server::it_port itprt = ws.get_port_vect().begin(); itprt != ws.get_port_vect().end(); itprt++) 
					{
						if (ws.get_fd_revent((*itprt)->get_listenfd()) & POLLIN)
							ws.accept_incoming_connections(*itprt);
					}
					for (Server::it_port itprt = ws.get_port_vect().begin(); itprt != ws.get_port_vect().end(); itprt++) 
					{
						for (Port::it_clt itclt = (*itprt)->get_clients_vect().begin(); itclt != (*itprt)->get_clients_vect().end();) 
						{
							// LE CLIENT A N'A TRIGGER AUCUN REVENT ON PASSE AU SUIVANT
							if (ws.get_client_revent(*itclt) == 0)
							{
								if ((ws.get_fd_revent(itclt->get_req().get_tmpfd()) & POLLOUT) && itclt->get_fullreq() == 1)
								{
									itclt->get_req().fill_tmpfile();
									rewind(itclt->get_req().get_tmpdata());
									itclt->print_request();
									ws.get_fd_event(itclt->get_req().get_tmpfd()) = POLLIN;
									ws.get_client_event(*itclt) = POLLOUT;

								}
								itclt++;
								continue;
							}
							// LE CLIENT A TRIGGER UN REVENT D'ERREUR ON CLOSE LA CONNECTION
							else if (ws.get_client_revent(*itclt) & POLLERR || ws.get_client_revent(*itclt) & POLLRDHUP)
							{
								std::cout << _BRED "Client connection closed " << _RED << "(POLLERR || POLLRDHUP) " << _BRED << "[" << itclt->get_fd() << "]" << _END << std::endl;
								ws.close_connection(itclt, p);
							}
							// LE CLIENT A TRIGGER LE REVENT DE LECTURE LE SOCKET EST PRET A ETRE LU
							else if (ws.get_client_revent(*itclt) & POLLIN)
							{
								std::cout << _CYAN << "	Client is readable	[" << itclt->get_fd() << "]" << _END << std::endl;
								if ((ret = ws.receive(&(*itclt), ws.get_cgi_extensions((*itprt)->get_port()))) == 1)
								{
									std::cout << _BRED "Client connection closed " << _RED << "(receive())" << _BRED << " [" << itclt->get_fd() << "]" << _END << std::endl;
									ws.close_connection(itclt, p);
									break;
								}
								if (itclt->get_req().get_cgi() == 1 && itclt->get_res().get_cgidata() == NULL)
								{
									// SI CGI MODE ON ON INIT LE CGIFILE ET ON L AJOUTE A LA LISTE MONITORING DE POLL
									itclt->get_res().init_cgifile();
									ws.add_polling_out(itclt->get_res().get_cgifd());
								}
								itclt++;
							}
							// LE CLIENT A TRIGGER LE REVENT D'ECRITURE
							else if (ws.get_client_revent(*itclt) & POLLOUT)
							{
								// LE TMP_FILE_REQUEST EST PRET POUR LECTURE ON PARSE LE HEADER
								if (ws.get_fd_revent(itclt->get_req().get_tmpfd()) & POLLIN)
								{
									// SOIT CGI OFF SOIT ON S ASSURE QUE LE CGI FILE EST PRET POUR ECRITURE
									if (itclt->get_req().get_cgi() == 0 || (itclt->get_req().get_cgi() == 1 && (ws.get_fd_revent(itclt->get_res().get_cgifd()) & POLLOUT)))
									{
										rewind(itclt->get_req().get_tmpdata());
										itclt->get_req().parse_header();
										Elems *el;
										el = ws.get_elems(&(*itclt), *itprt);
										ws.bridge_req_res(&(*itclt), el);
									}
								}
								// LE TMP_FILE_RESPONSE EST PRET POUR ECRITURE
								else if (ws.get_fd_revent(itclt->get_res().get_tmpfd()) & POLLOUT)
								{
									// MODE CGI ET LE CGI FILE EST PRET POUR LECTURE
									if (itclt->get_res().get_cgi() == 1 && (ws.get_fd_revent(itclt->get_res().get_cgifd()) & POLLIN))
									{
										ws.process_response_cgi(&(*itclt));
										ws.remove_req_cgi(&(*itclt));
									}
									// LE FILE EST PRET POUR LECTURE POUR GET / PRET POUR ECRITURE POUR UN POST
									else if ((itclt->get_res().get_location() != "" || itclt->get_res().get_autoindex() != "") 
											|| itclt->get_res().get_method() == DELETE 
											|| (itclt->get_res().get_method() == GET && (ws.get_fd_revent(itclt->get_res().get_file_fd()) & POLLIN)) 
											|| (itclt->get_res().get_method() == POST && (ws.get_fd_revent(itclt->get_res().get_file_fd()) & POLLOUT)))
									{
										ws.process_response(&(*itclt));
										if (itclt->get_res().get_file_fd() != -1 && ws.is_in_pollvect(itclt->get_res().get_file_fd()) == 1)
										{
											ws.remove_fd_poll(itclt->get_res().get_file_fd());
											if (itclt->get_res().get_location() == "" && itclt->get_res().get_autoindex() == "")
											{
												close(itclt->get_res().get_file_fd());
												itclt->get_res().set_file_fd(-1);
											}
										}
									}
									ws.get_fd_event(itclt->get_res().get_tmpfd()) = POLLIN;
									if (itclt->get_res().get_tmpdata())
										rewind(itclt->get_res().get_tmpdata());
								}
								// LE TMP_FILE_RESPONSE EST PRET POUR LECTURE ON L ECRIT DANS LE SOCKET PUIS ON SEND
								else if (ws.get_fd_revent(itclt->get_res().get_tmpfd()) & POLLIN)
								{
									if (ws.snd(&(*itclt)) == 1)
									{
										std::cout << _BRED << "Client connection closed " << _RED << "(send() error) " << _BRED << " [" << itclt->get_fd() << "]" << _END << std::endl;
										ws.close_connection(itclt, p);
									}
									itclt->print_response();
									ws.reset_post_response(itclt, p);
									goto deb;
								}
								itclt++;
							}
							else
								itclt++;
						}
					}
				}
			}
		}
		ws.close_all_connections(p);
		return (0);
	}
	catch (const std::exception &e)
	{
		ws.close_all_connections(p);
		//return (0);
	}
	return (0);
}