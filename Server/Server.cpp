#include "./Server.hpp"

namespace WS
{

/* Constructorz */
Server::Server(void) : _config(Config()), _cfds(0) 
{
    //std::cout << _YELLOW << "...Creating Server..." << _END << std::endl;
    _run = 1;
	_zero = 0;
	_zzero = 0;
}

Server::Server(std::string const config_file) : _config(Config(config_file)), _cfds(0)
{
    //std::cout << _YELLOW << "...Creating Server..." << _END << std::endl;
	_run = 1;
	_zero = 0;
	_zzero = 0;
}

/* Desctructorz */
Server::~Server(void) { }

// ASSIGNMENT OPERATOR ?

/* Otherz */
void Server::parseConfig(void)
{
	std::cout << _YELLOW << "...Parsing Configuration File..." << _END << std::endl;
	_config.launch_parsing();
	//_config.print();
	_c =  _config.getConfigStruct();
	std::cout << _YELLOW << "...Parsing done !..." << _END << std::endl;
    std::cout << _YELLOW << "...Creating Server..." << _END << std::endl;
}

void Server::launchVirtualHosts(Port *port)
{
	int i = 0;
	it_conf it;

	for (it = _c.begin(); it != _c.end(); it++, i++)
	{
		if (it->first.find("-") != std::string::npos)
			continue;
		port[i].init_port(it->first, it->second->pa.ports, it->second->pa.ip);	
		addPort(&port[i]);
		add_polling(port[i].get_listenfd());
	}
}

void Server::addPort(Port *new_port)
{
	this->_portvect.push_back(new_port);
}

int Server::polling(void)
{
	return (poll(&(*_pollvect.begin()), _pollvect.size(), TIME_OUT));
}

void Server::add_polling(int fd)
{
    struct pollfd n_elem;
	memset(&n_elem, 0, sizeof(n_elem));
	n_elem.fd = fd;
	n_elem.events = POLLIN | POLLRDHUP | POLLERR;
	n_elem.revents = 0;
	_pollvect.push_back(n_elem);
}

void Server::add_polling_out(int fd)
{
    struct pollfd n_elem;
	memset(&n_elem, 0, sizeof(n_elem));
	n_elem.fd = fd;
	n_elem.events = POLLOUT | POLLRDHUP | POLLERR;
	n_elem.revents = 0;
	_pollvect.push_back(n_elem);
}

void Server::remove_fd_poll(int fd)
{
	for (it_poll it = _pollvect.begin(); it != _pollvect.end(); )
	{
		if ((*it).fd == fd)
			it = _pollvect.erase(it);
		else
			it++;
	}
}

void Server::close_connection(WS::Port::it_clt &clt, Port *port)
{
	int i = 0;
	this->get_client_revent(*clt) = 0;
	this->get_client_pollfd(*clt) = -1;

	if (clt->get_req().get_tmpfd() != -1 && is_in_pollvect(clt->get_req().get_tmpfd()) != 0)
		remove_fd_poll(clt->get_req().get_tmpfd());
	if (clt->get_res().get_tmpfd() != -1 && is_in_pollvect(clt->get_res().get_tmpfd()) != 0)
		remove_fd_poll(clt->get_res().get_tmpfd());
	if (clt->get_res().get_cgifd() != -1 && is_in_pollvect(clt->get_res().get_cgifd()) != 0)
		remove_fd_poll(clt->get_res().get_cgifd());
	if (clt->get_fd() >= 0)
		close(clt->get_fd());
	if (clt->get_req().get_tmpdata())
		fclose(clt->get_req().get_tmpdata());
	if (clt->get_res().get_tmpdata())
		fclose(clt->get_res().get_tmpdata());
	if (clt->get_res().get_cgidata())
		fclose(clt->get_res().get_cgidata());

	if (port->get_clients_vect().size() > 0)
	{
		for (std::vector<Client>::iterator it = port->get_clients_vect().begin(); it != port->get_clients_vect().end(); it++)
		{
			if (it != clt)
				++i;
			else
				break;
		}
		port->get_clients_vect().erase(port->get_clients_vect().begin() + i);
	}
	_cfds = 1;
}

void Server::close_all_connections(Port *port)
{
	if (port->get_clients_vect().size() > 0)
	{
		for (std::vector<Client>::iterator it = port->get_clients_vect().begin(); it != port->get_clients_vect().end(); it++)
		{
			std::cout << _BRED "	Client connection closed (closing server)	[" << it->get_fd() << "]" << _END << std::endl;
			if (it->get_fd() >= 0)
				close(it->get_fd());
			if (it->get_req().get_tmpdata())
				fclose(it->get_req().get_tmpdata());
			if (it->get_res().get_tmpdata())
				fclose(it->get_res().get_tmpdata());
			if (it->get_res().get_cgidata())
				fclose(it->get_res().get_cgidata());
		}
		port->get_clients_vect().erase(port->get_clients_vect().begin(), port->get_clients_vect().end());
	}

	for (it_poll itp = _pollvect.begin(); itp != _pollvect.end(); itp++)
	{
		if (itp->fd != -1)
			close(itp->fd);
	}
	
}

void Server::clean_closed_clients(void)
{
	if (_cfds)
	{
		for (it_poll it = _pollvect.begin(); it != _pollvect.end(); )
		{
			if ((*it).fd == -1)
				it = _pollvect.erase(it);
			else
				it++;
		}
	}
	_cfds = 0;
}

void Server::reset_revents(void)
{
	for (it_poll it = _pollvect.begin(); it != _pollvect.end(); it++)
		(*it).revents = 0;
}

void Server::accept_incoming_connections(Port *port)
{
	int client_fd = -1;

	client_fd = port->accept_client();
	if (client_fd != -1)
	{
			std::cout << _BCYAN << "Client connected on port	[" << client_fd << "]" << _END << std::endl;
			this->add_polling(client_fd);
	}
}

void Server::bridge_req_res(Client *cli, Elems *el)
{
	ParsingAttributes tmp_pa;

	cli->req_to_res();
	if (cli->get_res().get_status_code() == 200)
		tmp_pa = conf_to_res(cli, el);
	open_good_file(cli, el, tmp_pa);
	add_polling_out(cli->get_res().get_tmpfd());
	if (cli->get_res().get_cgi() == 1)
		get_fd_event(cli->get_res().get_cgifd()) = POLLIN;
	else if (cli->get_res().get_method() == GET)
		add_polling(cli->get_res().get_file_fd());
	else if (cli->get_res().get_method() == POST)
		add_polling_out(cli->get_res().get_file_fd());
	get_fd_event(cli->get_req().get_tmpfd()) = POLLOUT;
	return ;
}

ParsingAttributes Server::conf_to_res(Client *cli, Elems *el)
{
	typedef std::map<std::string, ParsingAttributes>::iterator	it_map;
	typedef std::vector<std::string>::iterator				it_vect;
	ParsingAttributes tmpa = el->pa;
	std::string tmp_uri = "";
	std::string sp_uri = "";
	int flag = 0, test = 0;

	tmp_uri = cli->get_res().get_uri();
	if (tmp_uri.rfind("/") == (tmp_uri.size() - 1))
		tmp_uri.erase(tmp_uri.rfind("/"));
	for (; tmp_uri.size() > 1; tmp_uri.erase(tmp_uri.rfind("/")))
	{
		for (it_map it = el->locations.begin(); it != el->locations.end(); it++)
		{
			if (it->first == tmp_uri)
			{
				tmpa = it->second;
				flag = 1;
				break;
			}
		}
		if (flag == 1)
			break;
	}
	//std::cout << "TMPA " << tmpa.name << std::endl;
	cli->get_res().set_status_code(405);
	if (cli->get_res().get_max_body_size() > tmpa.get_body_size())
		cli->get_res().set_max_body_size(tmpa.get_body_size());
	for (it_vect it = tmpa.method.begin(); it != tmpa.method.end(); it++)
	{	
		if (*it == cli->get_res().get_method_str())
		{
			cli->get_res().set_status_code(200);
			break;
		}
	}
	if ((long long)cli->get_req().get_body().size() > cli->get_res().get_max_body_size())
		cli->get_res().set_status_code(413);
	test = tmpa.root.rfind("/");
	if (test == (int)(tmpa.root.size() - 1))
	{
		test = cli->get_res().get_uri().find("/");
		if (test == 0)
			sp_uri = tmpa.root + cli->get_res().get_uri().substr(1);
	}
	if (sp_uri != "")
		cli->get_res().set_uri(sp_uri);
	else
		cli->get_res().set_uri(tmpa.root + cli->get_res().get_uri());
	//std::cout << "URI CONF2RES : " << cli->get_res().get_uri() << std::endl;

	return (tmpa);
}

int	Server::open_good_file(Client *cli, Elems *el, ParsingAttributes pa)
{
	struct stat st;
	int file_fd = -2;
	std::string tmp_uri = "";

//	std::cout << "[ENTREE GOOD_FILE]" << std::endl;
	if (cli->get_res().get_status_code() != 200)
	{
		err:
		cli->get_res().set_method(1);
		cli->get_res().set_cgi(0);

//		std::cout << "[ENTREE ERROR] | ERR_CODE : " << cli->get_res().get_status_code() << std::endl;
		if (get_error_page(cli->get_res().get_status_code(), pa) == NULL)
		{
			tmp_uri = el->pa.root + "/error/basic_error.html";
			//std::cout << "[BASIC ERROR] tmp_uri : " << tmp_uri << std::endl;
		}
		else 
		{
			tmp_uri = el->pa.root + get_error_page(cli->get_res().get_status_code(), pa);
			//std::cout << "[ERROR PAGE FOUND] tmp_uri : " << tmp_uri << std::endl;

		}
		file_fd = open(tmp_uri.c_str(), O_RDONLY, 755);
		if (file_fd < 0)
		{
			tmp_uri = el->pa.root + "/error/basic_error.html";
			file_fd = open (tmp_uri.c_str(), O_RDONLY, 755);
			cli->get_res().set_file_fd(file_fd);
		}
		else
			cli->get_res().set_file_fd(file_fd);
		return (0);
	}
	if (pa.return_uri != "")
	{
		//std::cout << "[ENTREE RETURN]" << std::endl;
		if (cli->get_res().get_cgi() == 1)
			remove_req_cgi(cli);
		cli->get_res().set_status_code(pa.return_num);
		cli->get_res().set_location(pa.return_uri);
		return (0);
	}
	if (stat(cli->get_res().get_uri().c_str(), &st) != 0)
	{
		cli->get_res().set_status_code(404);
		goto err;
	}
	if (cli->get_res().get_cgi() == 1)
	{
		if (stat(cli->get_res().get_uri().c_str(), &st) != 0)
		{
			cli->get_res().set_status_code(404);
			goto err;
		}
		Cgi cgi(cli, el, pa);
		if (cli->get_res().get_status_code() != 200)
			goto err;
	}
	else if (cli->get_res().get_method() == 1)
	{
//		std::cout << "[ENTREE METHOD GET]" << std::endl;
		if (st.st_mode & S_IFDIR)
		{
			//std::cout << "DIRECTORY" << std::endl;
			std::string work_uri = cli->get_res().get_uri();
			if (work_uri[work_uri.size() - 1] != '/')
				work_uri += '/';
			if (pa.index.size() > 0)
			{
				//std::cout << "CAS A" << std::endl;
				tmp_uri = *(pa.index.begin());
				cli->get_res().set_uri(cli->get_res().get_uri() + '/' + tmp_uri);
				//std::cout << "URI : " << cli->get_res().get_uri() << std::endl;
			}
			else if (stat((work_uri + "index.html").c_str(), &st) == 0 && (st.st_mode & S_IFREG))
			{
				//std::cout << "CAS B" << std::endl;
				cli->get_res().set_uri(work_uri + "index.html");
			}
			else if (pa.autoindex == "on")
			{
				//std::cout << "CAS C" << std::endl;
				Autoindex autoindex(cli->get_res().get_uri());
				cli->get_res().set_autoindex(autoindex.getAutoindex());
				return (0);
			}
			else
			{
				//std::cout << "[CAS 403 FORBIDDEN]" << std::endl;
				cli->get_res().set_status_code(403);
				goto err;
			}
			file_fd = open(cli->get_res().get_uri().c_str(), O_RDONLY, 755);
			if (file_fd < 0)
			{
				perror("Error : file cant be opened");
				cli->get_res().set_status_code(500);
				goto err;
			}
			else
				cli->get_res().set_file_fd(file_fd);	
		}
		else if (st.st_mode & S_IFREG)
		{

			//std::cout << "[CAS REG_FILE OK]" << std::endl;
			file_fd = open(cli->get_res().get_uri().c_str(), O_RDONLY, 777);
			if (file_fd < 0)
			{
				perror("Error : file cant be opened");
				cli->get_res().set_status_code(500);
				goto err;
			}
			else
				cli->get_res().set_file_fd(file_fd);	
		}
	}
	else if (cli->get_res().get_method() == 2 && cli->get_res().get_cgi() == 0)
	{

	//	std::cout << "[CAS NO CGI + POST -> 403 FORBIDDEN]" << std::endl;
		cli->get_res().set_status_code(403);
		goto err;
	}
	else if (cli->get_res().get_method() == 3) 
	{
		struct stat buff;
		std::string path = cli->get_res().get_uri().c_str();
		std::string cmd = "rm -rf ";
		if ((stat(path.c_str(), &buff)) < 0)
		{
			cli->get_res().set_status_code(404);
			//std::cout << "NOT FOUND" << std::endl;
			goto err;
		}
		cmd += path;
		if ((system(cmd.c_str())) != 0)
		{
			cli->get_res().set_status_code(403);
			//std::cout << "FORBIDDEN" << std::endl;
			goto err;
		}
	}
	return (0);
}

std::string Server::extension_type(Client *cli)
{
	typedef std::map<std::string, std::string> t_extmap;
	typedef std::pair<std::string, std::string> t_extpair;
	std::string tmp;

	t_extmap emap;
	/*** Application ***/
	emap.insert(t_extpair(".pdf", "Content-Type: application/pdf\n"));
	emap.insert(t_extpair(".zip", "Content-Type: application/zip\n"));
	/*** Audio ***/
	emap.insert(t_extpair(".mpeg", "Content-Type: audio/mpeg\n"));
	emap.insert(t_extpair(".mp3", "Content-Type: audio/mpeg\n"));
	emap.insert(t_extpair(".wav", "Content-Type: audio/x-wav\n"));
	/*** Image ***/
	emap.insert(t_extpair(".jpg", "Content-Type: image/jpg\n"));
	emap.insert(t_extpair(".gif", "Content-Type: image/gif\n"));
	emap.insert(t_extpair(".jpeg", "Content-Type: image/jpeg\n"));
	emap.insert(t_extpair(".tiff", "Content-Type: image/tiff\n"));
	/*** Video ***/
	emap.insert(t_extpair(".mpeg", "Content-Type: video/mpeg\n"));
	emap.insert(t_extpair(".mp4", "Content-Type: video/mp4\n"));
	emap.insert(t_extpair(".mov", "Content-Type: video/quicktime\n"));
	/*** Text ***/
	emap.insert(t_extpair(".css", "Content-Type: text/css\n"));
	emap.insert(t_extpair(".csv", "Content-Type: text/csv\n"));
	emap.insert(t_extpair(".html", "Content-Type: text/html\n"));
	emap.insert(t_extpair(".xml", "Content-Type: text/xml\n"));

	if (cli->get_res().get_status_code() == 200 && cli->get_res().get_autoindex() == "")
	{
		for (t_extmap::iterator itmap = emap.begin(); itmap != emap.end(); itmap++)
		{
			tmp = itmap->first;
			if (cli->get_res().get_uri().find(tmp) != std::string::npos)
			{
				//std::cout << "[FORMAT FOUND] : " << itmap->second << std::endl;
				return (itmap->second);
			}
		}
		//std::cout << "[FORMAT DEFAULT] : " << "Content-Type: text/plain\n" << std::endl;
		return ("Content-Type: text/plain\n");
	}
	else 
		return ("Content-Type: text/html\n");
}

int	Server::process_response_cgi(Client *cli)
{
	struct stat stating;
	std::string header_cgi, header;
	char *buf[BUFF_SIZE];
	int ret = 1;
	int retot = 0, toread = 0;

//	std::cout << "[ENTREE PROCESS RESPONSE CGI]" << std::endl;
	rewind(cli->get_res().get_cgidata());
	fstat(cli->get_res().get_cgifd(), &stating);
	/*** ON LIT LA PREMIERE LIGNE DU TMP FILE POUR SAVOIR LE TYPE DE REPONSE DU CGI***/
	while ((header_cgi.find(BODY_CRLF) == std::string::npos && header_cgi.find("\n\n") == std::string::npos) && ret > 0)
	{
		ret = read(cli->get_res().get_cgifd(), buf, BUFF_SIZE - 1);
		if (ret < 0)
		{
			perror("Error : failed to read requested file");
			cli->get_res().set_status_code(500);
			return (1);
		}
		std::string buf_str((const char *)buf, ret);
		header_cgi += buf_str;
	}

//	std::cout << "Header_cgi : " << header_cgi.substr(0, header_cgi.find(BODY_CRLF)) << std::endl;
	/*** CAS ERREUR A AMELIORER POUR EVITER LA REDIR***/
	if (header_cgi.find("Status: ") != std::string::npos)
	{
		//std::cout << "[CAS ERREUR]" << std::endl;
		header += "HTTP/1.1 301\r\n";
		header += "Connection: keep-alive\r\n";
		header += "Location : http://localhost/error/basic_error.html\r\n";
		header += "\r\n";
	}
	/*** CAS CLASSIQUE ON VA REPONDRE UNE PAGE ON CALCULE SA SIZE***/
	else if (header_cgi.find("Content-type:") != std::string::npos
				|| header_cgi.find("content-type:") != std::string::npos
				|| header_cgi.find("Content-Type:") != std::string::npos)
	{
		//std::cout << "[CAS CLASSIQUE]" << std::endl;
		ret = header_cgi.find(BODY_CRLF);
		if (ret == -1)
		{
			ret = header_cgi.find("\n\n");
			ret += 2;
		}
		else
			ret += 4;
		header += "HTTP/1.1 200 OK\r\n";
		header += "Connection: keep-alive\r\n";
		header += "Content-Length: " ;
		int result = stating.st_size - ret;
		std::ostringstream conv;
		conv << result;
		header += conv.str();
		toread = stating.st_size - ret;
		header += "\r\n";
	}
	/*** CAS REDIRECTION ***/
	else if (header_cgi.find("Location: ") != std::string::npos)
	{
		//std::cout << "[CAS REDIRECTION]" << std::endl;
		header += "HTTP/1.1 301 Moved Permanently\r\n";
		header += "Connection: keep-alive\r\n";
	}
	else
	{
		/*** CAS SPECIAL ERREUR INTERNE CGI***/
		if (cli->get_res().get_uri().find(".jpg") != std::string::npos)
				{
					header += "Content-Type: image/jpg\n";
					//std::cout << "JPG TROUVE" << std::endl;
				}
				else 
					header += "Content-Type: text/html\n";
		cli->get_res().set_status_code(500);
		perror("Error : process_response_cgi case unknown");
		return (1);
	}	
	/*** HEADERS PRET ON VA LES ECRIRE DANS LE RES.TMP_FILE***/
	rewind(cli->get_res().get_cgidata());
	cli->get_res().set_resp(header);
	ret = write(cli->get_res().get_tmpfd(), header.c_str(), header.size());
	if (ret < -1)
	{
		perror("Error : process_response_cgi failed to write in requested file");
		cli->get_res().set_status_code(500);
		return (1);
	}
	if (header_cgi.find("Status: ") == std::string::npos)
	{
		while (retot < toread)
		{
			ret = read(cli->get_res().get_cgifd(), buf, BUFF_SIZE);
			if (ret < -1)
			{
				perror("Error : process_response_cgi failed to read requested file");
				cli->get_res().set_status_code(500);
				return (1);
			}
			else if (ret == 0)
				break;
			ret = write(cli->get_res().get_tmpfd(), buf, ret);
			if (ret < -1)
			{
				perror("Error : process_response_cgi failed to write in requested file");
				cli->get_res().set_status_code(500);
				return (1);
			}
			retot += ret;
		}
	}
	fstat(cli->get_res().get_tmpfd(), &stating);
	cli->get_res().set_tmpsize(stating.st_size);
	return (0);
}

int Server::process_response(Client *cli)
{
	struct stat stating;
	int ret, retrd, retwt, tmp = 0;
	char buf[BUFF_SIZE];

	//std::cout << "[ENTREE PROCESS RESPONSE]" << std::endl;
	fstat(cli->get_res().get_file_fd(), &stating);
	std::string header = "HTTP/1.1 ";
	header += itoa(cli->get_res().get_status_code());
	header += " ";
	header += cli->get_res().get_code_string(cli->get_res().get_status_code());
	header += "\n";
	if (cli->get_res().get_method() == 1)
	{
			if (cli->get_res().get_location() == "")
			{
				header += extension_type(cli);
				header += "Content-Length: ";
				std::ostringstream conv;
				if (cli->get_res().get_autoindex() != "")
				{
					conv << cli->get_res().get_autoindex().size();
					header += conv.str();
				}
				else
				{
					conv << stating.st_size;
					header += conv.str(); 
				}
				header += "\n\n";
				if (cli->get_res().get_autoindex() != "")
					header += cli->get_res().get_autoindex();
			}
			else
			{
				header += "Location: " + cli->get_res().get_location();
				header += "\n\n";
			}
			cli->get_res().set_resp(header);
			ret = write(cli->get_res().get_tmpfd(), header.c_str(), header.size());
			if (ret < 0)
			{
				perror("Error : failed to write() header on res tmp file");
				cli->get_res().set_status_code(500);
				return (1);
			}
			if (cli->get_res().get_autoindex() == "" && cli->get_res().get_location() == "")
			{
				tmp = 0;
				while (tmp <= stating.st_size)
				{
					if (cli->get_res().get_autoindex() == "")
						retrd = read(cli->get_res().get_file_fd(), buf, BUFF_SIZE);
					if (retrd == -1)
					{
						perror("Error : failed to read requested file");
						cli->get_res().set_status_code(500);
						return (1);
					}
					else if (retrd == 0)
						break;
					retwt = write(cli->get_res().get_tmpfd(), buf, retrd);
					if (retwt < 0)
					{
						perror("Error : failed to write() file buffer in res tmp file");
						cli->get_res().set_status_code(500);
						return (1);
					}
					else if (retwt == 0)
						std::cout << _BLUE << "EOF REACHED ON file write buffer" << _END << std::endl;
					tmp += retwt; 
				}
			}
			cli->get_res().set_add_tmpsize(ret + tmp);
	}
	else if (cli->get_res().get_method() == DELETE)
	{
		header += "\n";
		ret = write(cli->get_res().get_tmpfd(), header.c_str(), header.size());
		if (ret < 0)
		{
			perror("Error : failed to write() header on res tmp file");
			cli->get_res().set_status_code(500);
			return (1);
		}
		cli->get_res().set_resp(header);
		cli->get_res().set_add_tmpsize(ret);
	}
	return (0);
}

int		Server::receive(Client *cli, std::vector<std::string> cgi_ext)
{
	char buffer[BUFF_SIZE];
	int ret;
	int full_req;

	memset(buffer, '\0', BUFF_SIZE);
	ret = recv(cli->get_fd(), buffer, BUFF_SIZE - 1, 0);
	buffer[ret] = '\0';
	if (ret < 0)
	{
		perror("Error : recv() failed");
		return (1);
	}
	if (ret == 1 || ret == 5) 
	{
		if ((std::atoi(cli->get_req().get_header().c_str()) == /*"\x03"*/ SIGINT) || cli->get_req().get_header() == "")
		{
			//std::cout << "RECEIVE CLOSED" << std::endl;
			return (1);
		}
	}
	full_req = cli->get_req().conc_request(buffer, ret, cgi_ext);
	//std::cout << "FULL REQ : " << full_req << std::endl;
	if (cli->get_req().is_req_tobig() == 1)
		cli->get_req().set_status_code(413);
	if (full_req == 1 || cli->get_req().is_req_tobig() == 1)
	{
		add_polling_out(cli->get_req().get_tmpfd());
		cli->set_fullreq(1);
		return (0) ;
	}
	return (0);
}

/*** LE TMP_FILE_RESPONSE EST PRET POUR LECTURE ON L ECRIT DANS LE SOCKET PUIS ON SEND ***/
int Server::snd(Client *cli)
{
	char buffer[BUFF_SIZE];
	int ret = 0;
	int sendret = 0;

//	std::cout << "[RESPONSE]" << std::endl;
	while (ret < cli->get_res().get_tmpsize())
	{
		memset(buffer, '\0', BUFF_SIZE);
		ret = read(cli->get_res().get_tmpfd(), buffer, BUFF_SIZE);
		if (ret == -1)
		{
			perror("Error : failed to read requested file");
			cli->get_res().set_status_code(500);
			return (1);
		}
		else if (ret == 0)
			break;
		sendret = send(cli->get_fd(), buffer, ret, 0);
		if (sendret == -1)
		{
			perror("Error : failed to read requested file");
			cli->get_res().set_status_code(500);
			return (1);
		}
		if (cli->get_res().get_uri().find(".jpg") == std::string::npos)
		{
			//std::cout << buffer << std::endl;
		}
	}
	return (0);
}

void Server::reset_post_response(std::vector<Client>::iterator &cli, Port *port)
{
	if (cli->get_res().get_connection() == 1)
	{
		get_client_revent(*cli) = 0;
		get_client_event(*cli) = POLLIN | POLLRDHUP | POLLERR;
		remove_fd_poll(cli->get_res().get_tmpfd());
		remove_fd_poll(cli->get_req().get_tmpfd());
		cli->get_req().reset_req();
		cli->get_res().reset_res();
		cli->set_fullreq(0);
	}
	else
	{
		std::cout << _BRED << "Client connection closed (end of response)		[" << cli->get_fd() << "]" << _END << std::endl;
		close_connection(cli, port);
		clean_closed_clients();
	}
}

void Server::remove_req_cgi(Client *cli)
{
	if (cli->get_res().get_cgifd() != -1)
	{
		remove_fd_poll(cli->get_res().get_cgifd());
		fclose(cli->get_res().get_cgidata());
	}
	cli->get_res().set_cgidata(NULL);
	cli->get_res().set_cgifd(-1);
	cli->get_res().set_cgi(0);
	cli->get_req().set_cgi(0);
}

/* Getterz */
void	Server::check_fd(int ret)
{
	int clinb = 0;
	std::cout << "[POLL RET] : " << ret << std::endl;
	std::cout << "[POLLVECT FDS]" << std::endl;
	for (it_poll itp = _pollvect.begin(); itp != _pollvect.end(); itp++)
		std::cout << itp->fd << " | ";
	std::cout << std::endl << std::endl;
	for (it_port it = _portvect.begin(); it != _portvect.end(); it++)
	{
		std::cout << "[PORT " << (*it)->get_port() << "] : ";
		std::cout << "FD : " << (*it)->get_listenfd();
		if (get_fd_revent((*it)->get_listenfd()) != 0)
			std::cout << " | REVENT : " << get_revent_string(get_fd_revent((*it)->get_listenfd()));
		std::cout << std::endl;
		clinb = 0;
		std::vector<Client> client_vect = (*it)->get_clients_vect();
		for (std::vector<Client>::iterator itc = client_vect.begin(); itc != client_vect.end(); itc++)
		{
			std::cout << "	[CLIENT " << clinb << "] : ";
			std::cout << "FD : " << itc->get_fd();
			if (get_fd_revent(itc->get_fd()) != 0)
				std::cout << " | REVENT : " << get_revent_string(get_fd_revent(itc->get_fd()));
			std::cout << std::endl;

			std::cout << "		[REQUEST] : ";
			std::cout << "TMPFD : " << itc->get_req().get_tmpfd();
			if (get_fd_revent(itc->get_req().get_tmpfd()) != 0)
				std::cout << " | REVENT : " << get_revent_string(get_fd_revent(itc->get_req().get_tmpfd()));
			std::cout << std::endl;

			std::cout << "		[RESPONSE] : ";
			std::cout << "TMPFD : " << itc->get_res().get_tmpfd();
			if (get_fd_revent(itc->get_res().get_tmpfd()) != 0)
				std::cout << " | REVENT : " << get_revent_string(get_fd_revent(itc->get_res().get_tmpfd()));
			std::cout << std::endl;
			std::cout << "					 FILEFD : " << itc->get_res().get_file_fd();
			if (get_fd_revent(itc->get_res().get_file_fd()) != 0)
				std::cout << " | REVENT : " << get_revent_string(get_fd_revent(itc->get_res().get_file_fd()));
			std::cout << std::endl;
			std::cout << "					 CGIFD : " << itc->get_res().get_cgifd();
			if (get_fd_revent(itc->get_res().get_cgifd()) != 0)
				std::cout << " | REVENT : " << get_revent_string(get_fd_revent(itc->get_res().get_cgifd()));
			std::cout << std::endl;
			clinb++;
		}
		std::cout << "----------------------------------------------------------------" << std::endl;
	}
}

std::string Server::get_revent_string(int revent)
{
	if (revent == POLLIN)
		return ("POLLIN");
	else if (revent == POLLOUT)
		return ("POLLOUT");
	else if (revent == POLLERR)
		return ("POLLERR");
	else if (revent == POLLRDHUP)
		return ("POLLRDHUP");
	else if (revent == POLLNVAL)
		return ("POLLNVAL");
	else if (revent == EBADF)
		return ("EBADF");
	else if (revent == EFAULT)
		return ("EFAULT");
	else if (revent == EINTR)
		return ("EINTR");
	else if (revent == EINVAL)
		return ("EINVAL");
	else if (revent == ENOMEM)
		return ("ENOMEM");
	else if (revent == (POLLIN | POLLRDHUP))
		return ("POLLIN | POLLRDHUP");
	else
	{
		std::cout << "UNKNOWN REVENT : " << revent << std::endl;
		return ("UNKNOWN REVENT");
	}
}

int Server::is_in_pollvect(int fd)
{
	if (fd == -1)
		return (0);
	for (it_poll it = get_poll_vect().begin(); it != get_poll_vect().end(); it++)
	{
		if ((*it).fd == fd)
			return (1);
	}
	return (0);
}

Elems *Server::get_elems(Client *cli, Port *port)
{
	it_conf itc = _c.begin();
	it_conf itce = _c.end();
	Elems *tmp = NULL;
	int ct = 0;
	int tmp_port = 0;

	
	while (itc != itce)
	{
		if (itc->first.find("-") != std::string::npos)
			tmp_port = atoi((itc->first.substr(0, itc->first.find("-"))).c_str());
		else
			tmp_port = atoi(itc->first.c_str());
		if (tmp_port == port->get_port())
		{
			if (ct == 0)
				tmp = itc->second;
			std::vector<std::string>::iterator it_vect = itc->second->pa.names.begin();
			std::vector<std::string>::iterator it_vecte = itc->second->pa.names.end();
			while (it_vect != it_vecte)
			{
				if (cli->get_req().get_host() == *it_vect)
					return (itc->second);
				it_vect++;
			}
			ct++;
		}
		itc++;
	}
	if (tmp != NULL)
		return (tmp);
	return (NULL);
}


Server::t_port_vect &Server::get_port_vect(void)
{
    return (_portvect);
}

Server::t_pollfd_vect &Server::get_poll_vect(void)
{
    return (_pollvect);
}

int &Server::get_client_pollfd(WS::Client &clt)
{
	if (get_poll_vect().size() != 0)
	{
		for (it_poll it = get_poll_vect().begin(); it != get_poll_vect().end(); it++)
		{
			if ((*it).fd == clt.get_fd())
				return ((*it).fd);
		}
	}
	return (_zzero); 
}

short &Server::get_client_event(WS::Client &clt)
{
	if (get_poll_vect().size() != 0)
	{
		for (it_poll it = get_poll_vect().begin(); it != get_poll_vect().end(); it++)
		{
			if ((*it).fd == clt.get_fd())
				return ((*it).events);
		}
	}
	return (_zero);
}

short &Server::get_client_revent(WS::Client &clt)
{
	if (get_poll_vect().size() != 0)
	{
		for (it_poll it = get_poll_vect().begin(); it != get_poll_vect().end(); it++)
		{
			if ((*it).fd == clt.get_fd())
				return ((*it).revents);
		}
	}
	return (_zero);
}

short &Server::get_fd_event(int fd)
{
	if (get_poll_vect().size() != 0)
	{
		for (it_poll it = get_poll_vect().begin(); it != get_poll_vect().end(); it++)
		{
			if ((*it).fd == fd)
				return ((*it).events);
		}
	}
	return (_zero);
}

short &Server::get_fd_revent(int fd)
{
	if (get_poll_vect().size() != 0)
	{
		for (it_poll it = get_poll_vect().begin(); it != get_poll_vect().end(); it++)
		{
			if ((*it).fd == fd)
				return ((*it).revents);
		}
	}
	return (_zero);
}

int	Server::get_port_vect_size(void)
{
	return (_c.size());
}

int Server::get_run(void)
{
    return (_run);
}

std::vector<std::string> Server::get_cgi_extensions(int port)
{
	t_conf_struct::iterator it;
	for (it = _c.begin(); it != _c.end(); it++)
	{
		if (std::atoi(it->first.c_str()) == port)
			break;
	}
	std::map<std::string, std::string> cgi = (it->second)->pa.cgi;
	std::vector<std::string> ext_vect;
	if (cgi.size() > 0)
	{
		for (std::map<std::string, std::string>::iterator it2 = cgi.begin(); it2 != cgi.end(); it2++)
			ext_vect.push_back(it2->first);	
	}
	return (ext_vect);
}

char *Server::get_error_page(int statuscode, ParsingAttributes pa)
{
	typedef std::map<int, std::string>::iterator			it_map;

	for (it_map it = pa.error_page.begin(); it != pa.error_page.end(); it++)
	{
		if (statuscode == it->first)
			return ((char *)it->second.c_str());
	}
	return (NULL);
}

/* Setterz */
void Server::set_run(int n)
{
    _run = n;
}

std::string	itoa(int status_code)
{
	std::string ret1;
	std::string ret2;
	std::string ret3;
	ret3 = status_code % 10 + '0';
	status_code /= 10;
	ret2 = status_code % 10 + '0';
	status_code /= 10;
	ret1 = status_code % 10 + '0';
	return (ret1 + ret2 + ret3);
}


} // namespace WS
