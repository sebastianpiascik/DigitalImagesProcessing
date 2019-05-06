#include "labs.h"

//#include "opencv2/imgcodecs.hpp"
//#include "opencv2/imgproc.hpp"
//#include "opencv2/videoio.hpp"
//#include <opencv2/highgui.hpp>
//#include <opencv2/video.hpp>

#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <ctype.h>

using namespace cv;
using namespace std;

bool initTracking = true;

static void resetTracking(int, void*)
{
	initTracking = true;
}

int motionDetectionOpticalFlow() {
	VideoCapture vcap(0);
	//VideoCapture vcap("bike.avi");

	Mat frame, oldGray, gray;

	vector<Point2f> corners, corners_prev;
	vector<uchar> status;
	vector<float> err;
	TermCriteria termcrit(TermCriteria::COUNT | TermCriteria::EPS, 20, 0.03);
	int maxCorners = 50, minDistance = 5, blockSize = 3;
	double qualityLevel = 0.001;

	if (!vcap.isOpened())
		return -1;

	namedWindow("Final window", 1);
	createTrackbar("maxCorners", "Final window", &maxCorners, 500, resetTracking);
	createTrackbar("minDistance", "Final window", &minDistance, 50, resetTracking);
	//createTrackbar("qualityLevel", "Final window", &qualityLevel, 1.0, resetTracking);

	while (vcap.read(frame)) {

		cvtColor(frame, gray, COLOR_BGR2GRAY);

		if (corners_prev.size() < 10 || initTracking) {
			goodFeaturesToTrack(gray, corners, maxCorners, qualityLevel, minDistance, Mat(), blockSize, 0, false, 0.04);
			initTracking = false;
		}
		else if (!corners_prev.empty()) {
			calcOpticalFlowPyrLK(oldGray, gray, corners_prev, corners, status, err, Size(31, 31), 3, termcrit, 0, 0.001);
		}

		for (int i = 0; i < corners_prev.size(); i++) {
			line(frame, corners[i], corners_prev[i], Scalar(255, 255, 255), 2, 1, 0);
			circle(frame, corners[i], 2, Scalar(255, 255, 255), 2, 1, 0);
		}

		
		swap(corners_prev, corners);
		corners.clear();
		gray.copyTo(oldGray);

		imshow("Final window", frame);
		//imshow("mask", mask);

		char c = (char)waitKey(33);
		if (c == 27) break;
	}

	vcap.release();
	cvDestroyAllWindows();

	return 0;
}

//int motionDetectionOpticalFlow() {
//	//VideoCapture vcap(0);
//	VideoCapture vcap("bike.avi");
//
//	Mat frame, oldGray, flow, gray, prevGray, image, mask;
//
//	vector<Point2f> corners, corners_prev;
//	vector<uchar> status;
//	vector<float> err;
//	Size winSize(31,31), subPixWinSize(10, 10);
//	TermCriteria termcrit(TermCriteria::COUNT | TermCriteria::EPS, 10, 0.03);
//	int maxCorners = 200, minDistance = 7, blockSize = 7, maxLevel = 3;
//	double qualityLevel = 0.2;
//
//	if (!vcap.isOpened())
//		return -1;
//
//	namedWindow("Final window", 1);
//	createTrackbar("maxCorners", "Final window", &maxCorners, 500, resetTracking);
//	createTrackbar("minDistance", "Final window", &minDistance, 50, resetTracking);
//	//createTrackbar("qualityLevel", "Final window", &qualityLevel, 1.0, resetTracking);
//
//	vcap.read(frame);
//	mask = Mat::zeros(frame.size(), frame.type());
//	while (vcap.read(frame)) {
//
//
//		if (corners_prev.size() < 5 || initTracking) {
//			cout << "init" << endl;
//
//			cvtColor(frame, oldGray, COLOR_BGR2GRAY);
//			goodFeaturesToTrack(oldGray, corners_prev, maxCorners, qualityLevel, minDistance, Mat(), blockSize);
//
//			if (corners_prev.size() >= 5) {
//				cornerSubPix(oldGray, corners_prev, subPixWinSize, Size(-1, -1), termcrit);
//				initTracking = false;
//			}
//		}
//
//		if(corners_prev.size() >= 5) {
//			cout << "calcOpticalFlow" << endl;
//			cvtColor(frame, gray, COLOR_BGR2GRAY);
//			calcOpticalFlowPyrLK(oldGray, gray, corners_prev, corners, status, err, winSize, maxLevel, termcrit, 0, 0.001);
//
//			size_t i, k;
//			for (i = k = 0; i < corners.size(); i++) {
//				if (!status[i]) continue;
//
//				//cout << "diff: " << norm(corners[i] - corners_prev[i]) << endl;
//				if (norm(corners[i] - corners_prev[i]) <= 0.001) continue;
//
//				corners_prev[k] = corners_prev[i];
//				corners[k] = corners[i];
//				err[k] = err[i];
//				k++;
//			}
//			corners_prev.resize(k);
//			corners.resize(k);
//			err.resize(k);
//
//			for (size_t i = 0; i < corners.size(); i++)
//			{
//				//circle(mask, corners[i], 3, Scalar(0, 0, 255), FILLED);
//				//line(mask, corners[i], corners_prev[i], Scalar(0, 255, 0), 2);
//				circle(frame, corners[i], 3, Scalar(0, 0, 255), FILLED);
//				line(frame, corners[i], corners_prev[i], Scalar(0, 255, 0), 2);
//			}
//
//			gray.copyTo(oldGray);
//			//corners_prev = corners;
//		}
//
//		imshow("Final window", frame);
//		//imshow("mask", mask);
//
//		char c = (char)waitKey(33);
//		if (c == 27) break;
//	}
//
//	vcap.release();
//	cvDestroyAllWindows();
//
//	return 0;
//}