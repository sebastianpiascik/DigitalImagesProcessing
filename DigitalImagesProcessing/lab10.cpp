#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

int faceDetect()
{
	VideoCapture vcap(0);
	Mat frame, frame_gray;
	Mat firstFace, secoundFace;

	if (!vcap.isOpened())
		return -1;

	// Load Face cascade (.xml file)
	CascadeClassifier face_cascade;
	face_cascade.load("../../opencv/sources/data/haarcascades/haarcascade_frontalface_alt.xml");

	CascadeClassifier eyes_cascade;
	eyes_cascade.load("../../opencv/sources/data/haarcascades/haarcascade_eye_tree_eyeglasses.xml");
	// 10 2, 30(rozmiar)

	int scaleFactorInt = 11;
	int minNeighbors = 2;
	int minSize = 30;
	int blur = 0;
	int rectangle = 0;
	int swap = 0;
	namedWindow("Final window", 1);
	createTrackbar("scaleFactor", "Final window", &scaleFactorInt, 20);
	createTrackbar("minNeighbors", "Final window", &minNeighbors, 5);
	createTrackbar("minSize", "Final window", &minSize, 50);
	createTrackbar("blur", "Final window", &blur, 1);
	createTrackbar("rectangle", "Final window", &rectangle, 1);
	createTrackbar("swap", "Final window", &swap, 1);

	double scaleFactor = (double)scaleFactorInt / 10;

	while (vcap.read(frame)) {

		cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
		equalizeHist(frame_gray, frame_gray);

		// Detect faces
		std::vector<Rect> faces;
		face_cascade.detectMultiScale(frame, faces, scaleFactor, minNeighbors, 0 | CV_HAAR_SCALE_IMAGE, Size(minSize, minSize));


		if (blur == 1) {
			for (vector<Rect>::const_iterator r = faces.begin(); r != faces.end(); r++)
			{
				Mat roi = frame(*r);
				GaussianBlur(roi, roi, Size(55, 55), 0);
			}
		}
		else {
			// Draw circles on the detected faces
			for (size_t i = 0; i < faces.size(); i++)
			{
				Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
				ellipse(frame, center, Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, Scalar(255, 255, 255), 4);
				Mat faceROI = frame_gray(faces[i]);
				// In each face, detect eyes
				std::vector<Rect> eyes;
				eyes_cascade.detectMultiScale(faceROI, eyes);
				for (size_t j = 0; j < eyes.size(); j++)
				{
					Point eye_center(faces[i].x + eyes[j].x + eyes[j].width / 2, faces[i].y + eyes[j].y + eyes[j].height / 2);
					int radius = cvRound((eyes[j].width + eyes[j].height)*0.25);
					//cv::circle(frame, eye_center, radius, Scalar(255, 0, 0), 4);
				}
				// draw rectangle
				if (eyes.size() >= 1 && rectangle == 1) {
					cv::rectangle(frame, Point(faces[i].x, faces[i].y), Point(faces[i].x + faces[i].width, faces[i].y + faces[i].height / 2), cv::Scalar(0, 0, 0), CV_FILLED);
				}
				
			}
		}

		if (faces.size() > 1 && faces[0].width > 100 && swap == 1) {
			firstFace = frame(faces[0]);
			secoundFace = frame(faces[1]);
			// resize face 
			resize(firstFace, firstFace, cv::Size(secoundFace.cols, secoundFace.rows), 0, 0);
			//copy face
			Rect WhereRec(faces[1].x, faces[1].y, firstFace.cols, firstFace.rows);
			firstFace.copyTo(frame(WhereRec));
		}

		imshow("Final window2", frame);

		char c = (char)waitKey(33);
		if (c == 27) break;
	}

	vcap.release();
	cvDestroyAllWindows();
	return 0;
}