#include "./Request.hpp"

namespace WS
{

/* CANON */

Request::Request(void) :
	_tmpdata(NULL),
	_tmpfd(-1),
    _tmp(""),
    _uri(""),
    _query(""),
    _accept(""),
    _host(""),
    _multipart_delim(""),
    _content_type(""),
	_accept_encoding(""),
	_accept_language(""),
	_sec_ch_ua(""),
	_sec_ch_ua_mobile(""),
	_sec_ch_ua_platform(""),
	_upgrade_insecure_requests(""),
	_user_agent(""),
	_sec_fetch_site(""),
	_sec_fetch_mode(""),
	_sec_fetch_user(""),
	_sec_fetch_dest(""),
	_cookie(""),
	_method_str(""),
	_connection_str(""),
    _cgi(0),
    _rqlen(0),
    _rqpos(0),
    _mult(0),
    _state(RCVG_HEADER),
    _connection(CLOSE),
    _body_reception_encoding(NOT_SPECIFIED),
    _content_length(0),
    _body_recv(0),
    _method(NOT_SPEC),
    _status_code(0),
    _continue(0),
    _upload_ok(0),
	_req_buff(""),
	_header(""),
	_body(""),
	_ret(0),
	_header_flag(0),
	_body_flag(0),
	_cgi_ext("")
{
	init_parsemap();
}

Request::Request(Request const &src) { *this = src; }

Request::~Request(void) { }

Request &Request::operator=(Request const &ref)
{
    if (this != &ref)
    {
    _tmpdata = ref._tmpdata;
    _tmpfd = ref._tmpfd;
    _tmp = ref._tmp;
    _query = ref._query;
    _uri = ref._uri;
    _accept = ref._accept;
    _host = ref._host;
    _multipart_delim = ref._multipart_delim;
    _rqlen = ref._rqlen;
    _rqpos = ref._rqpos;
    _state = ref._state;
    _connection = ref._connection;
    _body_reception_encoding = ref._body_reception_encoding;
    _content_length = ref._content_length;
    _body_recv = ref._body_recv;
    _method = ref._method;
    _status_code = ref._status_code;
    _continue = ref._continue;
    _cgi = ref._cgi;
    _mult = ref._mult;
    _upload_ok = ref._upload_ok;
	_content_type = ref._content_type,
	_accept_encoding = ref._accept_encoding,
	_accept_language = ref._accept_language,
	_sec_ch_ua = ref._sec_ch_ua;
	_sec_ch_ua_mobile = ref._sec_ch_ua_mobile;
	_sec_ch_ua_platform = ref._sec_ch_ua_platform;
	_upgrade_insecure_requests = ref._upgrade_insecure_requests;
	_user_agent = ref._user_agent;
	_sec_fetch_site = ref._sec_fetch_site;
	_sec_fetch_mode = ref._sec_fetch_mode;
	_sec_fetch_user = ref._sec_fetch_user;
	_sec_fetch_dest = ref._sec_fetch_dest;
	_cookie = ref._cookie;
	_method_str = ref._method_str;
	_connection_str = ref._connection_str;
	_header_flag = ref._header_flag;
	_body_flag = ref._body_flag;
	_req_buff = ref._req_buff;
	_header = ref._header;
	_ret = ref._ret;
	_body = ref._body;
	_cgi_ext = ref._cgi_ext;
    }
    return (*this);
}

