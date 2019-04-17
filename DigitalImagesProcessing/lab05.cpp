#include "labs.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string>
#include <sstream>
#include <iostream>
#include <cstdio>
#include <ctime>

using namespace cv;
using namespace std;

int shapeDetection() {
	VideoCapture vcap(0);
	Mat frame, gauss_blur, gray, edge, draw, detected_edges, dst, grad_x, grad_y, mag, abs_grad_x, abs_grad_y, grad;

	int lowThreshold = 50;
	int const max_lowThreshold = 200;
	int ratio = 3;
	int kernel_size = 3;
	int sobelX_size = 3;
	int sobelY_size = 3;

	RNG rng(12345);

	if (!vcap.isOpened())
		return -1;

	namedWindow("Final window", 1);

	createTrackbar("Min Threshold:", "Final window", &lowThreshold, max_lowThreshold);
	createTrackbar("Gauss size:", "Final window", &kernel_size, 9);
	setTrackbarMin("Gauss size:", "Final window", 3);
	createTrackbar("Sobel X size:", "Final window", &sobelX_size, 9);
	setTrackbarMin("Sobel X size:", "Final window", 1);
	createTrackbar("Sobel Y size:", "Final window", &sobelY_size, 9);
	setTrackbarMin("Sobel Y size:", "Final window", 1);

	while (vcap.read(frame)) {

		if (kernel_size % 2 == 0) {
			kernel_size--;
			setTrackbarPos("Gauss size:", "Final window", kernel_size);
		}

		if (sobelX_size % 2 == 0) {
			sobelX_size--;
			setTrackbarPos("Sobel X size:", "Final window", sobelX_size);
		}

		if (sobelY_size % 2 == 0) {
			sobelY_size--;
			setTrackbarPos("Sobel Y size:", "Final window", sobelY_size);
		}

		// Gaussian smoothing
		GaussianBlur(frame, gauss_blur, Size(kernel_size, kernel_size), 0, 0, BORDER_DEFAULT);
		imshow("GaussianBlur", gauss_blur);

		// Convert it to gray
		cvtColor(gauss_blur, gray, CV_BGR2GRAY);
		imshow("Gray", gray);

		// Canny detector
		Canny(gray, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size);
		imshow("canny", detected_edges);

		// Gradient X
		Sobel(detected_edges, grad_x, CV_32F, 1, 0, sobelX_size);
		//threshold(grad_x, grad_x, 127, 255, THRESH_BINARY);
		imshow("sobel x", grad_x);
		// Gradient Y
		Sobel(detected_edges, grad_y, CV_32F, 0, 1, sobelY_size);
		//threshold(grad_y, grad_y, 127, 255, THRESH_BINARY);
		imshow("sobel y", grad_y);

		// Total Gradient (approximate)
		//convertScaleAbs(grad_x, abs_grad_x);
		//convertScaleAbs(grad_y, abs_grad_y);
		//addWeighted(grad_x, 0.5, grad_y, 0.5, 0, grad);
		magnitude(grad_x, grad_y, grad);
		threshold(grad, grad, 127, 255, THRESH_BINARY);
		imshow("Gradient", grad);


		Mat drawing = Mat::zeros(detected_edges.size(), CV_8UC3);

		Mat angles;
		// calculates the rotation angle of each 2D vector, save degrees (true)
		phase(grad_x, grad_y, angles, true);

		for (int i = 0; i < detected_edges.rows; i++)
		{
			for (int j = 0; j < detected_edges.cols; j++)
			{
				if (angles.at<float>(i, j) > 45 && angles.at<float>(i, j) <= 135) {
					drawing.at<Vec3b>(i, j)[0] = 255; // b
					drawing.at<Vec3b>(i, j)[1] = 255; // g
					drawing.at<Vec3b>(i, j)[2] = 255; // r
				}
				if (angles.at<float>(i, j) > 135 && angles.at<float>(i, j) <= 225) {
					drawing.at<Vec3b>(i, j)[0] = 0;
					drawing.at<Vec3b>(i, j)[1] = 0;
					drawing.at<Vec3b>(i, j)[2] = 255;
				}
				if (angles.at<float>(i, j) > 225 && angles.at<float>(i, j) <= 315) {
					drawing.at<Vec3b>(i, j)[0] = 0;
					drawing.at<Vec3b>(i, j)[1] = 255;
					drawing.at<Vec3b>(i, j)[2] = 0;
				}
				if ((angles.at<float>(i, j) > 0 && angles.at<float>(i, j) <= 45) || (angles.at<float>(i, j) > 315 && angles.at<float>(i, j) <= 360)) {
					drawing.at<Vec3b>(i, j)[0] = 255;
					drawing.at<Vec3b>(i, j)[1] = 0;
					drawing.at<Vec3b>(i, j)[2] = 0;
				}
			}
		}
		imshow("Colors", drawing);


		//vector<vector<Point> > contours;
		//vector<Vec4i> hierarchy;
		//// Find contours
		//findContours(detected_edges, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

		//// Draw contours
		//Mat drawing = Mat::zeros(detected_edges.size(), CV_8UC3);
		//for (int i = 0; i < contours.size(); i++)
		//{
		//	cout << contours[0] << endl;
		//	Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		//	drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
		//}

		//// Show in a window
		//namedWindow("Contours", CV_WINDOW_AUTOSIZE);
		//imshow("Contours", drawing);


		dst = Scalar::all(0);
		frame.copyTo(dst, detected_edges);

		imshow("Final window", dst);

		char c = (char)waitKey(33);
		if (c == 27) break;
	}

	vcap.release();
	cvDestroyAllWindows();

	return 0;
}


