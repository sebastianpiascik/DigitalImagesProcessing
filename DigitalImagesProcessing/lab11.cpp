#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <stdio.h>
#include <windows.h>

using namespace std;
using namespace cv;

int motionDetectAnother()
{
	VideoCapture vcap(0);

	if (!vcap.isOpened())
		return -1;

	int threshNumber = 50;
	namedWindow("Final window", 1);
	createTrackbar("thresh", "Final window", &threshNumber, 255);
	Mat firstFrame, frame, frame1, frame2, gray1, gray2, dst, thresh, gaussBlur;
	Rect bounding_rect;
	RNG rng(12345);
	boolean paused = false;


	while (vcap.read(frame1)) {

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

			vector<vector<Point> >hull(contours.size());
			for (int i = 0; i < contours.size(); i++) // iterate through each contour. 
			{
				double a = contourArea(contours[i], false);
				if (a > largest_area) {
					largest_area = a;
					largest_contour_index = i;
				}
			}

			if (contours.size() > 0 && contours.size() >= largest_contour_index) {
				bounding_rect = boundingRect(contours[largest_contour_index]);
				convexHull(contours[largest_contour_index], hull[largest_contour_index]);
				Scalar color(255, 255, 255);
				//drawContours(frame1, contours, largest_contour_index, color);
				drawContours(frame1, hull, largest_contour_index, color);
				rectangle(frame1, bounding_rect, Scalar(0, 255, 0), 1, 8, 0);

				Moments m = moments(contours[largest_contour_index]);
				if (m.m00 > 10) {
					int posX = m.m10 / m.m00;
					int posY = m.m01 / m.m00;
					if (posX >= 0 && posY >= 0) {
						std::ostringstream strs;
						strs << posX;
						strs << ",";
						strs << posY;
						std::string str = strs.str();
						putText(frame1, str, Point(posX, posY), FONT_HERSHEY_PLAIN, 1, (0, 255, 0));
						//putText(frame, " Detection On ", Point2f(posX, posY), FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 255, 255), 2, 8, false);
					}
				}
			}
			//imshow("gray1", gray1);
			imshow("absdiff", dst);
			imshow("gaussBlur", gaussBlur);
			imshow("thresh", thresh);
			imshow("frame1", frame1);
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
				destroyWindow("absdiff");
				destroyWindow("gaussBlur");
				destroyWindow("thresh");
			}
		}
	}

	vcap.release();
	cvDestroyAllWindows();

	return 0;
}