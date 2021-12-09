/*
 * calibration.h
 *
 *  Created on: 14 nov. 2020
 *      Author: admin
 */

#ifndef INCLUDE_CALIBRATION_H_
#define INCLUDE_CALIBRATION_H_

#include <string>

#include <opencv2/videoio.hpp>
#include <opencv2/core/types.hpp>

namespace calibration{
	enum Method{
		ChessBoard,
		ChessBoardSB,
		Circles
	};

	struct ChessBoardConfig{
		cv::Size size {7, 6};
		bool bFindSubPixel {false};
		cv::Size subPixelWinSize {5, 5};
		cv::Size subPixelZeroZone {-1, -1};
		cv::TermCriteria subPixelCriteria {cv::TermCriteria::COUNT, 150, 0};
	};

	struct Config
	{
		int cameraIndex {0};
		int nSamples {10};
		Method method {Method::ChessBoard};
		union {
			ChessBoardConfig chessBoard;
		}methodConfig {ChessBoardConfig()};

		Config() = default;
		Config(const Config &src) {memcpy(this, &src, sizeof(src));}
	};

	class Calibration
	{
	private:
		Config _configuration;
		cv::VideoCapture _camera;
		std::vector<cv::Mat> _points;

		void _loadChessBoardConfig(const cv::FileNode &chessBoardNode);
		void _loadChessBoardSBConfig(const cv::FileNode &chessBoardNode);
		void _loadCirclesConfig(const cv::FileNode &circlesNode);
		void _processFrameChessBoard(cv::Mat &img);
		void _processFrameChessBoardSB(cv::Mat &img);
		void _processFrameCircles(cv::Mat &img);
		void _readSize(const cv::FileNode &sizeNode, cv::Size &size);

	public:
		Calibration(std::string configFile);
		Config configuration() const { return _configuration; }
		void processFrame(cv::Mat &img);
	};
}//namespace calibration

#endif /* INCLUDE_CALIBRATION_H_ */
