/*
 * mjpeg_stream.h
 *
 *  Created on: 19 ago. 2021
 *      Author: admin
 */

#ifndef MJPEG_HTTP_INCLUDE_MJPEG_STREAM_H_
#define MJPEG_HTTP_INCLUDE_MJPEG_STREAM_H_

#include <mutex>
#include <set>
#include <sys/socket.h>
#include <sstream>
#include <memory>
#include <iostream>

namespace mjpeg_http{
	class Connection;

	class MJPEG_Stream{
	private:
		std::string boundary;
		std::set<std::shared_ptr<Connection> > connections;
		std::mutex streamMutex;

	public:
		MJPEG_Stream(const std::string &delim) : boundary(delim){}

		void addConnection(std::shared_ptr<Connection> connection){
			std::lock_guard<std::mutex> lock(streamMutex);
			connections.insert(connection);
		}

		std::string getBoundary(){
			return boundary;
		}

		void writeImg(const char *img, size_t size);

	};
}

#endif /* MJPEG_HTTP_INCLUDE_MJPEG_STREAM_H_ */
