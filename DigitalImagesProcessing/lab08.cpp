#include "labs.h"

#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>

#include<iostream>

using namespace cv;
using namespace std;

int mixtures = 100;
int history = 20;
int shadows = 1;
Ptr<BackgroundSubtractorMOG2> pMOG2;

static void trackbarSet(int, void*)
{
	pMOG2->setNMixtures(mixtures);
	pMOG2->setHistory(history);
	pMOG2 = createBackgroundSubtractorMOG2(history, 100, false);
}

int motionDetectionGauss() {
	//VideoCapture vcap(0);
	VideoCapture vcap("bike.avi");

	vector<Vec4i> lines;
	Mat frame, frame_with_lines, canny_frame, gray, background, foreground, fgMaskMOG2;

	if (!vcap.isOpened())
		return -1;


	pMOG2 = createBackgroundSubtractorMOG2(history, 100, false);
	namedWindow("Final window", 1);
	createTrackbar("history", "Final window", &history, 200);
	createTrackbar("mixtures", "Final window", &mixtures, 100, trackbarSet);

	while (vcap.read(frame)) {

		frame_with_lines = frame.clone();

		cvtColor(frame, gray, COLOR_BGR2GRAY);
		GaussianBlur(gray, gray, Size(21, 21), 0);

		pMOG2->apply(frame, foreground);
		pMOG2->getBackgroundImage(background);
		//pMOG2->setDetectShadows(static_cast<bool>(shadows));

		vector<vector<Point> > contours;
		findContours(foreground, contours, RETR_LIST, CHAIN_APPROX_NONE);

		for (int i = 0; i < contours.size(); i++)
			drawContours(frame, contours, i, Scalar(0,0,255), 1);

		imshow("Final window", frame);
		imshow("Foreground", foreground);
		imshow("Background", background);

		char c = (char)waitKey(33);
		if (c == 27) break;
	}

	vcap.release();
	cvDestroyAllWindows();

	return 0;
}

int houghTranform() {
	VideoCapture vcap(0);
	//VideoCapture vcap("bike.avi");

	Mat frame, gray, edges;

	int thresh = 50, minLineLenght = 10, maxLineGap = 50;

	if (!vcap.isOpened())
		return -1;

	namedWindow("Final window", 1);
	createTrackbar("Threshold", "Final window", &thresh, 255);
	createTrackbar("minLineLenghta", "Final window", &minLineLenght, 100);
	createTrackbar("maxLineGap", "Final window", &maxLineGap, 300);

	while (vcap.read(frame)) {

		cvtColor(frame, gray, COLOR_BGR2GRAY);

		// Znajdz kontury
		Canny(gray, edges, 50, 200);

		// Create a vector to store lines of the image
		vector<Vec4i> lines;
		// Apply Hough Transform
		HoughLinesP(edges, lines, 1, CV_PI / 180, thresh, minLineLenght, maxLineGap);
		// Draw lines on the image
		for (size_t i = 0; i < lines.size(); i++) {
			Vec4i l = lines[i];
			line(frame, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, LINE_AA);
		}

		imshow("edges", edges);
		imshow("Final window", frame);

		char c = (char)waitKey(33);
		if (c == 27) break;
	}

	vcap.release();
	cvDestroyAllWindows();

	return 0;
}