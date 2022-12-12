#include "./Cgi.hpp"

namespace WS {

Cgi::Cgi() { }
Cgi::Cgi(Cgi const & other) { *this = other; }
Cgi &Cgi::operator=(Cgi const & other ) {
	if (this != &other)
	{
		this->_cli = other._cli;
		this->_serv_conf = other._serv_conf;
		this->_loc_conf = other._loc_conf;
		this->_env = other._env;
		this->_arg[3] = other._arg[3];
		this->_path_bin_cgi = other._path_bin_cgi;
		this->_extension = other._extension;
		this->_path_file = other._path_file;
		this->_loc_flag = other._loc_flag;
		this->_map = other._map;
	}
	return (*this);
}

Cgi::~Cgi()
{
	if (_arg[0])
		free (_arg[0]);
	if (_arg[1])
		free (_arg[1]);
	if (_env)
	{
		for (int i = 0; _env[i] != NULL; ++i)
			free(_env[i]);
		delete[](_env);
	}
}

Cgi::Cgi(Client *cli, Elems *el, ParsingAttributes pa) : _loc_flag(0)
{
	_cli = cli;
	_serv_conf = el->pa;
	_loc_conf = pa;
	_cgi_ext = cli->get_req().get_cgi_ext();

	if (!_loc_conf.name.empty())
		_loc_flag = 1;

	init_attributes();
	init_arg();
	init_env();
	exec_cgi();
}


void	Cgi::init_attributes(void)
{
	std::map<std::string, std::string>::iterator it;
	for (it = _serv_conf.cgi.begin(); it != _serv_conf.cgi.end(); it++)
	{
		if (it->first == _cgi_ext)
			break;
	}
	_path_bin_cgi = it->second;
	_extension = _serv_conf.cgi.begin()->first;
	_path_file = _cli->get_res().get_uri();
}

void    Cgi::init_arg()
{
    _arg[0] = strdup(_path_bin_cgi.c_str());
	_arg[1] = strdup(_path_file.c_str());
	_arg[2] = NULL;
}

void    Cgi::init_env()
{
	init_map();
	
	int i = 0;
	_env = new char*[_map.size() + 2];
	for (t_map::iterator it = _map.begin(); it != _map.end(); it++)
	{
		std::stringstream var;

		var << it->first << it->second;
		_env[i] = strdup((var.str()).c_str());
		_env[i][var.str().size()] = 0;
		++i;
	}
	_env[_map.size()] = 0;
}

void	Cgi::init_map()
{
	struct stat st;

	/* VARIABLES SERVEUR */
	_map["SERVER_SOFTWARE="] = "webserv/v_chadserv";
	_map["SERVER_NAME="] = *_serv_conf.names.begin();
	_map["GATEWAY_INTERFACE="] = "CGI/1.1"; 

	/* VARIABLES REQUETE */
	_map["SERVER_PROTOCOL="] = "HTTP/1.1"; 
	_map["SERVER_ADDR="] = *_serv_conf.ip.begin();
	//_map["SERVER_ADDR="] = "127.0.0.1";
	_map["SERVER_PORT="] = *_serv_conf.ports.begin();
	_map["REQUEST_METHOD="] = _cli->get_req().get_method_str();
	if ((_cli->get_req().get_content_type()) != "")
			_map["CONTENT_TYPE="] = _cli->get_req().get_content_type();

	fstat(_cli->get_req().get_tmpfd(), &st);
	std::stringstream out;
	out << st.st_size;

	if (_cli->get_req().get_method_str() == "POST")
			_map["CONTENT_LENGTH="] = out.str();
	_map["SCRIPT_NAME="] = _cli->get_req().get_uri();
	_map["SCRIPT_FILENAME="] = _path_file;
	_map["QUERY_STRING="] = _cli->get_req().get_query(); 
	_map["REMOTE_ADDR="] = _cli->get_ip();
	_map["REMOTE_PORT="] = _cli->get_port();
	_map["REQUEST_URI="] = _cli->get_req().get_uri();
	_map["DOCUMENT_URI="] = _cli->get_req().get_uri();
	if (!_loc_flag)
		_map["DOCUMENT_ROOT="] = _serv_conf.root;
	else 
		_map["DOCUMENT_ROOT="] = _loc_conf.root;
	_map["HTTPS="] = "";
	_map["REDIRECT_STATUS="] = "200";
	_map["PATH_INFO="] = _path_bin_cgi;

	// VARIABLES CLIENT
	_map["HTTP_HOST="] = _cli->get_req().get_host();
	_map["HTTP_CONNECTION="] = _cli->get_req().get_connection_str();
	_map["HTTP_SEC_CH_UA="] = _cli->get_req().get_sec_ch_ua();
	_map["HTTP_SEC_CH_UA_MOBILE="] = _cli->get_req().get_sec_ch_ua_mobile();
	_map["HTTP_SEC_CH_UA_PLATFORM="] = _cli->get_req().get_sec_ch_ua_platform();
	_map["HTTP_UPGRADE_INSECURE_REQUESTS="] = _cli->get_req().get_upgrade_insecure_requests();
	_map["HTTP_USER_AGENT="] = _cli->get_req().get_user_agent();
	_map["HTTP_ACCEPT="] = _cli->get_req().get_accept();
	_map["HTTP_SEC_FETCH_SITE="] = _cli->get_req().get_sec_fetch_site();
	_map["HTTP_SEC_FETCH_MODE="] = _cli->get_req().get_sec_fetch_mode();
	_map["HTTP_SEC_FETCH_USER="] = _cli->get_req().get_sec_fetch_user();
	_map["HTTP_SEC_FETCH_DEST="] = _cli->get_req().get_sec_fetch_dest();
	_map["HTTP_ACCEPT_ENCODING="] = _cli->get_req().get_accept_encoding();
	_map["HTTP_ACCEPT_LANGUAGE="] = _cli->get_req().get_accept_language();
	_map["HTTP_COOKIE="] = _cli->get_req().get_cookie();
}

void    Cgi::exec_cgi()
{
	pid_t	pid;
	int		status;
	int		ret;
	struct stat st;
	int len_header; 

	fstat(_cli->get_req().get_tmpfd(), &st);

	//std::cout << "[TMP SIZE IN CGI] :" << st.st_size << std::endl;
	len_header = st.st_size - _cli->get_req().get_content_length(); 

	fseek(_cli->get_req().get_tmpdata(), len_header, SEEK_SET);
	pid = fork();
	if (pid == -1)
		return ;
	else if (pid == 0)
	{
		if (_cli->get_req().get_method_str() == "POST")
		{
			if ((ret = dup2(_cli->get_req().get_tmpfd(), STDIN_FILENO)) < 0)
			{
				perror("dup2()");
				exit(1);
			}
		}
		if ((ret = dup2(_cli->get_res().get_cgifd(), STDOUT_FILENO)) < 0)
		{
			perror("dup2()");
			exit(EXIT_FAILURE);
		}
		if (execve(_path_bin_cgi.c_str(), _arg, _env) == -1)
		{
			perror("execve()");	
			exit(1);
		}
		exit(0);
	}
	else
	{
		wait(&status);
		if (WIFEXITED(status))
		{
			int tmp = WEXITSTATUS(status);
			if (tmp == 1)
				_cli->get_res().set_status_code(500);
		}

	}
	return ;
}

}










