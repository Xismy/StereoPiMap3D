#include <string.h>
#include <string>
#include <opencv2/opencv.hpp>
#include "exceptions.h"
#include "StereoPiMap3D.h"
#include <opencv2/videoio.hpp>
#include <algorithm>
#include <unistd.h>
#include "http_server.h"
#include "mjpeg_stream.h"

StereoPiMap3D::StereoPiMap3D(mjpeg_http::MJPEG_Stream &leftStream, mjpeg_http::MJPEG_Stream &rightStream) :
leftStream(leftStream),
rightStream(rightStream)
{
	leftCamera = new cv::VideoCapture(
			"videotestsrc ! video/x-raw,framerate=20/1 !"
			" videoscale ! videoconvert ! appsink",
			cv::CAP_GSTREAMER
	);

	rightCamera = new cv::VideoCapture(
			"videotestsrc ! video/x-raw,framerate=20/1 !"
			" videoscale ! videoconvert ! appsink",
			cv::CAP_GSTREAMER
	);
}

StereoPiMap3D::~StereoPiMap3D(){
	delete leftCamera;
	delete rightCamera;
}

void StereoPiMap3D::mainLoop(){
	cv::Mat leftImage;
	cv::Mat rightImage;

	 *leftCamera >> leftImage;
	 *rightCamera >> rightImage;

	std::vector<uchar> buff;
	cv::imencode(".jpeg", leftImage, buff);
	leftStream.writeImg(reinterpret_cast<const char*>(buff.data()), buff.size());
	cv::imencode(".jpeg", rightImage, buff);
	rightStream.writeImg(reinterpret_cast<const char*>(buff.data()), buff.size());
}

void StereoPiMap3D::setCameraIndex(Side side, int index){
	cv::VideoCapture *&camera = side==Side::LEFT? leftCamera : rightCamera;
	delete camera;
	camera = new cv::VideoCapture(index);
}

void StereoPiMap3D::swapCameras(){
	std::swap(leftCamera, rightCamera);
}

void parseException(const std::exception &exception){
	std::cout << exception.what();
	exit(-1);
}

int main(int argc, char** argv){
	try{
		mjpeg_http::ServerConfig config;
		config.addr = "127.0.0.1";
		config.port = 5002;
		config.maxConnections = 1;
		mjpeg_http::Server server(config);
		StereoPiMap3D spm3d(
				server.addMJPEG_Stream("videoL", "--stpiL"),
				server.addMJPEG_Stream("videoR", "--stpiR")
		);

		while(1){
			spm3d.mainLoop();
			usleep(50000);
		}

		server.close();
	}catch(std::exception &e){std::cout << e.what();}
	return 0;
}
