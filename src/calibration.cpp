#include "calibration.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/persistence.hpp>
#include <exception>
#include <stdio.h>

Calibration::Calibration(std::string configFile)
{
	cv::FileStorage file(configFile, cv::FileStorage::READ);

	if(!file.isOpened())
		throw std::string("Could not open file: [") + configFile + "]";

	configuration.cameraIndex = file["CameraIndex"];
	cv::FileNode size = file["Size"];
	configuration.size.width = size["Width"];
	configuration.size.height = size["Height"];
	configuration.nSamples = file["NSamples"];

	if(!camera.open(configuration.cameraIndex))
		throw std::string("Could not open camera");

	points = std::vector<cv::Mat>(configuration.nSamples);
}

void Calibration::capture(cv::Mat &frame)
{
	camera >> frame;
	cv::Mat framePoints;
	bool bFound = cv::findChessboardCornersSB(frame, configuration.size, framePoints);
	cv::drawChessboardCorners(frame, configuration.size, framePoints, bFound);

	if(bCalibrating){
		points.push_back(std::move(framePoints));
	}
}