    /* INIT / RESET */
int Request::init_tmpfile(void)
{
    _tmpdata = tmpfile();
    if (_tmpdata == NULL)
    {
        _status_code = 500;
        return (-1);
    }
    _tmpfd = fileno(_tmpdata);
    return (_tmpfd);
}

void Request::fill_tmpfile(void)
{
	int ret, ret2, ret3;

    /*** SI CHUNK -> NUNCHUNK***/
    ret3 = _header.find("Transfer-Encoding: chunked");
	if  (ret3 != -1)
    {
        //std::cout << "  [UNCHUNKING]" << std::endl;
        Nunchunk();
    }
	/*** ON ECRIT HEADER + BODY CRLF DANS TMPFILE***/
    ret = write(_tmpfd, _header.c_str(), _header.size());
	if (ret < 0)
	{
		perror("Error : write() on req_tmp_file failed");
		_status_code = 500;
	}
	if (ret == 0 && (_status_code == 200 || _status_code == 0))
		_status_code = 200;
    /*** SI BODY IL Y A ON L ECRIT DANS TMP FILE***/
    if (_body != "")
    {
        ret2 = write(_tmpfd, _body.c_str(), _body.size());
    	if (ret2 < 0)
        {
            perror("Error : write() on req_tmp_file failed");
            _status_code = 500;
        }
        if (ret2 == 0 && (_status_code == 200 || _status_code == 0))
            _status_code = 200;
    }
}

void Request::reset_req(void)
{
    //std::cout << "[RESET REQUEST]" << std::endl;
    _tmp = "";
    _uri = "";
    _query = "";
    _accept = "";
    _host = "";
    _multipart_delim = "";
    _rqlen = 0;
    _rqpos = 0;
    _state = RCVG_HEADER;
    _connection = CLOSE;
    _body_reception_encoding = NOT_SPECIFIED;
    _content_length = 0;
    _body_recv = 0;
    _cgi = 0;
    _mult = 0;
    _method = NOT_SPEC;
    _status_code = 0;
    _continue = 0;
    _upload_ok = 0;
	_header.clear();
	_body.clear();
	_req_buff.clear();
	_ret = 0;
	_header_flag = 0;
	_body_flag = 0;
	_tmpfd = -1;
	if (_tmpdata != NULL)
    	fclose(_tmpdata);
    init_tmpfile();
}

void Request::init_parsemap(void)
{
    _parsemap.insert(t_parsepair("content-length:", &WS::Request::parse_content_length));
    _parsemap.insert(t_parsepair("transfer-encoding:", &WS::Request::parse_transfer_encoding));
    _parsemap.insert(t_parsepair("content-type:", &WS::Request::parse_content_type));
	_parsemap.insert(t_parsepair("Connection:", &WS::Request::parse_connection));
	_parsemap.insert(t_parsepair("accept:", &WS::Request::parse_accept));
    _parsemap.insert(t_parsepair("host:", &WS::Request::parse_host));

	_parsemap.insert(t_parsepair("Accept-Encoding:", &WS::Request::parse_accept_encoding));
	_parsemap.insert(t_parsepair("Accept-Language:", &WS::Request::parse_accept_language));
	_parsemap.insert(t_parsepair("sec-ch-ua:", &WS::Request::parse_sec_ua));
	_parsemap.insert(t_parsepair("sec-ch-ua-mobile:", &WS::Request::parse_sec_ua));
	_parsemap.insert(t_parsepair("sec-ch-ua-platform:", &WS::Request::parse_sec_ua));
	_parsemap.insert(t_parsepair("Upgrade-Insecure_Requests:", &WS::Request::parse_upgrade_insecure_requests));
	_parsemap.insert(t_parsepair("User-Agent:", &WS::Request::parse_user_agent));
	_parsemap.insert(t_parsepair("Sec-Fetch-Site:", &WS::Request::parse_sec_fetch));
	_parsemap.insert(t_parsepair("Sec-Fetch-Mode:", &WS::Request::parse_sec_fetch));
	_parsemap.insert(t_parsepair("Sec-Fetch-User:", &WS::Request::parse_sec_fetch));
	_parsemap.insert(t_parsepair("Sec-Fetch-Dest:", &WS::Request::parse_sec_fetch));
	_parsemap.insert(t_parsepair("Cookie:", &WS::Request::parse_cookie));
}

