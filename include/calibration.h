/*
 * calibration.h
 *
 *  Created on: 14 nov. 2020
 *      Author: admin
 */

#ifndef INCLUDE_CALIBRATION_H_
#define INCLUDE_CALIBRATION_H_

#include <opencv2/videoio.hpp>
#include <opencv2/core/types.hpp>

struct CalibrationConfig
{
	int cameraIndex;
	cv::Size size;
	int nSamples;
};

class Calibration
{
private:
	CalibrationConfig configuration;
	cv::VideoCapture camera;
	std::vector<cv::Mat> points;
	bool bCalibrating = false;

public:
	Calibration(std::string configFile);

	void capture(cv::Mat &img);
};

#endif /* INCLUDE_CALIBRATION_H_ */
