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
		EXPECT_EQ(calibration.configuration().method, calibration::Method::ChessBoard);
		EXPECT_EQ(calibration.configuration().methodConfig.chessBoard.size, cv::Size(8, 7));
		EXPECT_EQ(calibration.configuration().methodConfig.chessBoard.bFindSubPixel, true);
		EXPECT_EQ(calibration.configuration().methodConfig.chessBoard.subPixelCriteria.type, cv::TermCriteria::COUNT + cv::TermCriteria::EPS);
		EXPECT_EQ(calibration.configuration().methodConfig.chessBoard.subPixelCriteria.epsilon, 0.01);
		EXPECT_EQ(calibration.configuration().methodConfig.chessBoard.subPixelCriteria.maxCount, 100);
		EXPECT_EQ(calibration.configuration().methodConfig.chessBoard.subPixelWinSize, cv::Size(4, 4));
		EXPECT_EQ(calibration.configuration().methodConfig.chessBoard.subPixelZeroZone, cv::Size(1, 1));
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
		EXPECT_EQ(calibration.configuration().method, calibration::Method::ChessBoard);
		EXPECT_EQ(calibration.configuration().methodConfig.chessBoard.size, cv::Size(8, 7));
		EXPECT_EQ(calibration.configuration().methodConfig.chessBoard.bFindSubPixel, true);
		EXPECT_EQ(calibration.configuration().methodConfig.chessBoard.subPixelCriteria.type, cv::TermCriteria::COUNT);
		EXPECT_EQ(calibration.configuration().methodConfig.chessBoard.subPixelCriteria.maxCount, 150);
		EXPECT_EQ(calibration.configuration().methodConfig.chessBoard.subPixelWinSize, cv::Size(5, 5));
		EXPECT_EQ(calibration.configuration().methodConfig.chessBoard.subPixelZeroZone, cv::Size(-1, -1));
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
		EXPECT_EQ(calibration.configuration().method, calibration::Method::ChessBoard);
		EXPECT_EQ(calibration.configuration().methodConfig.chessBoard.size, cv::Size(8, 7));
		EXPECT_EQ(calibration.configuration().methodConfig.chessBoard.bFindSubPixel, false);
	}
	catch(const std::string &str){
		ASSERT_TRUE(false) << "Exception thrown:" << str;
	}
	catch(...){
		ASSERT_TRUE(false) << "Unexpected exception thrown";
	}
}



