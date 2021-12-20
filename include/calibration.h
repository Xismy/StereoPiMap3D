/*
 * calibration.h
 *
 *  Created on: 14 nov. 2020
 *      Author: admin
 */

#ifndef INCLUDE_CALIBRATION_H_
#define INCLUDE_CALIBRATION_H_

#include <string>
#include <map>
#include <memory>

#include <opencv2/videoio.hpp>
#include <opencv2/core/types.hpp>

namespace calibration{
	struct MethodConfig{
		static MethodConfig *instance(const std::string &type);

		bool operator==(const MethodConfig &other) const{ return this->equals(other); }
		bool operator!=(const MethodConfig &other) const{ return not (*this == other); }

		virtual std::string name() = 0;
		virtual void loadConfig(const cv::FileNode &file) = 0;
		virtual void saveConfig(cv::FileStorage &file) const = 0;
		virtual void processFrame(cv::Mat &frame, cv::Mat &points) = 0;
		virtual bool equals(const MethodConfig &other) const = 0;
		virtual ~MethodConfig() = default;
	};

	struct ChessBoardConfig : MethodConfig{
		cv::Size size {7, 6};
		bool bFindSubPixel {false};
		cv::Size subPixelWinSize {5, 5};
		cv::Size subPixelZeroZone {-1, -1};
		cv::TermCriteria subPixelCriteria {cv::TermCriteria::COUNT, 150, 0};

		static std::string s_name() { return "ChessBoard"; }
		std::string name() { return s_name(); }
		void loadConfig(const cv::FileNode &file) override;
		void saveConfig(cv::FileStorage &file) const override;
		void processFrame(cv::Mat &frame, cv::Mat &points) override;

		bool equals(const MethodConfig &other) const {
			const ChessBoardConfig *pOther = dynamic_cast<const ChessBoardConfig*>(&other);

			return pOther != nullptr
			   and size == pOther->size
			   and bFindSubPixel == pOther->bFindSubPixel
			   and subPixelWinSize == pOther->subPixelWinSize
			   and subPixelZeroZone == pOther->subPixelZeroZone
			   and subPixelCriteria.type == pOther->subPixelCriteria.type
			   and subPixelCriteria.maxCount == pOther->subPixelCriteria.maxCount
			   and subPixelCriteria.epsilon == pOther->subPixelCriteria.epsilon;
		}
	};


	struct Config
	{
		int cameraIndex {0};
		int nSamples {10};
		std::shared_ptr<MethodConfig> methodConfig = std::make_shared<ChessBoardConfig>();

		bool operator==(const Config &other) const {
			return cameraIndex == other.cameraIndex and nSamples == other.nSamples and *methodConfig == *(other.methodConfig);
		}

		bool operator!=(const Config &other) const { return not (*this == other); }
	};


	class Calibration
	{
	private:
		Config _configuration;
		cv::VideoCapture _camera;
		std::vector<cv::Mat> _points;

	public:
		Calibration(const std::string &configFile);
		Config configuration() const { return _configuration; }
		void processFrame(cv::Mat &img);
		void saveConfigFile(const std::string &filename);
	};
}//namespace calibration

#endif /* INCLUDE_CALIBRATION_H_ */
