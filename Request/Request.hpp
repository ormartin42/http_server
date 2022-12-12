#ifndef REQUEST_HPP
# define REQUEST_HPP

# define BUFFER_SIZE 4096
 
# include <vector>
# include <map>
# include <string>
# include <iostream>
# include <fstream>
# include <stdexcept>
# include "unistd.h"
# include "stdlib.h"
# include "stdio.h"
# include <string.h>

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

// HEADER
# define COMPLETE 1
# define INCOMPLETE 0
# define MAX_HEADER_SIZE 4096
# define MAX_BODY_SIZE 4000000000

namespace WS
{
    
class Request
{

public:
    /* TypeDef */ 
    typedef void (Request::*t_parse_fct)(std::string);
    typedef std::map<std::string, t_parse_fct> t_parsemap;
    typedef std::pair<std::string, t_parse_fct> t_parsepair;

    /* CANON */
    Request(void);
    Request(Request const &ref);
    ~Request(void);

    Request &operator=(Request const &rhs); 

    /* INIT / RESET */
    void init_parsemap(void);
    int	 init_tmpfile(void);
    void fill_tmpfile(void);
    void reset_req(void);

    /* PARSING ENGINE */
    std::string gnl(void);
    int  parse_header(void);
	//int	 conc_request(std::string buff, int ret);
    int  conc_request(char *buff, int bufflen, std::vector<std::string> cgi_ext);
    void Nunchunk(void);
    int is_body_full(void);
    int is_req_tobig(void);


    /* PARSING FCT */ 
    void parse_first_line(std::string line);
    void parse_method(std::string line);
    void parse_uri(std::string line);
    void parse_query(std::string line);
    void parse_version(std::string line);

    void parse_content_length(std::string line);
    void parse_transfer_encoding(std::string line);
    void parse_content_type(std::string line);
    void parse_connection(std::string line);
    void parse_accept(std::string line);
    void parse_host(std::string line);
	void parse_accept_encoding(std::string line);
	void parse_accept_language(std::string line);
	void parse_sec_ua(std::string line);
	void parse_upgrade_insecure_requests(std::string line);
	void parse_user_agent(std::string line);
	void parse_sec_fetch(std::string line);
	void parse_cookie(std::string line);

    /* GETTERZ */

    FILE *get_tmpdata(void);
    int get_tmpfd(void);
    int &get_rqlen(void);
    int get_rqpos(void);
    int get_state(void);
    int get_body_reception_encoding(void);
    long long get_body_recv(void);
    int get_method(void);
    int get_status_code(void);
    std::string get_multipart_delim(void);
    int get_continue(void);
    int get_upload_ok(void);
    int get_cgi(void);
    int get_mult(void);
	
    int get_connection(void);
    long long get_content_length(void);
    std::string get_body(void) const;
    std::string get_uri(void) const;
    std::string get_host(void) const;
	std::string get_accept(void) const;
	std::string get_query(void) const ;
	std::string get_content_type(void) const;
	std::string get_accept_encoding(void) const;
	std::string get_accept_language(void) const;
	std::string get_sec_ch_ua(void) const;
	std::string get_sec_ch_ua_mobile(void) const;
	std::string get_sec_ch_ua_platform(void) const;
	std::string get_upgrade_insecure_requests(void) const;
	std::string get_user_agent(void) const;
	std::string get_sec_fetch_site(void) const;
	std::string get_sec_fetch_mode(void) const;
	std::string get_sec_fetch_user(void) const;
	std::string get_sec_fetch_dest(void) const;
	std::string get_cookie(void) const;
	std::string get_method_str(void) const;
	std::string get_connection_str(void) const;
	std::string get_cgi_ext(void) const;
	std::string get_header(void) const;

    /* SETTERZ */
    void set_state(int state);
    void set_rqlines(int line);
    void set_status_code(int code);
    void set_cgi(int cgi);

	void print();

private:
    t_parsemap _parsemap;
    FILE *_tmpdata;
    int  _tmpfd;
    std::string _tmp;
    std::string _uri;
    std::string _query;
    std::string _accept; // formats acceptes pour les reponses
    std::string _host; //host used a comparer avec la liste de host d'oriane
    std::string _multipart_delim; // Flag si le content_type est multipart;
	std::string	_content_type;
	std::string _accept_encoding;
	std::string	_accept_language;
	std::string _sec_ch_ua;
	std::string _sec_ch_ua_mobile;
	std::string _sec_ch_ua_platform;
	std::string _upgrade_insecure_requests;
	std::string _user_agent;
	std::string _sec_fetch_site;
	std::string _sec_fetch_mode;
	std::string _sec_fetch_user;
	std::string _sec_fetch_dest;
	std::string _cookie;
	std::string _method_str;
	std::string _connection_str;
    
    int _cgi;
	int _rqlen; // Nb lignes stockees dans la request
    int _rqpos; // nombre de char lu dans la requete
    int _mult;
    int _state; // Etat de la requete  : RQ_FMT_ERROR -1 | RCVG_HEADER 0 | HEADER_OK 1 | BODY_OK 2
    int _connection; // Directives de connection : CLOSE 0 | KEEP_ALIVE 1
    int _body_reception_encoding; // NOT_SPECIFIED 0 | CONTENT_LENGTH 1 | CHUNKED 2 | MULTIPART 3
    long long _content_length; // Length du body
    long long _body_recv; // Nombre de bytes recus dans le body 
    int _method; // Methode de la requete
    int _status_code; // Status code to return
    int _continue; // Si la directive expect 100-continue est presente;
    int _upload_ok; // si l'upload est autorise (default = oui)
	//std::string _content; // Full requete dans une string
    //std::string _header; // Full header dans une string
    //std::vector<std::string> _vheader; //vector contenant tout les champs de requete headers  
    //std::string _body; // Full body dans dans une string
    //int _header_size; // Length du header
    //int _header_recv; // Nombre de bytes recus dans le header
	std::string _req_buff;
//public:
	std::string _header;
	std::string _body;
private:
	int			_ret;
	bool		_header_flag;
	bool		_body_flag;
	std::string _cgi_ext;

}; // class Request

} // namespace WS

std::string strToLower(std::string s1);
int findpos(char *str, char *ptr, int len);

#endif