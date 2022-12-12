#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <vector>
# include <map>
# include <string>
# include <iostream>
# include <fstream>
# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <sys/types.h>
# include <sys/stat.h>

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

// REQUEST STATE
# define RQ_FMT_ERROR -1
# define RCVG_HEADER 0
# define HEADER_OK 1
# define BODY_OK 2

// CONNECTION ORDERS
# define CLOSE 0
# define KEEP_ALIVE 1

// BODY RECEPTION ENCODING TYPE
# define NOT_SPECIFIED 0
# define CONTENT_LENGTH 1
# define CHUNKED 2
# define MULTIPART 3

// SEPARATOR 
# define CRLF "\r\n"
# define BODY_CRLF "\r\n\r\n"

// METHOD
# define NOT_SPEC -1
# define OTHER 0
# define GET 1
# define POST 2
# define DELETE 3

# define MAX_BODY_SIZE 4000000000

namespace WS 
{

class Response 
{
	/* typedef */
	typedef std::map<int, std::string> t_codemap;
	typedef std::pair<int, std::string> t_codepair;

private:

	t_codemap		_codemap;
	FILE 			*_tmpdata;
    int  			_tmpfd;
	FILE 			*_cgidata;
	int 			_cgifd;
	long  			_tmpsize;
	int  			_file_fd;
	std::string		_response;
	std::string		_uri; 
	std::string 	_status_line;
	std::string		_header;
	std::string		_body;
	std::string		_content_type;
	std::string		_content_disposition;
	std::string		_autoindex;
	std::string		_location;
	int				_status;
	int				_connection;
	int				_status_code;
	int				_upload_ok;
	int				_method;
	int				_cgi;
	std::string		_resp;
	long long		_max_body_size;

public:
	
	Response( void );
	Response( Response const & src );

	~Response();

	void    			reset_res(void);
	void            	init_codemap(void);
	int 				init_tmpfile(void);
	int 				init_cgifile(void);

	Response &		operator=( Response const & ref );

	void				set_tmpsize(long size);
	void				set_add_tmpsize(long size);
    void				set_content_type( std::string file_path );
	void				set_content_disposition( std::string file_path );
	void				set_response( std::string resp );
	void				set_status_line( std::string status_l );
	void				set_header( std::string header );
	void				set_body( std::string newbody );
	void				set_status( int status );
	void				set_method(int meth);
	void				set_status_code(int code);
	void				set_upload(int up);
	void				set_uri(std::string uri);
	void				set_file_fd(int fd);
	void				set_connection(int connection);
	void				set_autoindex(std::string autoindex);
	void				set_location(std::string location);
	void				set_cgidata(FILE *ptr);
	void				set_cgifd(int fd);
	void				set_cgi(int cgi);
	void				set_resp(std::string response);
	void				set_max_body_size(long long bodysize);

	FILE 				*get_tmpdata(void);
    int 				get_tmpfd(void);
	FILE 				*get_cgidata(void);
    int 				get_cgifd(void);
	long				get_tmpsize(void);
	int 				get_file_fd(void);
    std::string			get_response( void ) const;
	std::string			get_content_type( void ) const;
	std::string			get_status_line(void) const;
	std::string			get_header( void ) const;
	std::string			get_body( void );
	std::string			get_autoindex( void ) const;
	int					get_status( void ) const;
	int					get_method(void);
	int					get_connection(void);
	std::string			get_method_str(void);
	int					get_status_code(void);
	int					get_upload(void);
	int					get_cgi(void);
	std::string			get_uri(void);
	std::string			get_code_string(int code);
	std::string			get_location(void);
	long long			get_max_body_size(void);

	void				print(void);

}; // class Response

std::string	itoa(int status_code);

} // namespace WS

#endif