#include "calibration.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core/persistence.hpp>


using namespace calibration;
Calibration::Calibration(std::string configFile)
{
	cv::FileStorage file(configFile, cv::FileStorage::READ);

	if(!file.isOpened())
		throw std::string("Could not open file: [") + configFile + "]";

	_configuration.cameraIndex = file["CameraIndex"];
	_configuration.nSamples = file["NSamples"];
	cv::FileNode node = file["MethodConfig"];

	if(node["Type"] == "ChessBoard"){
		_configuration.method = Method::ChessBoard;
		_loadChessBoardConfig(node);
	}
	else if(node["Type"] == "ChessBoardSB"){
		_configuration.method = Method::ChessBoardSB;
		_loadChessBoardSBConfig(node);
	}
	else if(node["Type"] == "Circles"){
		_configuration.method = Method::Circles;
		_loadCirclesConfig(node);
	}
	else{
		std::cerr << "Method not specified, using ChessBoard";
	}

	_points = std::vector<cv::Mat>(_configuration.nSamples);
}


void Calibration::_loadChessBoardConfig(const cv::FileNode &chessBoardNode){
	ChessBoardConfig &chessBoardConfig = _configuration.methodConfig.chessBoard;
	_readSize(chessBoardNode, chessBoardConfig.size);

	cv::FileNode subpixelConfig = chessBoardNode["SubPixelConfig"];

	if(not subpixelConfig.empty()){
		chessBoardConfig.bFindSubPixel = true;
		_readSize(subpixelConfig["WinSize"], chessBoardConfig.subPixelWinSize);
		_readSize(subpixelConfig["ZeroZone"],chessBoardConfig.subPixelZeroZone);

		cv::FileNode subPixelMaxCount = subpixelConfig["MaxCount"];
		cv::FileNode subPixelEpsilon = subpixelConfig["Epsilon"];
		chessBoardConfig.subPixelCriteria.type = not subPixelEpsilon.empty() and not subPixelMaxCount.empty()? cv::TermCriteria::COUNT + cv::TermCriteria::EPS :
				                                 not subPixelEpsilon.empty()? cv::TermCriteria::EPS :
		                                         cv::TermCriteria::COUNT;

		if(not subPixelMaxCount.empty())
			chessBoardConfig.subPixelCriteria.maxCount = subPixelMaxCount;

		chessBoardConfig.subPixelCriteria.epsilon = subPixelEpsilon;
	}
}


void Calibration::_loadChessBoardSBConfig(const cv::FileNode &chessBoardNode){
	//TODO
	std::cerr << "Not implemented yet";
}


void Calibration::_loadCirclesConfig(const cv::FileNode &circlesNode){
	//TODO
	std::cerr << "Not implemented yet";
}


void Calibration::processFrame(cv::Mat &frame)
{
	_camera >> frame;

	if(_points.size() == 0 or (_points.size() < _configuration.nSamples and not _points.back().empty())){
		_points.emplace_back();
	}

	if(_points.back().empty()){
		switch(_configuration.method){
		case Method::ChessBoard:
			_processFrameChessBoard(frame);
			break;
		case Method::ChessBoardSB:
			_processFrameChessBoardSB(frame);
			break;
		case Method::Circles:
			_processFrameCircles(frame);
			break;
		}
	}
}


void Calibration::_processFrameChessBoard(cv::Mat &frame){
	bool bFound = cv::findChessboardCorners(frame, _configuration.methodConfig.chessBoard.size, _points.back(), 0);
	cv::drawChessboardCorners(frame, _configuration.methodConfig.chessBoard.size, _points.back(), bFound);
}


void Calibration::_processFrameChessBoardSB(cv::Mat &frame){
	//TODO
	std::cerr << "Not implemented yet";
}


void Calibration::_processFrameCircles(cv::Mat &frame){
	//TODO
	std::cerr << "Not implemented yet";
}


void Calibration::_readSize(const cv::FileNode &sizeNode, cv::Size &size){
	cv::Size sizeRead {sizeNode["Width"], sizeNode["Height"]};

	if(sizeRead != cv::Size{0, 0})
		size = sizeRead;
}
