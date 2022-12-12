#include "Response.hpp"

namespace WS 
{

Response::Response(void) :
	_tmpdata(NULL),
	_tmpfd(-1),
    _cgidata(NULL),
    _cgifd(-1),
    _tmpsize(0), 
	_file_fd(-1),
    _response(""),
    _uri(""),
    _status_line(""),
    _header(""),
    _body(""),
    _content_type(""),
    _content_disposition(""),
    _autoindex(""),
    _location(""),
	_status(0),
    _connection(0),
	_status_code(200),
    _upload_ok(1),
    _method(NOT_SPEC),
    _cgi(0),
	_resp(""),
    _max_body_size(MAX_BODY_SIZE)
{
	init_codemap();
}

Response::Response(Response const &ref) { *this = ref; }

Response::~Response(void) { } 

Response &Response::operator=(Response const &ref)
{
    if (this != &ref)
    {
        _codemap = ref._codemap;
        _tmpdata = ref._tmpdata;
        _tmpfd = ref._tmpfd;
        _tmpsize = ref._tmpsize;
        _file_fd = ref._file_fd;
        _response = ref._response;
        _uri = ref._uri;
        _autoindex = ref._autoindex;
        _status_line = ref._status_line;
	    _header = ref._header;
        _cgi = ref._cgi;
        _cgifd = ref._cgifd;
        _cgidata = ref._cgidata;
	    _body = ref._body;
	    _content_type = ref._content_type;
	    _content_disposition = ref._content_disposition;
	    _status = ref._status;
	    _status_code = ref._status_code;
	    _upload_ok = ref._upload_ok;
	    _method = ref._method;
        _connection = ref._connection;
        _location = ref._location;
		_response = ref._resp;
        _max_body_size = ref._max_body_size;
    }
    return (*this);
}

void    Response::reset_res(void)
{
    //std::cout << "[RESET RESPONSE]" << std::endl;
    _response = "";
    _uri = "";
    _status_line = "";
    _header = "";
    _body = "";
    _autoindex = "";
    _content_type = "";
    _content_disposition = "";
    _location = "";
    _tmpsize = 0;
    _cgi = 0;
    _status_code = 200;
    _upload_ok = 1;
    _method = NOT_SPEC;
	_tmpfd = -1;
    _connection = 0;
    _resp = "";
	_file_fd = -1;
    _max_body_size = MAX_BODY_SIZE;
    init_codemap();
	if (_tmpdata != NULL)
    	fclose(_tmpdata);
    init_tmpfile();
}

void	Response::init_codemap(void)
{
    _codemap.insert(t_codepair(200, "OK"));
    _codemap.insert(t_codepair(201, "Created"));
    _codemap.insert(t_codepair(301, "Moved Permanently"));
    _codemap.insert(t_codepair(400, "Bad Request"));
    _codemap.insert(t_codepair(403, "Forbidden"));
    _codemap.insert(t_codepair(404, "Not Found"));
    _codemap.insert(t_codepair(405, "Method Not Allowed"));
    _codemap.insert(t_codepair(413, "Request Entity Too Large"));
    _codemap.insert(t_codepair(500, "Internal Server Error"));
    _codemap.insert(t_codepair(501, "Not Implemented"));
}

int	Response::init_tmpfile(void)
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

int Response::init_cgifile(void)
{
    _cgidata = tmpfile();
    if (_cgidata == NULL)
    {
        _status_code = 500;
        return (-1);
    }
    _cgifd = fileno(_cgidata);
    return (_cgifd);
}

//ACCESSORS - GETTERS//

FILE            *Response::get_tmpdata(void) { return (_tmpdata); }
int             Response::get_tmpfd( void ) { return (_tmpfd); }
FILE            *Response::get_cgidata(void) { return (_cgidata); }
int             Response::get_cgifd( void ) { return (_cgifd); }
long            Response::get_tmpsize( void ) { return (_tmpsize); }
int             Response::get_file_fd( void ) { return (_file_fd); }
std::string 	Response::get_response( void ) const { return _response; }
std::string 	Response::get_autoindex( void ) const { return _autoindex; }
std::string 	Response::get_content_type( void ) const { return _content_type; }
std::string		Response::get_status_line(void) const { return _status_line; }
std::string		Response::get_header( void ) const { return _header; }
std::string		Response::get_body( void ) { return _body; }
int				Response::get_status( void ) const { return _status; }
int				Response::get_method( void ) { return _method; }
int				Response::get_connection( void ) { return _connection; }
int				Response::get_status_code( void ) { return _status_code; }
int				Response::get_upload( void ) { return _upload_ok; }
int				Response::get_cgi( void ) { return _cgi; }
long long		Response::get_max_body_size( void ) { return _max_body_size; }
std::string		Response::get_uri( void ) { return _uri; }
std::string		Response::get_location( void ) { return _location; }
std::string		Response::get_code_string(int code)
{
    init_codemap();
    t_codemap::iterator itcod = _codemap.begin();
    t_codemap::iterator itend = _codemap.end();
    while (itcod != itend)
    {
        if (code == itcod->first)
            return (itcod->second);
        itcod++;
    }
    return ("");
}

std::string	Response::get_method_str( void )
{ 
    if (_method == 0)
        return ("OTHER");
    if (_method == 1)
        return ("GET");
    if (_method == 2)
        return ("POST");
    if (_method == 3)
        return ("DELETE");
    else
        return ("NOT_SPEC");
}

//ACCESSORS - SETTERS//
void			Response::set_response( std::string resp ){ _response = resp; }
void            Response::set_content_type( std::string content_type ){ _content_type = content_type; }
void			Response::set_status_line( std::string status_l ){ _status_line = status_l; }
void			Response::set_header( std::string header ){ _header = header; }
void			Response::set_body( std::string newbody ){ _body = newbody; }
void			Response::set_status ( int status ) { _status = status; }
void			Response::set_method ( int meth ) { _method = meth; }
void			Response::set_status_code ( int code ) { _status_code = code; }
void			Response::set_upload ( int up ) { _upload_ok = up; }
void			Response::set_uri ( std::string uri ) { _uri = uri; }
void			Response::set_file_fd ( int fd ) { _file_fd = fd; }
void			Response::set_connection ( int connection ) { _connection = connection; }
void			Response::set_tmpsize ( long size ) { _tmpsize = size; };
void			Response::set_add_tmpsize ( long size ) { _tmpsize += size; };           
void			Response::set_autoindex(std::string autoindex) { _autoindex = autoindex; }
void			Response::set_location(std::string location) { _location = location; }
void            Response::set_cgifd(int fd) { _cgifd = fd;}
void            Response::set_cgidata(FILE *ptr) { _cgidata = ptr; }
void            Response::set_cgi(int cgi) { _cgi = cgi; }
void            Response::set_max_body_size(long long bodysize) { _max_body_size = bodysize; }

void			Response::set_resp(std::string response) { _resp = response; }

void			Response::print(void) {

	std::cout << _PURPLE << "\n----------------RESPONSE----------------\n" << _END << std::endl;
	std::cout << _resp << std::endl;
	std::cout << _PURPLE << "----------------------------------------\n" << _END << std::endl;
}


} // namespace WS