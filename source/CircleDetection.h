#ifndef GA_H
#define GA_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <time.h>
#include <string>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#define SHOW_DETECTION_RESULT

#define MIN_RADIUS 2
#define MAX_RADIUS 20

#define R_G_R 0.5
#define R_B_R 0.45

#define MAX_CANNY 100
#define MIN_CANNY 15

using namespace std;
using namespace cv;

double circleDistance(Vec3i A, Vec3i B);
void paintCircles(Mat img);
pair<vector<vector<double>>, vector<Vec3f>> circleDetection(Mat img);

#endif