#ifndef INCLUDE_STEREOPIMAP3D_H_
#define INCLUDE_STEREOPIMAP3D_H_

namespace mjpeg_http{
	class MJPEG_Stream;
}

namespace cv
{
	class VideoCapture;
}

class CameraViewport;

enum class Side
{
	LEFT,
	RIGHT
};

class StereoPiMap3D
{
private:
	cv::VideoCapture *leftCamera;
	cv::VideoCapture *rightCamera;
	mjpeg_http::MJPEG_Stream &leftStream;
	mjpeg_http::MJPEG_Stream &rightStream;

public:
	StereoPiMap3D(mjpeg_http::MJPEG_Stream &leftStream, mjpeg_http::MJPEG_Stream &rightStream);
	~StereoPiMap3D();

	StereoPiMap3D(const StereoPiMap3D &other) = delete;
	StereoPiMap3D(StereoPiMap3D &&other) = delete;
	bool operator=(StereoPiMap3D other) = delete;
	bool operator=(StereoPiMap3D &&other) = delete;

	void setCameraIndex(Side side, int index);
	void swapCameras();

	void init();
	void mainLoop();

};



#endif /* INCLUDE_STEREOPIMAP3D_H_ */
