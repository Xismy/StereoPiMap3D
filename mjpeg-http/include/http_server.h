/*
 * http_server.h
 *
 *  Created on: 19 ago. 2021
 *      Author: admin
 */

#ifndef MJPEG_HTTP_INCLUDE_HTTP_SERVER_H_
#define MJPEG_HTTP_INCLUDE_HTTP_SERVER_H_

#include <string>
#include <atomic>
#include <thread>
#include <map>
#include <set>
#include <memory>
#include "mjpeg_stream.h"

#define L_HTML_REQ_BUFF 4*1024

namespace mjpeg_http{
	class HTTP_Parser;

	struct Connection{
		int sock;
		std::mutex &connMutex;
		std::atomic_bool bActive;

		Connection(int sock, std::mutex &m) : sock(sock), connMutex(m), bActive(true){}

		bool operator<(const Connection &other) const {return this->sock < other.sock;}
		void close(){std::lock_guard<std::mutex> lock(connMutex); bActive = false;}
	};

	struct ServerConfig{
		std::string addr;
		int port;
		int maxConnections;
	};

	class Server{
	private:
		ServerConfig config;
		int sock;
		std::atomic_bool bStopping;

		std::thread listeningThread;
		void acceptLoop();
		void connectionLoop(int clientSocket);
		bool isStopping();

		std::set<std::shared_ptr<Connection> > connections;
		std::map<std::string, MJPEG_Stream> streams;

		std::mutex serverMutex;

	public:
		Server(const ServerConfig &config);
		Server(const Server &&other);
		MJPEG_Stream &addMJPEG_Stream(const std::string &resource, const std::string &delimiter);
		MJPEG_Stream *getMJPEG_Stream(std::string resource);
		void joinThread();
		void close();
	};

	class SocketException : public std::exception{
	private:
		std::string info;

	public:
		SocketException(const ServerConfig &config, const std::string info);
		virtual const char *what() const noexcept override;
	};
}

#endif /* MJPEG_HTTP_INCLUDE_HTTP_SERVER_H_ */