    /* PARSING ENGINE */
std::string Request::gnl(void)
{
    char buf[BUFFER_SIZE];
    std::string line;
    int ret, ret2, ret3;

    if (_tmp != "")
    {
        ret2 = _tmp.find(CRLF);
        if (ret2 > -1)
        {
			ret3 = _tmp.find(BODY_CRLF);
            if (ret2 == ret3)            
                _state = HEADER_OK;
            line = _tmp.substr(0, ret2);
            _tmp = _tmp.substr(ret2 + 2, _tmp.size() - (ret2 + 2));
           return (line);
        }
    }
    ret = read(_tmpfd, buf, BUFFER_SIZE);
    if (ret > 0)
    {
        std::string buff(buf, ret);
        _tmp += buff;
        _rqpos += ret;
        ret2 = _tmp.find(CRLF);
        if (ret2 > -1)
        {
           ret3 = _tmp.find(BODY_CRLF);
            if (ret2 == ret3)
                _state = HEADER_OK;
            line = _tmp.substr(0, ret2);
            _tmp = _tmp.substr(ret2 + 2, _tmp.size() - (ret2 + 2));
            return (line);
        }
    }
    else if (ret < 0)
    {
        perror("Error : req_gnl was enable to read");
        _status_code = 500;
    }
    else if (ret == 0)
        std::cout << _BLUE << "EOF reached in req_gnl" << _END << std::endl;
    return ("");
}

int Request::parse_header(void)
{
    init_parsemap();
    std::string line;
    t_parsemap::iterator itmap;
    t_parsemap::iterator itend = _parsemap.end();
    std::string tmp;
    int i = 0;

    while (_state == RCVG_HEADER && (_status_code == 0 || _status_code == 200))
    {
        itmap = _parsemap.begin();
        line = gnl();
        if (line == "")
            break ;
        if (i == 0)
            parse_first_line(line);
        else
        {
            while (itmap != itend)
            {
                tmp = itmap->first;
                if (strToLower(line).find(tmp) != std::string::npos || line.find(tmp) != std::string::npos)
                {
                    (this->*(itmap->second))(line);
                }
                itmap++;
            }
        }
        i++;
    }
    if (_method == 1 && _host == "" && (_status_code == 0 || _status_code == 200))
        _status_code = 400;
    return (0);
}

