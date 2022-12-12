#ifndef AUTOINDEX_HPP
# define AUTOINDEX_HPP

# include "iostream"
# include <dirent.h>
# include <string>
# include <sstream>
# include <vector>
# include <stdexcept>
# include <cstring>

# define DT_DIR DT_DIR
# define DT_REG DT_REG

namespace WS {

class Autoindex {

	public :

		typedef std::vector<std::string>::iterator	it_vec;

		Autoindex();
		Autoindex(std::string const path);
		Autoindex(Autoindex const & other);
		~Autoindex();
		Autoindex &operator=(Autoindex const & other);

		void readDirectory(std::string const path);
		void generateAutoindex(std::string const loc);

		std::string const getAutoindex(void) const;
		std::string const getLocation(std::string path) const;

	private:

		std::vector<std::string>	_files;
		std::vector<std::string>	_dirs;
		std::string					_autoindex;

}; // class Autoindex

} // namespace WS

#endif