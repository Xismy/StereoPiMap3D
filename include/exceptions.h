#ifndef INCLUDE_EXCEPTIONS_H_
#define INCLUDE_EXCEPTIONS_H_

#include <exception>
#include <string>

class CouldNotOpenException : public std::exception
{
private:
	std::string text;

public:
	CouldNotOpenException(std::string file){
		text = std::string("Could not open file [") + file + "]";
	}

	virtual const char *what() const noexcept override{
		return text.c_str();
	}
};


#endif /* INCLUDE_EXCEPTIONS_H_ */
