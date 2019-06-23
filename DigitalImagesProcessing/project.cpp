#include <opencv2/core/core.hpp>
#include "opencv2/objdetect/objdetect.hpp"
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <time.h> 
#include <windows.h>
#include <chrono> 
#include <ctime> 

#define CVUI_IMPLEMENTATION
#include "cvui.h"

#define WINDOW_NAME	"Project"

using namespace std;
using namespace cv;

int main(int argc, const char *argv[])
{
	VideoCapture vcap(0);
	//VideoCapture vcap("projektVideo.avi");

	if (!vcap.isOpened())
		return -1;

	CascadeClassifier face_cascade;
	face_cascade.load("../../opencv/sources/data/haarcascades/haarcascade_frontalface_alt.xml");

	CascadeClassifier eyes_cascade;
	eyes_cascade.load("../../opencv/sources/data/haarcascades/haarcascade_eye_tree_eyeglasses.xml");

	Mat window = Mat(680, 950, CV_8UC3);
	Mat frame, frame_gray, firstFace, secoundFace, choosenFace, choosenFaceCopy, resized;
	Mat faceImages[10];

	int frame_width = vcap.get(CV_CAP_PROP_FRAME_WIDTH);
	int frame_height = vcap.get(CV_CAP_PROP_FRAME_HEIGHT);
	VideoWriter video("tmp.avi", CV_FOURCC('M', 'J', 'P', 'G'), 10, Size(frame_width, frame_height), true);

	double scaleFactor = 1.1;
	int minNeighbors = 2;
	int minSize = 30;
	int numberFaces = 0;
	bool blurFace = false, 
		rectangleFace=false, 
		swapFace=false,
		drawCircleFace = false,
		drawCircleEyes = false,
		recordVideo = false,
		staticPhoto = true,
		pickedPhoto = false;

	int dynamicPhotoId;


	double videoDuration = 0;
	clock_t start;

	// Init cvui and tell it to create a OpenCV window, i.e. cv::namedWindow(WINDOW_NAME).
	cvui::init(WINDOW_NAME);

	while (waitKey(20) != 27) {
		vcap.read(frame);

		if (!frame.data) {
			vcap.open("projektVideo.avi");
			vcap.read(frame);
		}

		// Fill the frame with color
		window = Scalar(49, 52, 49);

		// Make gray frame
		cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
		equalizeHist(frame_gray, frame_gray);

		// Detect faces
		std::vector<Rect> faces;
		face_cascade.detectMultiScale(frame, faces, scaleFactor, minNeighbors, 0 | CV_HAAR_SCALE_IMAGE, Size(minSize, minSize));

		// Assign number of faces
		numberFaces = faces.size();

		for (int i = 0; i < numberFaces; i++)
		{
			faceImages[i] = frame(faces[i]);
		}

		// Show available faces
		if (numberFaces > 0) {
			int positionLeft = 10;
			for (int i = 0; i < numberFaces; i++)
			{
				resize(faceImages[i], resized, cv::Size(70, 70), 0, 0);
				if (cvui::button(window, positionLeft, 520, resized, resized, resized)) {
					faceImages[i].copyTo(choosenFace);
					faceImages[i].copyTo(choosenFaceCopy);
					dynamicPhotoId = i;
					pickedPhoto = true;
					imshow("Display window", choosenFace);
				}
				positionLeft += 80;
			}
			cvui::checkbox(window, 10, 600, "Statyczne zdjecie", &staticPhoto);
			cvui::checkbox(window, 200, 600, "Wybrano zdjecie", &pickedPhoto);

			// Save faces
			if (cvui::button(window, 10, 630, "Zapisz zdjecia")) {
				for (int i = 0; i < numberFaces; i++)
				{
					stringstream ss;
					ss << "photo_" << i << ".jpg";
					string filename = ss.str();
					imwrite(filename, faceImages[i]);
				}
			}
		}

		for (int i = 0; i < numberFaces; i++)
		{
			// Draw circles on the detected faces (checkbox)
			if (drawCircleFace) {
				Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
				ellipse(frame, center, Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, Scalar(255, 255, 255), 4);
			}

			// In each face, detect eyes and draw circles (checkbox)
			Mat faceROI = frame_gray(faces[i]);
			std::vector<Rect> eyes;
			eyes_cascade.detectMultiScale(faceROI, eyes);
			if (drawCircleEyes) {
				for (int j = 0; j < eyes.size(); j++)
				{
					Point eye_center(faces[i].x + eyes[j].x + eyes[j].width / 2, faces[i].y + eyes[j].y + eyes[j].height / 2);
					int radius = cvRound((eyes[j].width + eyes[j].height)*0.25);
					cv::circle(frame, eye_center, radius, Scalar(255, 0, 0), 4);
				}
			}

			// Draw rectangle (chechbox)
			if (rectangleFace) {
				if (eyes.size() > 0) {
					cv::rectangle(frame, Point(faces[i].x, faces[i].y + eyes[0].y), Point(faces[i].x + faces[i].width, faces[i].y + faces[i].height / 2), cv::Scalar(0, 0, 0), CV_FILLED);
				}
				else {
					cv::rectangle(frame, Point(faces[i].x, faces[i].y), Point(faces[i].x + faces[i].width, faces[i].y + faces[i].height / 2), cv::Scalar(0, 0, 0), CV_FILLED);
				}
			}

			// Blur face (checkbox)
			if (blurFace) {
				Mat roi = frame(faces[i]);
				GaussianBlur(roi, roi, Size(55, 55), 0);
			}
		}

		if (swapFace) {
			if (numberFaces > 0) {
				// Take static photo from file
				if (!pickedPhoto) {
					Mat imageFromFile = imread("avatar.png");
					choosenFace = imageFromFile;
				}
				else {
					choosenFace = choosenFaceCopy;
				}
				if (staticPhoto) {
					// Swap all images
					for (int i = 0; i < numberFaces; i++)
					{
						resize(choosenFace, choosenFace, cv::Size(faceImages[i].cols, faceImages[i].rows), 0, 0);
						Rect WhereRec(faces[i].x, faces[i].y, choosenFace.cols, choosenFace.rows);
						choosenFace.copyTo(frame(WhereRec));
					}
				}
				else {
					// Swap images (exclude id)
					for (int i = 0; i < numberFaces; i++)
					{
						if (i != dynamicPhotoId) {
							choosenFace = faceImages[dynamicPhotoId];
							resize(choosenFace, choosenFace, cv::Size(faceImages[i].cols, faceImages[i].rows), 0, 0);
							Rect WhereRec(faces[i].x, faces[i].y, choosenFace.cols, choosenFace.rows);
							choosenFace.copyTo(frame(WhereRec));
						}
					}
				}
				
			}
		}

		resize(frame, frame, cv::Size(640, 480), 0, 0);
		cvui::image(window, 10, 10, frame);

		cvui::printf(window, 660, 20, "Ilosc wykrytych twarzy = %d", numberFaces);

		cvui::checkbox(window, 660, 75, "Blur face", &blurFace);
		cvui::checkbox(window, 660, 100, "Rectangle face", &rectangleFace);
		cvui::checkbox(window, 660, 125, "Swap face", &swapFace);
		cvui::checkbox(window, 660, 150, "Draw circle face", &drawCircleFace);
		cvui::checkbox(window, 660, 175, "Draw circle eye", &drawCircleEyes);

		cvui::text(window, 660, 225, "Scale factor");
		cvui::trackbar(window, 660, 240, 220, &scaleFactor, (double)1.0, (double)2.0);
		cvui::text(window, 660, 295, "Min neighbours");
		cvui::trackbar(window, 660, 310, 220, &minNeighbors, 1, 5);
		cvui::text(window, 660, 365, "Min size");
		cvui::trackbar(window, 660, 380, 220, &minSize, 10, 50);

		// Record video
		if (cvui::button(window, 660, 490, "Rozpocznij nagrywanie")) {
			recordVideo = true;
			video.open("projektVideo2.avi", CV_FOURCC('M', 'J', 'P', 'G'), 10, Size(frame_width, frame_height), true);
		}
		if (cvui::button(window, 660, 520, "Zakoncz nagrywanie")) {
			video.release();
			recordVideo = false;
		}
		if (recordVideo) {
			video.write(frame);
			cvui::printf(window, 660, 570, "Nagrywanie...", videoDuration);
		}

		if (cvui::button(window, 660, 630, "Kamera")) {
			vcap.open(0);
			vcap.read(frame);
		}
		if (cvui::button(window, 750, 630, "Film")) {
			vcap.open("projektVideo.avi");
			vcap.read(frame);
		}
		

		cvui::update();
		imshow(WINDOW_NAME, window);

		/*if (waitKey(20) == 27) {
			break;
		}*/
	}

	return 0;
}