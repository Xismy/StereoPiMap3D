/*
 * XMLTests.cpp
 *
 *  Created on: 21 nov. 2021
 *      Author: usuario
 */

#include "gtest/gtest.h"

#include <string>
#include <iostream>

#include "calibration.h"

TEST(LoadXMLTest, ChessboardFull){
	try{
		calibration::Calibration calibration ("xml/ChessBoardFull.xml");
		EXPECT_EQ(calibration.configuration().cameraIndex, 2);
		EXPECT_EQ(calibration.configuration().nSamples, 7);
		calibration::ChessBoardConfig *cbConfig = dynamic_cast<calibration::ChessBoardConfig*>(calibration.configuration().methodConfig.get());
		ASSERT_TRUE(cbConfig != nullptr);
		EXPECT_EQ(cbConfig->size, cv::Size(8, 7));
		EXPECT_EQ(cbConfig->bFindSubPixel, true);
		EXPECT_EQ(cbConfig->subPixelCriteria.type, cv::TermCriteria::COUNT + cv::TermCriteria::EPS);
		EXPECT_EQ(cbConfig->subPixelCriteria.epsilon, 0.01);
		EXPECT_EQ(cbConfig->subPixelCriteria.maxCount, 100);
		EXPECT_EQ(cbConfig->subPixelWinSize, cv::Size(4, 4));
		EXPECT_EQ(cbConfig->subPixelZeroZone, cv::Size(1, 1));
	}
	catch(const std::string &str){
		ASSERT_TRUE(false) << "Exception thrown:" << str;
	}
	catch(...){
		ASSERT_TRUE(false) << "Unexpected exception thrown";
	}
}


TEST(LoadXMLTest, ChessboardDefaultSubpixel){
	try{
		calibration::Calibration calibration ("xml/ChessBoardDefaultSubpixel.xml");
		EXPECT_EQ(calibration.configuration().cameraIndex, 2);
		EXPECT_EQ(calibration.configuration().nSamples, 7);
		calibration::ChessBoardConfig *cbConfig = dynamic_cast<calibration::ChessBoardConfig*>(calibration.configuration().methodConfig.get());
		ASSERT_TRUE(cbConfig != nullptr);
		EXPECT_EQ(cbConfig->size, cv::Size(8, 7));
		EXPECT_EQ(cbConfig->bFindSubPixel, true);
		EXPECT_EQ(cbConfig->subPixelCriteria.type, cv::TermCriteria::COUNT);
		EXPECT_EQ(cbConfig->subPixelCriteria.maxCount, 150);
		EXPECT_EQ(cbConfig->subPixelWinSize, cv::Size(5, 5));
		EXPECT_EQ(cbConfig->subPixelZeroZone, cv::Size(-1, -1));
	}
	catch(const std::string &str){
		ASSERT_TRUE(false) << "Exception thrown:" << str;
	}
	catch(...){
		ASSERT_TRUE(false) << "Unexpected exception thrown";
	}
}


TEST(LoadXMLTest, ChessboardNoSubpixel){
	try{
		calibration::Calibration calibration ("xml/ChessBoardNoSubpixel.xml");
		EXPECT_EQ(calibration.configuration().cameraIndex, 2);
		EXPECT_EQ(calibration.configuration().nSamples, 7);
		calibration::ChessBoardConfig *cbConfig = dynamic_cast<calibration::ChessBoardConfig*>(calibration.configuration().methodConfig.get());
		ASSERT_TRUE(cbConfig != nullptr);
		EXPECT_EQ(cbConfig->size, cv::Size(8, 7));
		EXPECT_EQ(cbConfig->bFindSubPixel, false);
	}
	catch(const std::string &str){
		ASSERT_TRUE(false) << "Exception thrown:" << str;
	}
	catch(...){
		ASSERT_TRUE(false) << "Unexpected exception thrown";
	}
}


TEST(ConfigComparatation, ChessboardFull){
	try{
		calibration::Calibration calibration1 ("xml/ChessBoardFull.xml");
		calibration::Calibration calibration2 ("xml/ChessBoardFull.xml");

		EXPECT_TRUE(calibration1.configuration().methodConfig->equals(*(calibration2.configuration().methodConfig)));
		EXPECT_EQ(calibration1.configuration(), calibration2.configuration());
	}
	catch(const std::string &str){
		ASSERT_TRUE(false) << "Exception thrown:" << str;
	}
	catch(...){
		ASSERT_TRUE(false) << "Unexpected exception thrown";
	}
}


TEST(LoadXMLTest, SaveConfigFile){
	try{
		calibration::Calibration calibration ("xml/ChessBoardFull.xml");
		calibration.saveConfigFile("tmp/ChessBoardFull.xml");
		calibration::Calibration writtenCalibration("tmp/ChessBoardFull.xml");
		EXPECT_EQ(calibration.configuration(), writtenCalibration.configuration());
	}
	catch(const std::string &str){
		ASSERT_TRUE(false) << "Exception thrown:" << str;
	}
	catch(...){
		ASSERT_TRUE(false) << "Unexpected exception thrown";
	}
}
