#pragma once

// wstep, obsluga bledu
int lab01();

// negatyw, zapisywanie okrelonej liczby klatek jako zdjêcia
int negativeImage();
int videoSaveFrames();

// detekcja ruchu, logo(znak wodny)
int motionDetection();
int motionDetectionCamera();
int addBackgroundLogo();

// detekcja ruchu
int motionDetectionAlpha();
int createVideoFromImages();

// detekcja krawedzi
int shapeDetection();

// detekcja kolorów
int colorDetection();

// detekcja kolorów multi
int colorDetectionMultiple();

// detekcja ruchu gauss, hought
int motionDetectionGauss();
int houghTranform();

int motionDetectionOpticalFlow();
