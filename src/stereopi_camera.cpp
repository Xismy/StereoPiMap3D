#include "stereopi_camera.h"

#include <string>

#include <opencv2/core/types.hpp>
#include <opencv2/core/persistence.hpp>

using namespace stereopi;

Camera::Camera(const std::string &paramFile){
	cv::FileStorage file{paramFile, cv::FileStorage::READ};

	_id = file["Id"];

	cv::FileNode matrix = file["Matrix"];

	for(double element : matrix){
		_matrix.push_back(element);
	}
	_matrix.reshape(0, 3);

	cv::FileNode distortion = file["Distortion"];
	for(double element : distortion){
		_distortion.push_back(element);
	}
}
