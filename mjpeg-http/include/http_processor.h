/*
 * mjpeg-http.h
 *
 *  Created on: 16 ago. 2021
 *      Author: admin
 */

#ifndef MJPEG_HTTP_MJPEG_HTTP_H_
#define MJPEG_HTTP_MJPEG_HTTP_H_


#include <string>
#include <vector>
#include <map>
#include <memory>
#include <sstream>

#define HTTP_VERSION "HTTP/1.1"

namespace mjpeg_http{
	class Server;
	class MJPEG_Stream;
	class Connection;

	enum class HTTP_Status{
		OK           = 200,
		BAD_REQUEST  = 400,
		FORBIDDEN    = 403,
		NOT_FOUND    = 404,
		INTERNAL_ERR = 500
	};

	struct HTTP_StatusInfo{
		std::string name;
		std::string defaultFile;
	};

	const std::map<HTTP_Status, HTTP_StatusInfo> STATUS_MAP{
		{HTTP_Status::OK,           {"OK"             , "html/index.html"          }},
		{HTTP_Status::BAD_REQUEST,  {"Bad request"    , "html/bad_request.html"    }},
		{HTTP_Status::FORBIDDEN,    {"Forbidden"      , "html/forbidden.html"      }},
		{HTTP_Status::NOT_FOUND,    {"Not found"      , "html/not_found.html"      }},
		{HTTP_Status::INTERNAL_ERR, {"Internal error" , "html/internal_err.html"   }}
	};


	struct HTTP_Request{
		std::string method;
		std::string resource;
		std::string version;
		std::vector<std::pair<std::string, std::string>> headers;
	};

	struct HTTP_Response{
		HTTP_Status status;
		std::vector<std::pair<std::string, std::string>> headers;
		std::string body;

		void reset();
	};

	class HTTP_Parser{
	private:
		bool bKeepAlive = true;
		bool bReadingHeader = false;

		HTTP_Request request;
		HTTP_Response response;

		Server &server;
		std::shared_ptr<Connection> connection;

		void addHeader(std::string label, std::string value);
		void addHeader(std::string label, const char * value);

		template<typename T>
		void addHeader(std::string label, T value){
			addHeader(label, std::to_string(value));
		}

		void readHeaders(std::stringstream &is);
		void process();
		void processGet();
		void processForbidden();
		void processStatus(HTTP_Status status);
		void loadContent(const std::string &filename);
		void joinStream(MJPEG_Stream &stream);

		bool fileExists(const std::string &fileExists) const;

		typedef void (HTTP_Parser::*HTTP_Cb)();
		const std::map<std::string, HTTP_Cb> METHOD_MAP{
			{"GET"    , &HTTP_Parser::processGet},
			{"HEAD"   , &HTTP_Parser::processForbidden},
			{"POST"   , &HTTP_Parser::processForbidden},
			{"PUT"    , &HTTP_Parser::processForbidden},
			{"DELETE" , &HTTP_Parser::processForbidden},
			{"CONNECT", &HTTP_Parser::processForbidden},
			{"OPTIONS", &HTTP_Parser::processForbidden},
			{"TRACE"  , &HTTP_Parser::processForbidden},
			{"PATCH"  , &HTTP_Parser::processForbidden}
		};

	public:
		HTTP_Parser(Server &server, std::shared_ptr<Connection> conn);
		void parse(const char *buff, int size);
		std::string getResponse();
		bool keepAlive() const;
	};
}





#endif /* MJPEG_HTTP_MJPEG_HTTP_H_ */
