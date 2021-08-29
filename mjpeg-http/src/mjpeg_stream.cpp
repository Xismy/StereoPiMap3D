#include "mjpeg_stream.h"
#include "http_server.h"

using namespace mjpeg_http;

void MJPEG_Stream::writeImg(const char *img, size_t size){
	std::stringstream frameHTTP;

	frameHTTP << boundary << "\r\n";
	frameHTTP << "Content-Type: image/jpeg\r\n";
	frameHTTP << "\r\n";
	frameHTTP.write(img, size);

	std::lock_guard<std::mutex> streamLock(streamMutex);

	for(auto con=connections.begin(); con!=connections.end(); ){
		std::unique_lock<std::mutex> lock((*con)->connMutex, std::try_to_lock);

		if(lock.owns_lock()){
			if(not (*con)->bActive){
				connections.erase(con);
			}
			else{
				send((*con)->sock, frameHTTP.str().c_str(), frameHTTP.str().size(), 0);
				++con;
			}
		}
	}
}


