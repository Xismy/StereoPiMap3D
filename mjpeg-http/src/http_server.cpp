#include "http_server.h"
#include "http_processor.h"
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <iostream>
#include <set>
#include <unistd.h>
#include <poll.h>

using namespace mjpeg_http;

Server::Server(const ServerConfig &config) : config(config){
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(sock == -1)
		throw SocketException(config, "creating socket");

	sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(config.port);
	saddr.sin_addr.s_addr = inet_addr(config.addr.c_str());

	if(bind(sock, reinterpret_cast<sockaddr*>(&saddr), sizeof(saddr)) == -1){
		throw SocketException(config, "binding socket");
	}

	if(listen(sock, config.maxConnections) == -1){
		throw SocketException(config, "listening socket");

	}

	bStopping = false;
	listeningThread	= std::thread([this](){this->acceptLoop();});

}

void Server::acceptLoop(){
	int clientSock = 0;
	std::vector<std::thread> threads;
	struct pollfd fd;
	fd.fd = sock;
	fd.events = POLLIN;

	do{
		poll(&fd, 1, 500);

		if(fd.revents == 0)
			continue;

		clientSock = accept(sock, nullptr, nullptr);

		if(clientSock != -1){
			threads.emplace_back([this, clientSock](){this->connectionLoop(clientSock);});
		}
		else{
			std::cerr << "Error accepting connection" << std::endl;
		}
	}while(clientSock != -1 and not bStopping);

	serverMutex.lock();
	for(auto conn : connections){
		conn->close();
	}
	serverMutex.unlock();

	for(auto &thread : threads){
		thread.join();
	}
}

void Server::connectionLoop(int clientSock){
	std::mutex sentMutex;
	std::shared_ptr<Connection> conn = std::make_shared<Connection>(clientSock, sentMutex);
	connections.insert(conn);

	HTTP_Parser http(*this, conn);
	char buff[L_HTML_REQ_BUFF];
	int nBytes;

	struct pollfd fd;
	fd.fd = clientSock;
	fd.events = POLLIN;

	do{
		poll(&fd, 1, 500);

		if(fd.revents == 0)
			continue;

		nBytes = recv(clientSock, buff, sizeof(buff), 0);

		if(nBytes == -1)
			break;

		std::lock_guard<std::mutex> lock(sentMutex);
		http.parse(buff, nBytes);
		std::cout << std::string(buff, nBytes) << std::endl << "----------------------------------------------" << std::endl;
		std::string response = http.getResponse();
		std::cout << response << std::endl << "***********************************************" << std::endl;

		if(response.size() > 0){
			if(send(clientSock, response.c_str(), response.size(), 0) == -1){
				break;
			}
		}
	}while(http.keepAlive() and conn->bActive);

	serverMutex.lock();
	connections.erase(conn);
	serverMutex.unlock();

	::close(clientSock);
}

MJPEG_Stream &Server::addMJPEG_Stream(const std::string &resource, const std::string &delimiter){
	return (streams.emplace(resource, delimiter).first)->second;
}

MJPEG_Stream *Server::getMJPEG_Stream(std::string resource){
	if(resource.size() > 0 and resource[0]=='/')
		resource = resource.substr(1);

	auto stream = streams.find(resource);

	if(stream != streams.end())
		return &(stream->second);

	return nullptr;
}

void Server::joinThread(){
	listeningThread.join();
}

void Server::close(){
	bStopping = true;
	joinThread();
}


/******************************************************************************/
SocketException::SocketException(const ServerConfig &config, const std::string info){
	std::stringstream infoStream;
	infoStream << "Error " << info << "." << std::endl;
	infoStream << "Config:" << std::endl;
	infoStream << "   Host: " << config.addr << ":" << config.port << std::endl;
	infoStream << "   Max connection: " << config.maxConnections << std::endl;

	this->info = infoStream.str();
}

const char *SocketException::what() const noexcept{
	return info.c_str();
}