    /* PARSING FCT */
void Request::parse_first_line(std::string line)
{
    parse_method(line);
    parse_uri(line);
    parse_query(line);
    parse_version(line);
}

void Request::parse_method(std::string line)
{
    if (line.find("GET") != std::string::npos)
	{
        _method = GET;
		_method_str = "GET";
	}
    else if (line.find("POST") != std::string::npos)
	{
	    _method = POST;
		_method_str = "POST";
	}	
	else if (line.find("DELETE") != std::string::npos)
	{
		_method = DELETE;
		_method_str = "DELETE";
	}
	else if (line.find("HEAD") != std::string::npos || line.find("PUT") != std::string::npos
				|| line.find("CONNECT") != std::string::npos || line.find("OPTIONS") != std::string::npos
				|| line.find("TRACE") != std::string::npos || line.find("PATCH") != std::string::npos ) 
        _method = OTHER;
    if (_method == NOT_SPEC)
    {
        _status_code = 400;
        perror("Error : No Method found");
        return ; 
    }
    //std::cout << "Method : " << _method << std::endl;
}

void Request::parse_uri(std::string line)
{
	//std::cout << "LINE " << line << std::endl;
    int ret = line.find("/");
    int ret2 = line.find("?", ret);
    if (ret2 == -1)
         ret2 = line.find(" ", ret);
    if (ret == -1)
    {
        _status_code = 400;
        perror("Error : No Path found");
        return ;
    }
    _uri = line.substr(ret, ret2 - ret);
    //std::cout << "URI : " << _uri << std::endl;
}

void Request::parse_query(std::string line)
{
    //std::cout << "Line: " << line << std::endl;
    int ret = line.find("?");
    if (ret != -1)
	{
        _query = line.substr(ret + 1);
	}
	ret = _query.find(" ");
	if (ret != -1)
		_query = _query.substr(0, ret);
    //std::cout << "Query String : " << _query << std::endl;
}

void Request::parse_version(std::string line)
{
    int ret = line.find("HTTP/1.1");
    if (ret == -1)
    {
        _status_code = 400;
        perror("Error : Version not Supported/Recognized");
        return ;  
    }
    //std::cout << "VERSION : HTTP/1.1" << std::endl;
    // VERIFIER SI CHAR APRES LA VERSION EN FIN DE LIGNE C EST UNE ERREUR JE CROIS
}

void Request::parse_content_length(std::string line)
{
	std::string s = "content-length";
	std::string cl;
	int ret = strToLower(line).find(s);
	int i = 0;
	while (line[ret + s.length() + 2 + i] && line[ret + s.length() + 2 + i] != '\r')
	{
		cl += line[ret + s.length() + 2 + i];
		i++;
	}
	_content_length = std::strtol(cl.c_str(), NULL, 10);
	if (_body_reception_encoding == CHUNKED)
	{
		_status_code = 400;
		perror("Error : overlapping body reception encoding directives");
		return ;  
	}
	if (_body_reception_encoding != MULTIPART)
	   	_body_reception_encoding = CONTENT_LENGTH;
       //std::cout << "Content-Length : " << _content_length << std::endl;
}

void Request::parse_transfer_encoding(std::string line)
{
    if (line.find("chunked") != std::string::npos)
    {
        if (_body_reception_encoding != NOT_SPECIFIED)
        {
            _status_code = 400;
            perror("Error : overlapping body reception encoding directives");
            return ;  
        }
        _body_reception_encoding = CHUNKED;
        //std::cout << "body_reception_encoding : " << _body_reception_encoding << std::endl;
    }
}

void Request::parse_content_type(std::string line)
{
	int ret;
	if (line.find("Multipart") != std::string::npos || line.find("multipart") != std::string::npos)
	{
		if (_body_reception_encoding == CHUNKED)
        {
           	_status_code = 400;
        	perror("Error : overlapping body reception encoding directives");
           	return ;  
       	}
		ret = line.find("boundary=");
		if (ret == -1)
		{
           	_status_code = 400;
        	perror("Error : boundary missing for multipart");
           	return ;  
       	}
		else
			_multipart_delim = line.substr(ret + 9);
		//std::cout << "Multipart_delim : " << _multipart_delim << std::endl;
	}
	_content_type = line.substr(std::string("content-type: ").size());
}

void    Request::parse_host(std::string line)
{
    int ret = line.find("Host: ");
    if (ret == -1)
        ret = line.find("host: ");
    if (ret != 0 && line.find("localhost") != std::string::npos)
        return ;
    if (ret == -1)
    {
        _status_code = 400;
        perror("Error: Host line format not recognized");
        return ;
    }
    else
        _host = line.substr(6);
    //std::cout << "host : " << _host << std::endl;
}

void    Request::parse_accept(std::string line)
{
    int ret = line.find("Accept: ");
    if (ret == -1)
        ret = line.find("accept: ");
    if (ret == -1)
    {
        _status_code = 400;
        perror("Error: Accept line format not recognized");
        return ;
    }
    else
        _accept = line.substr(8);
    //std::cout << "Accept : " << _accept << std::endl;
}

void Request::parse_connection(std::string line)
{
    int ret = line.find("Connection: ");
    if (ret == -1)
        ret = line.find("connection: ");
    if (ret == -1)
    {
        _status_code = 400;
        perror("Error : Connection line Format Incorrect");
        return ;  
    }
    else 
    {
        if (line.find("Keep-Alive", ret) != std::string::npos 
            || (line.find("keep-alive", ret) != std::string::npos))
            _connection = KEEP_ALIVE;
        else if (line.find("Close", ret) != std::string::npos 
            || (line.find("close", ret) != std::string::npos))
            _connection = CLOSE;
        else
        {
            _status_code = 400;
            perror("Error : Connection line option not recognized");
            return ;    
        }
        //std::cout << "Connection : " << _connection << std::endl;
		_connection_str = line.substr(std::string("connection: ").size());
	}
}

void Request::parse_accept_encoding(std::string line)
{
	int ret = line.find("accept-encoding: ");
    if (ret == -1)
        ret = line.find("Accept-Encoding: ");
    if (ret == -1)
    {
        _status_code = 400;
        perror("Error : Accept-Encoding line Format Incorrect");
        return ;  
    }
	_accept_encoding = line.substr(std::string("Accept-Encoding: ").size());
}

void Request::parse_accept_language(std::string line)
{
	int ret = line.find("accept-language: ");
    if (ret == -1)
        ret = line.find("Accept-Language: ");
    if (ret == -1)
    {
        _status_code = 400;
        perror("Error : Accept-Language line Format Incorrect");
        return ;  
    }
	_accept_language = line.substr(std::string("accept-language: ").size());
}

void Request::parse_sec_ua(std::string line)
{
	std::string dir;
	int pos;

	if (line.find("sec-ch-ua-mobile: ") != std::string::npos)
	{
		pos = line.find("sec-ch-ua-mobile: ");
		_sec_ch_ua_mobile = line.substr(pos + std::string("sec-ch-ua-mobile: ").size());
	}
	else if (line.find("sec-ch-ua-platform: ") != std::string::npos)
	{
		pos = line.find("sec-ch-ua-platform: ");
		_sec_ch_ua_platform = line.substr(pos + std::string("sec-ch-ua-platform: ").size());
	}
	else if (line.find("sec-ch-ua: ") != std::string::npos)
	{
		pos = line.find("sec-ch-ua: ");
		_sec_ch_ua = line.substr(pos + std::string("sec-ch-ua: ").size());
	}
	else
	{
		_status_code = 400;
        perror("Error : sec-ch-ua* line Format Incorrect");
        return ;  
    }
}

void Request::parse_upgrade_insecure_requests(std::string line)
{
	int ret = line.find("ugrade-insecure-requests: ");
    if (ret == -1)
        ret = line.find("Upgrade-Insecure-Requests: ");
    if (ret == -1)
    {
        _status_code = 400;
        perror("Error : Upgrade-Insecure-Requests line Format Incorrect");
        return ;  
    }
	_upgrade_insecure_requests = line.substr(std::string("upgrade-insecure-requests: ").size());
}

void Request::parse_user_agent(std::string line)
{
	int ret = line.find("user-agent: ");
    if (ret == -1)
        ret = line.find("User-Agent: ");
    if (ret == -1)
    {
        _status_code = 400;
        perror("Error : User-Agent line Format Incorrect");
        return ;  
    }
	_user_agent = line.substr(std::string("user-agent: ").size());
}

void Request::parse_cookie(std::string line)
{
	int ret = line.find("cookie: ");
    if (ret == -1)
        ret = line.find("Cookie: ");
    if (ret == -1)
    {
        _status_code = 400;
        perror("Error : Cookie line Format Incorrect");
        return ;  
    }
	_cookie = line.substr(std::string("cookie: ").size());
}

void Request::parse_sec_fetch(std::string line)
{
	std::string dir;
	int pos;

	if (line.find("Sec-Fetch-Site: ") != std::string::npos)
	{
		pos = line.find("Sec-Fetch-Site: ");
		_sec_fetch_site = line.substr(pos + std::string("Sec-Fetch-Site: ").size());
	}
	else if (line.find("Sec-Fetch-Mode: ") != std::string::npos)
	{
		pos = line.find("Sec-Fetch-Mode: ");
		_sec_fetch_mode = line.substr(pos + std::string("Sec-Fetch-Mode: ").size());
	}
	else if (line.find("Sec-Fetch-User: ") != std::string::npos)
	{
		pos = line.find("Sec-Fetch-User: ");
		_sec_fetch_user = line.substr(pos + std::string("Sec-Fetch-User: ").size());
	}
	else if (line.find("Sec-Fetch-Dest: ") != -std::string::npos)
	{
		pos = line.find("Sec-Fetch-Dest: ");
		_sec_fetch_dest = line.substr(pos + std::string("Sec-Fetch-Dest: ").size());
	}
	else
	{
		_status_code = 400;
        perror("Error : sec-ch-ua* line Format Incorrect");
        return ;  
    }
}

int Request::conc_request(char *buff, int bufflen, std::vector<std::string> cgi_ext)
{
    int ret, pos;
    /*** FILLING HEADER***/
    if (_header == "")
    {
        //std::cout << "  [CAS 1 GESTION HEADER]" << std::endl;
        /*** BODY CRLF TROUVE ON CREE LA STRING HEADER***/
        if (strstr(buff, "\r\n\r\n") != NULL)
        {
            //std::cout << "  [CAS 1 GESTION HEADER RNRN]" << std::endl;
            ret = findpos(buff, strstr(buff, "\r\n\r\n"), bufflen);
            if (ret == -1)
                return (-1);
            std::string h1(buff, ret + 4);
            /*** SI ON AVAIT DEJA UN MORCEAU DE HEADER ON CONCATENE ***/
            if (_req_buff != "")
                _header += _req_buff;
            _header += h1;
            /*** RECUP DEBUT DU BODY STOCKE DANS BUFFER ***/
            std::string bd(strstr(buff, "\r\n\r\n") + 4, (bufflen - (ret + 4)));
            if (bd != "")
            {
                _body = bd;
                //std::cout << "      [BODY 1ST PART]" << std::endl;
               // std::cout << "[" << _body << "]" << std::endl;
            }
        }
        /*** PAREIL SAUF QUE BODY_CRLF = \N\N ***/
        else if (strstr(buff, "\n\n") != NULL)
        {
            //std::cout << "  [CAS 1 GESTION HEADER NN]" << std::endl;
            ret = findpos(buff, strstr(buff, "\n\n"), bufflen);
            if (ret == -1)
                return (-1);
            std::string h2(buff, ret + 2);
            if (_req_buff != "")
                _header += _req_buff;
            _header += h2;
           // std::cout << "      [HEADER]" << std::endl;
            //std::cout << "[" << _header << "]" << std::endl;
            std::string bd(strstr(buff, "\n\n") + 2, (bufflen - (ret + 2)));
            if (bd != "")
            {
                _body = bd;
              // std::cout << "      [BODY 1ST PART]" << std::endl;
               // std::cout << "[" << _body << "]" << std::endl;
            }
        }
        /*** HEADER NON COMPLET ON STOCKE BUFF DANS REQ_BUFF ET ON REPART POUR UN TOUR ***/
        else 
        {
           // std::cout << "      [1.3 HEADER INCOMPLET]" << std::endl;
            std::string h3(buff, bufflen);
            _req_buff += h3;
            int retf;
            int size = 0;
            retf = _req_buff.find("\n\n");
            if (retf == -1)
            {
                retf = _req_buff.find("\r\n\r\n");
                size = 4;
            }
            else
                size = 2;
            if (retf != -1)
            {
                std::string h2;
                std::string bd;
                bd = "";
              //  std::cout << "  [CAS 1 GESTION SPE]" << std::endl;
                h2 = _req_buff.substr(0, retf + size);
                if ((int)_req_buff.size() > (retf + size))
                bd = _req_buff.substr(retf + size, _req_buff.size() - (retf + size));
                _header += h2;
             //   std::cout << "      [HEADER]" << std::endl;
              //  std::cout << "[" << _header << "]" << std::endl;
                if (bd != "")
                {
                    _body = bd;
                 //   std::cout << "      [BODY 1ST PART]" << std::endl;
                  //  std::cout << "[" << _body << "]" << std::endl;
                }
            }
            else 
                return (INCOMPLETE);
        }
    }
    /*** PRE-PARSING HEADER ***/
    if (_header != "" && _header_flag == 0)
    {
       // std::cout << "  [CAS 2 PARSING HEADER]" << std::endl;
        /*** VERIF HEADER SIZE ***/
        if (_header.size() > MAX_HEADER_SIZE)
			throw std::invalid_argument("Header size infinite too big or without crlf");
        /*** PARSING EXTENSION CGI (SI CGI IL Y A ) ***/
		std::string ext = "";
		for (std::vector<std::string>::iterator it = cgi_ext.begin(); it != cgi_ext.end(); it++)
		{
			ext = *it;
			if ((_header.find(ext) != std::string::npos && _header.find("Referer: ") == std::string::npos) 
  	          || (_header.find(ext) != std::string::npos && _header.find("Referer: ") != std::string::npos 
  	          && _header.find(ext) < _header.find("Referer: ")))
		 	{
				_cgi = 1;
				_cgi_ext = ext;
   		       // std::cout << "      [2.1 IS CGI] " << ext <<  std::endl;
			}
		}

        /*** PROTECTION CONTRE CHUNKED + CL ***/    
		if ((pos = _header.find("Content-Length: ")) != -1 && (pos = _header.find("Transfer-Encoding: chunked")) != -1)
			throw std::invalid_argument("Error: Wrong request : can't have both content-length and chunked");
        /*** PARSING CONTENT-LENGTH ***/
        else if ((pos = _header.find("Content-Length: ")) != -1)
        {
			parse_content_length(_header.substr(pos));
            //std::cout << "      [2.2 CONTENT LENGTH FOUND] | CL : " << _content_length << std::endl;
        }
        /*** PARSING MULTIPART ***/
        if ((pos = _header.find("Content-Type: multipart/form-data")) != -1)
        {
           // std::cout << "      [2.3 IS MULTIPART]" << std::endl;
            _mult = 1;
        }
        _header_flag = 1;
        if (is_body_full() != 0)
        {
            //std::cout << "      [2.4 BODY 1SHOT]" << std::endl;
            return (COMPLETE);
        }
        else
        {
           // std::cout << "      [2.5 BODY NOT 1SHOT]" << std::endl;
            return (INCOMPLETE);
        }
        
    }
    /*** PARTIE BODY ***/
    std::string bod(buff, bufflen);
    //std::cout << "  [CAS 3 GESTION BODY]" << std::endl;
    _body += bod;
    if (is_body_full() != 0)
        return (COMPLETE);
    else
    {
        _rqlen += _header.size();
        return (INCOMPLETE);
    }
}

void Request::Nunchunk(void)
{
    int chunk_len = 0;
    int pos;
    std::string tmpbody = "";
    std::string chunk_hex = "";
    std::string chunk = "";
    std::string crlf = "";

    if ((pos = _body.find("0\r\n\r\n")) != -1)
        crlf = "\r\n";
    else
        crlf = "\n";
    while (_body != ("0" + crlf + crlf))
    {
        pos = _body.find(crlf);
        chunk_hex = _body.substr(0, pos);
        chunk_len = (int)strtol(chunk_hex.c_str(), NULL, 16);
        chunk = _body.substr(pos + crlf.size(), chunk_len);
        _body = _body.substr(pos + 3 * crlf.size() + chunk_len);
        tmpbody += chunk;
    }
    _body = tmpbody;
    _body_flag = 1;
}

int Request::is_body_full(void)
{
    if(_header.find("Content-Length: ") == std::string::npos && _header.find("Transfer-Encoding: chunked") == std::string::npos)
        return (1);
    if (_content_length != 0)
    {
        if ((int)_body.size() >= _content_length)
            return (1);
    }
    else if (_header.find("Transfer-Encoding: chunked") != std::string::npos)
    {
        //std::cout << "[STAGE 1]" << std::endl;
        if ((_body.find("0\r\n\r\n") != std::string::npos) || (_body.find("0\n\n") != std::string::npos))
        {
        //    std::cout << "[STAGE 2]" << std::endl; 
            return (1);
        }
    }
    return (0);
}

int Request::is_req_tobig(void)
{
    if (_header == "")
    {
        if (_req_buff.size() > MAX_HEADER_SIZE)
        {
            _cgi = 0;
            _cgi_ext = "";
            return (1);
        }
    }
    else 
    {
        if (_header.size() > MAX_HEADER_SIZE || _req_buff.size() > MAX_BODY_SIZE || _body.size() > MAX_BODY_SIZE)
        {
            _cgi = 0;
            _cgi_ext = "";    
            return (1);
        }
    }
    return (0);
}

