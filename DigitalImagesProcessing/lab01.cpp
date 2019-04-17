#include "labs.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

#include <iostream>

using namespace cv;
using namespace std;

int lab01() {
	try {
		Mat image;
		VideoCapture cap;
		cap.open(0);

		namedWindow("window", CV_WINDOW_AUTOSIZE);

		double dWidth = cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
		double dHeight = cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);



		while (cvWaitKey(1) != 27) {
			cap >> image;
			imshow("window", image);

		}
		return 0;
	}
	catch (Exception& e)
	{
		const char* err_msg = e.what();
		std::cout << "====> exception caught: " << err_msg << std::endl;
		while (cvWaitKey(1) != 27) {}
	}
	return 0;
}