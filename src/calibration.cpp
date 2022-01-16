#include "calibration.h"

#include "algorithm"

#include <opencv2/opencv.hpp>
#include <opencv2/core/persistence.hpp>


using namespace calibration;

void readSize(const cv::FileNode &sizeNode, cv::Size &size){
	cv::Size sizeRead {sizeNode["Width"], sizeNode["Height"]};

	if(sizeRead != cv::Size{0, 0})
		size = sizeRead;
}


MethodConfig *MethodConfig::instance(const std::string &type){
	if(type == ChessBoardConfig::s_name())
		return new ChessBoardConfig();
	//TODO: Implement ChssboardSB and Circles
	return nullptr;
}

Calibration::Calibration(const std::string &configFile)
{
	cv::FileStorage file{configFile, cv::FileStorage::READ};

	if(!file.isOpened())
		throw std::string("Could not open file: [") + configFile + "]";

	_configuration.cameraIndex = file["CameraIndex"];
	_configuration.nSamples = file["NSamples"];
	cv::FileNode node = file["MethodConfig"];
	_configuration.methodConfig = std::shared_ptr<MethodConfig>{MethodConfig::instance(node["Type"])};
	_configuration.methodConfig->loadConfig(node);
	_points = std::vector<cv::Mat>(_configuration.nSamples);
}


void ChessBoardConfig::loadConfig(const cv::FileNode &chessBoardNode){
	readSize(chessBoardNode, size);

	cv::FileNode subpixelConfig = chessBoardNode["SubPixelConfig"];
	std::string strSubpixelConfig;

	if(subpixelConfig.isString()){
		strSubpixelConfig = subpixelConfig.string();
		std::for_each(strSubpixelConfig.begin(), strSubpixelConfig.end(), [](char &c){
			c=tolower(c);
		});
	}

	if(strSubpixelConfig != "none"){
		bFindSubPixel = true;
		readSize(subpixelConfig["WinSize"], subPixelWinSize);
		readSize(subpixelConfig["ZeroZone"], subPixelZeroZone);

		cv::FileNode subPixelMaxCount = subpixelConfig["MaxCount"];
		cv::FileNode subPixelEpsilon = subpixelConfig["Epsilon"];
		subPixelCriteria.type = not subPixelEpsilon.empty() and not subPixelMaxCount.empty()? cv::TermCriteria::COUNT + cv::TermCriteria::EPS :
		                                 not subPixelEpsilon.empty()? cv::TermCriteria::EPS :
		                                 cv::TermCriteria::COUNT;

		if(not subPixelMaxCount.empty())
			subPixelCriteria.maxCount = subPixelMaxCount;

		subPixelCriteria.epsilon = subPixelEpsilon;
	}
}


void Calibration::processFrame(cv::Mat &frame)
{
	_camera >> frame;

	if(_points.size() == 0 or (_points.size() < _configuration.nSamples and not _points.back().empty())){
		_points.emplace_back();
	}

	if(_points.back().empty()){
		_configuration.methodConfig->processFrame(frame, _points.back());
	}
}


void ChessBoardConfig::processFrame(cv::Mat &frame, cv::Mat &points){
	bool bFound = cv::findChessboardCorners(frame, size, points, 0);
	cv::drawChessboardCorners(frame, size, points, bFound);
}


void Calibration::saveConfigFile(const std::string &filename){
	cv::FileStorage file{filename, cv::FileStorage::WRITE};

	file << "CameraIndex" <<_configuration.cameraIndex;
	file << "NSamples" << _configuration.nSamples;
	file << "MethodConfig" << "{";
		file << "Type" << _configuration.methodConfig->name();
		_configuration.methodConfig->saveConfig(file);
	file << "}";
}


void ChessBoardConfig::saveConfig(cv::FileStorage &file) const{
	file << "Width" << size.width;
	file << "Height" << size.height;
	file << "SubPixelConfig" << "{";
		file << "WinSize" << "{";
			file << "Width" << subPixelWinSize.width;
			file << "Height" << subPixelWinSize.height;
		file << "}";

		file << "ZeroZone" << "{";
			file << "Width" << subPixelZeroZone.width;
			file << "Height" << subPixelZeroZone.height;
		file << "}";

		if(subPixelCriteria.type & cv::TermCriteria::COUNT)
			file << "MaxCount" << subPixelCriteria.maxCount;

		if(subPixelCriteria.type & cv::TermCriteria::EPS)
			file << "Epsilon" << subPixelCriteria.epsilon;
	file << "}";
}


