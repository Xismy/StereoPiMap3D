#include "http_processor.h"
#include "mjpeg_stream.h"
#include "http_server.h"
#include <fstream>
#include <filesystem>
#include <sys/stat.h>


using namespace mjpeg_http;

void HTTP_Response::reset(){
	headers.clear();
	body.clear();
}

HTTP_Parser::HTTP_Parser(Server &server, std::shared_ptr<Connection> conn) :
		server(server),
		connection(conn)
{}

void HTTP_Parser::parse(const char *buff, int size){

	std::stringstream req(std::string(buff, size));

	if(not bReadingHeader){
		std::getline(req, request.method, ' ');
		std::getline(req, request.resource, ' ');
		std::getline(req, request.version);

		if(request.version.back() == '\r')
			request.version.pop_back();

		if(METHOD_MAP.find(request.method) == METHOD_MAP.end()){
			processStatus(HTTP_Status::BAD_REQUEST);
			return;
		}

		bReadingHeader = true;
	}

	if(bReadingHeader)
		readHeaders(req);

	if(not bReadingHeader)
		process();

}

bool HTTP_Parser::fileExists(const std::string &filename) const{
	struct stat info;

	return (stat(filename.c_str(), &info) == 0);
}

void HTTP_Parser::processGet(){
	processStatus(HTTP_Status::OK);

	MJPEG_Stream *stream = server.getMJPEG_Stream(request.resource);

	if(stream){
		joinStream(*stream);
	}
	else{
		std::string filePath = "html/" + request.resource;

		if(fileExists(filePath)){
			loadContent(filePath);
			addHeader("Content-Type", "text/html");
			addHeader("Content-Length", response.body.size());
		}
		else{
			processStatus(HTTP_Status::NOT_FOUND);
		}
	}
}

void HTTP_Parser::processForbidden(){
	processStatus(HTTP_Status::FORBIDDEN);
}

void HTTP_Parser::processStatus(HTTP_Status status){
	response.status = status;

	if(status != HTTP_Status::OK){
		response.reset();
		loadContent(STATUS_MAP.at(status).defaultFile);
		addHeader("Content-Type", "text/html");
		addHeader("Content-Length", response.body.size());
	}
}

void HTTP_Parser::readHeaders(std::stringstream &is){
	std::string line;

	while(is.good()){
		std::getline(is, line);

		if(line=="" or line=="\r"){
			bReadingHeader = false;
			return;
		}
	}
}

void HTTP_Parser::addHeader(std::string label, std::string value){
	response.headers.emplace_back(label, value);
}

void HTTP_Parser::addHeader(std::string label, const char* value){
	addHeader(label, std::string(value));
}

void HTTP_Parser::process(){
	try{
		auto fn = METHOD_MAP.at(request.method);
		(this->*fn)();
	}catch(std::out_of_range &ex){
		processStatus(HTTP_Status::BAD_REQUEST);
	}
}

std::string HTTP_Parser::getResponse(){
	if(response.headers.size() == 0)
		return "";

	std::stringstream responseStream;

	responseStream << HTTP_VERSION << " " << static_cast<int>(response.status) << " " << STATUS_MAP.at(response.status).name << "\r\n";

	for(auto header : response.headers){
		responseStream << header.first << ": " << header.second << "\r\n";
	}

	responseStream << "\r\n";
	responseStream <<  response.body;

	response.reset();

	return responseStream.str();
}

void HTTP_Parser::loadContent(const std::string &filename){
	std::ifstream ifs(filename);

	if(ifs.fail()){
		std::stringstream body;
		body << "<h1>" << static_cast<int>(response.status) << " " << STATUS_MAP.at(response.status).name << "/h1\r\n";
		response.body = body.str();
	}

	std::stringstream ss;
	ss << ifs.rdbuf();
	response.body = ss.str();
}

void HTTP_Parser::joinStream(MJPEG_Stream &stream){
	std::string contenType = "multipart/x-mixed-replace; boundary=" + stream.getBoundary();
	addHeader("Content-Type", contenType);
	stream.addConnection(connection);
}

bool HTTP_Parser::keepAlive() const{
	return bKeepAlive;
}
