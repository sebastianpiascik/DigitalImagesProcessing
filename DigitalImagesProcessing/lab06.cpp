#include "labs.h"

#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <string>

using namespace cv;
using namespace std;

/* HSV Color Code:
	Color               Hue      Saturation    Value
	Red     ...     163 - 179    161 - 228   116 - 169
	Orange  ...     0 - 12       112 - 227   134 - 255
	Yellow  ...     22 - 34      38 - 244    160 - 255
	Green   ...     45 - 108     39 - 153    92 - 255
	Blue    ...     92 - 135     62 - 176    32 - 126
	Purple  ...     109 - 139    58 - 148    43 - 87
*/

int colorDetection() {
	VideoCapture vcap(0);
	if (!vcap.isOpened())
		return -1;

	Mat frame, frame_HSV, frame_threshold;
	const String window_detection_name = "---";
	// red
	int low_H = 0, low_S = 100, low_V = 100;
	int high_H = 10, high_S = 255, high_V = 255;
	RNG rng(12345);
	Rect bounding_rect;
	int color_index = 0;

	namedWindow(window_detection_name, 1);

	createTrackbar("Low H", window_detection_name, &low_H, 179); //Hue (0 - 179)
	createTrackbar("High H", window_detection_name, &high_H, 179);
	createTrackbar("Low S", window_detection_name, &low_S, 255); //Saturation (0 - 255)
	createTrackbar("High S", window_detection_name, &high_S, 255);
	createTrackbar("Low V", window_detection_name, &low_V, 255); //Value (0 - 255)
	createTrackbar("High V", window_detection_name, &high_V, 255);

	double duration;

	clock_t start = std::clock();

	while (vcap.read(frame)) {

		duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;

		if (duration > 5) {
			start = std::clock();
			if (color_index == 3) {
				color_index = 1;
			}
			else {
				color_index++;
			}
			cout << color_index << endl;
		}



		if (color_index == 1) {
			//blue
			setTrackbarPos("Low H", window_detection_name, 110);
			setTrackbarPos("High H", window_detection_name, 130);
			setTrackbarPos("Low S", window_detection_name, 50);
			setTrackbarPos("High S", window_detection_name, 255);
			setTrackbarPos("Low V", window_detection_name, 50);
			setTrackbarPos("High V", window_detection_name, 255);
		}

		if (color_index == 2) {
			//green
			setTrackbarPos("Low H", window_detection_name, 50);
			setTrackbarPos("High H", window_detection_name, 70);
			setTrackbarPos("Low S", window_detection_name, 50);
			setTrackbarPos("High S", window_detection_name, 255);
			setTrackbarPos("Low V", window_detection_name, 50);
			setTrackbarPos("High V", window_detection_name, 255);
		}

		if (color_index == 3) {
			//red
			setTrackbarPos("Low H", window_detection_name, 0);
			setTrackbarPos("High H", window_detection_name, 10);
			setTrackbarPos("Low S", window_detection_name, 100);
			setTrackbarPos("High S", window_detection_name, 255);
			setTrackbarPos("Low V", window_detection_name, 100);
			setTrackbarPos("High V", window_detection_name, 255);
		}

		cvtColor(frame, frame_HSV, COLOR_BGR2HSV);
		imshow("frame_HSV", frame_HSV);

		// Detect the object based on HSV Range Values
		inRange(frame_HSV, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), frame_threshold);

		//morphological opening (remove small objects from the foreground)
		erode(frame_threshold, frame_threshold, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(frame_threshold, frame_threshold, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		//morphological closing (fill small holes in the foreground)
		dilate(frame_threshold, frame_threshold, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		dilate(frame_threshold, frame_threshold, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;
		findContours(frame_threshold, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

		Mat drawing = Mat::zeros(frame_threshold.size(), CV_8UC3);
		double largest_area = 0;
		int largest_contour_index = 0;

		for (int i = 0; i < contours.size(); i++) // iterate through each contour. 
		{
			double a = contourArea(contours[i], false);  //  Find the area of contour
			if (a > largest_area) {
				largest_area = a;
				largest_contour_index = i;                //Store the index of largest contour
				bounding_rect = boundingRect(contours[i]); // Find the bounding rectangle for biggest contour
			}
		}

		//cout << largest_contour_index << ", contrours size:" << contours.size() << endl;

		Scalar color(255, 255, 255);
		drawContours(drawing, contours, largest_contour_index, color, CV_FILLED, 8, hierarchy); // Draw the largest contour using previously stored index.
		rectangle(frame, bounding_rect, Scalar(0, 255, 0), 1, 8, 0);

		if (contours.size() > 0 && contours.size() >= largest_contour_index) {
			Moments m = moments(contours[largest_contour_index]);
			if (m.m00 > 10) {
				int posX = m.m10 / m.m00;
				int posY = m.m10 / m.m00;
				if (posX >= 0 && posY >= 0) {
					std::ostringstream strs;
					strs << posX;
					strs << ",";
					strs << posY;
					std::string str = strs.str();
					putText(frame, str, Point(posX, posY), FONT_HERSHEY_PLAIN, 1, (0, 255, 0));
					//putText(frame, " Detection On ", Point2f(posX, posY), FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 255, 255), 2, 8, false);
				}
			}
		}

		imshow("drawing", drawing);
		imshow("frame", frame);

		char c = (char)waitKey(33);
		if (c == 27) break;
	}

	vcap.release();
	cvDestroyAllWindows();

	return 0;
}
