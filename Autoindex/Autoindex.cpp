#include "Autoindex.hpp"

namespace WS {

Autoindex::Autoindex() { }
Autoindex::Autoindex(Autoindex const & other) { *this = other; }
Autoindex::~Autoindex() { }
Autoindex &Autoindex::operator=(Autoindex const & other)
{
	if (this != &other)
		this->_files = other._files;
	return (*this);
}

Autoindex::Autoindex(std::string const path)
{
	std::string loc = getLocation(path);
	readDirectory(path);
	generateAutoindex(loc);
}

void Autoindex::readDirectory(std::string const path)
{
	DIR *dir;
	struct dirent *diread;
	std::string tmp;

	if ((dir = opendir(path.c_str())) != NULL)
	{
		while ((diread = readdir(dir)) != NULL && tmp.find(".php"))
		{
			tmp = diread->d_name;
			if (diread->d_name[0] != '.' && tmp.find(".php") == std::string::npos && diread->d_type == DT_REG)
				_files.push_back(diread->d_name);
			if (diread->d_name[0] != '.' && tmp.find(".php") == std::string::npos && diread->d_type == DT_DIR)
				_dirs.push_back(diread->d_name);
		}
		closedir(dir);
	}
	else
		throw std::invalid_argument("Error: opendir() failed");
}

void Autoindex::generateAutoindex(std::string const loc)
{
	std::stringstream autoindex;
	std::stringstream content;
	std::string dir_path;
	int pos;

	autoindex.str("");
	autoindex	<< "<DOCTYPE html>"
				<< "<html><head>"
        		<< "<meta charset=\"utf-8\">"
        		<< "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
				<< "<style>"
				<< "tite { text-align: left; background-color: whitesmoke; color: black; }"
				<< "body { text-align: left; width: 35em; margin: 0 auto; font-family: Tahoma, Verdana, Arial, sans-serif; text-align: center; background-color: whitesmoke; } </style>"
				<< "<br/><br/><br/>"
				<< "<title>Autoindex</title>"
				<< "</head><body>"
				<< "<h1>Autoindex of " << loc << "</h1>";
	_autoindex = autoindex.str();

	for (it_vec it = _files.begin(); it != _files.end(); it++)
	{
		content.str("");
		content << "<li><a href=\"" << loc + '/' + *it << "\">" << *it << "</a></li>";
		_autoindex += content.str();
	}
	for (it_vec it = _dirs.begin(); it != _dirs.end(); it++)
	{
		pos = loc.find("/");
		if (pos != -1)
			dir_path = loc.substr(pos + 1);
		else
			dir_path = loc;
		content.str("");
		content << "<li><a href=\"" << dir_path + '/' + *it  << "\">" << *it << "</a></li>";
		_autoindex += content.str();
	}
	for (it_vec it = _files.begin(); it != _files.end(); it++)

	_autoindex += "</ul></body></html>";
}

std::string const Autoindex::getAutoindex(void) const { return (_autoindex); }
std::string const Autoindex::getLocation(std::string path) const {

	std::string tmp1;
	std::string tmp2;
	int pos = path.find("www/");

	tmp1 = path.substr(pos + 4);
	pos = tmp1.find("/");
	tmp2 = tmp1.substr(pos + 1);
	if (tmp2 != "")
	{
		return (tmp2);
	}
	if (pos != -1)
		tmp1.resize(tmp1.size() - 1);
	return (tmp1);
}

} // namespace WS
