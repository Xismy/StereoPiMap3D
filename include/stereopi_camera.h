/*
 * stereopi_camera.h
 *
 *  Created on: 19 dic. 2021
 *      Author: admin
 */

#ifndef INCLUDE_STEREOPI_CAMERA_H_
#define INCLUDE_STEREOPI_CAMERA_H_

#include <string>

#include <opencv2/core/types.hpp>
#include <opencv2/core/core.hpp>

namespace stereopi{
	class Camera{
	private:
		int _id;
		cv::Mat _matrix;
		cv::Mat _distortion;

	public:
		//Camera() : _id(0), _matrix(cv::Mat{3,3, CV_64FC1}), _distortion(cv::Mat{5, 1, CV_64FC1}) {}
		Camera(const std::string &paramsFile);

		int id(){ return _id; }
		const cv::Mat &matrix(){ return _matrix; }
		const cv::Mat &distortion(){ return _distortion; }
	};
}



#endif /* INCLUDE_STEREOPI_CAMERA_H_ */
