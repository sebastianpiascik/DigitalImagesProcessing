#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <chrono> 
#include <ctime> 
#include <iostream>
#include <stdio.h> 
#include <stdlib.h> 
#include <time.h> 
#include <windows.h>
#include <wchar.h>

using namespace std;
using namespace cv;

int lab12()
{
	VideoCapture vcap(0);

	if (!vcap.isOpened())
		return -1;

	int threshNumber = 75;
	namedWindow("Final window", 1);
	createTrackbar("thresh", "Final window", &threshNumber, 255);
	Mat firstFrame, frame, frame1, frame2, gray1, gray2, dst, thresh, gaussBlur;
	Rect bounding_rect;
	RNG rng(12345);
	boolean paused = false, newFile = true;

	int frame_width = vcap.get(CV_CAP_PROP_FRAME_WIDTH);
	int frame_height = vcap.get(CV_CAP_PROP_FRAME_HEIGHT);
	VideoWriter video("tmp.avi", CV_FOURCC('M', 'J', 'P', 'G'), 10, Size(frame_width, frame_height), true);

	SYSTEMTIME lt = { 0 };

	std::clock_t start = std::clock();
	double duration;


	while (vcap.read(frame1)) {
		GetLocalTime(&lt);

		if (newFile) {
			stringstream ss;
			ss << lt.wHour;
			ss << lt.wMinute;
			ss << lt.wSecond;
			ss << "_";
			ss << lt.wYear;
			ss << lt.wMonth;
			ss << lt.wDay;
			string time = ss.str();
			string filename = "motion_" + time + ".avi";
			cout << filename << endl;
			video.release();
			video.open(filename, CV_FOURCC('M', 'J', 'P', 'G'), 10, Size(frame_width, frame_height), true);
			newFile = false;
			start = std::clock();
		}

		if (!paused) {
			cvtColor(frame1, gray1, COLOR_BGR2GRAY);

			vcap.read(frame2);
			cvtColor(frame2, gray2, COLOR_BGR2GRAY);

			absdiff(gray1, gray2, dst);

			GaussianBlur(dst, gaussBlur, Size(5, 5), 0);

			threshold(dst, thresh, threshNumber, 255, THRESH_BINARY);

			vector<vector<Point>> contours;
			vector<Vec4i> hierarchy;
			findContours(thresh, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

			double largest_area = 0;
			int largest_contour_index = 0;

			for (int i = 0; i < contours.size(); i++) 
			{
				double a = contourArea(contours[i], false);
				if (a > largest_area) {
					largest_area = a;
					largest_contour_index = i;
				}
			}

			if (contours.size() > 0 && contours.size() >= largest_contour_index) {
				start = std::clock();
				stringstream ss;
				ss << lt.wHour;
				ss << ":";
				ss << lt.wMinute;
				ss << ":";
				ss << lt.wSecond;
				ss << " ";
				ss << lt.wYear;
				ss << "/";
				ss << lt.wMonth;
				ss << "/";
				ss << lt.wDay;
				string str = ss.str();
				cout << str << endl;
				putText(frame1, str, Point(20, 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 0));
				video.write(frame1);
			}
			else {
				duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
				if (duration > 2) {
					video.release();
					newFile = true;
				}
			}

			imshow("thresh", thresh);
			imshow("frame1", frame1);
		}
		else {
			start = std::clock();
		}



		char c = (char)waitKey(33);
		if (c == 27) {
			break;
		}
		if (c == 32) {
			if (paused) {
				paused = false;
			}
			else {
				paused = true;
				destroyWindow("thresh");
			}
		}
	}

	vcap.release();
	video.release();
	cvDestroyAllWindows();

	return 0;
}