    /* SETTERZ */
void Request::set_state(int state) { _state = state; }
void Request::set_status_code(int code) { _status_code = code; }
void Request::set_cgi(int cgi) { _cgi = cgi; }

    /* GETTERZ */
FILE		*Request::get_tmpdata(void) { return (_tmpdata); }
int 		Request::get_tmpfd(void) { return (_tmpfd); }
int 		&Request::get_rqlen(void) { return (_rqlen);  }
int 		Request::get_rqpos(void) { return (_rqpos); }
int 		Request::get_state(void) { return (_state); }
int			Request::get_body_reception_encoding(void) { return (_body_reception_encoding); }
long long 	Request::get_body_recv(void) { return (_body_recv); }
int			Request::get_method(void) { return (_method); }
int			Request::get_status_code(void) { return (_status_code); }
std::string	Request::get_multipart_delim(void) { return (_multipart_delim); }
int			Request::get_continue(void) { return (_continue); }
int			Request::get_upload_ok(void) { return (_upload_ok); }
int			Request::get_cgi(void) { return (_cgi); }
int			Request::get_mult(void) { return (_mult); }

int 		Request::get_connection(void) { return (_connection); }
long long	Request::get_content_length(void) { return (_content_length); }
std::string	Request::get_uri(void) const { return (_uri); }
std::string	Request::get_body(void) const { return (_body); }
std::string Request::get_host(void) const { return (_host); }
std::string Request::get_accept(void) const { return (_accept); }
std::string Request::get_query(void) const { return (_query); }
std::string Request::get_accept_encoding(void) const { return (_accept_encoding); }
std::string Request::get_accept_language(void) const { return (_accept_language); }
std::string Request::get_content_type(void) const { return (_content_type); }
std::string Request::get_sec_ch_ua(void) const { return (_sec_ch_ua); }
std::string Request::get_sec_ch_ua_mobile(void) const { return (_sec_ch_ua_mobile); }
std::string Request::get_sec_ch_ua_platform(void) const { return (_sec_ch_ua_platform); }
std::string Request::get_upgrade_insecure_requests(void) const { return (_upgrade_insecure_requests); }
std::string Request::get_user_agent(void) const { return (_user_agent); }
std::string Request::get_sec_fetch_site(void) const { return (_sec_fetch_site); }
std::string Request::get_sec_fetch_mode(void) const { return (_sec_fetch_mode); }
std::string Request::get_sec_fetch_user(void) const { return (_sec_fetch_user); }
std::string Request::get_sec_fetch_dest(void) const { return (_sec_fetch_dest); }
std::string Request::get_cookie(void) const { return (_cookie); }
std::string Request::get_method_str(void) const { return (_method_str); }
std::string Request::get_connection_str(void) const { return (_connection_str); }
std::string Request::get_cgi_ext(void) const { return (_cgi_ext); }
std::string Request::get_header(void) const { return (_header); }

void Request::print(void) {
	
	std::cout << _BLUE << "\n----------------REQUEST-----------------\n" << _END << std::endl;
	std::cout << _header << std::endl;
	std::cout << _BLUE << "----------------------------------------\n" << _END << std::endl;
}

} // namespace WS

std::string strToLower(std::string s1)
{
	std::string s2;
	for (std::string::iterator it = s1.begin(); it != s1.end(); it++)
	{
		s2 += std::tolower(*it);
	}
	return s2;
}

int findpos(char *str, char *ptr, int len)
{
    char *tmp = str;
    int i = 0;

    while (i < len)
    {
        if (tmp == ptr)
            return (i);
        tmp++;
        i++;
    }
    return (-1);
}
