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

using namespace std;
using namespace cv;

double circleDistance(Vec3i A, Vec3i B);
void paintCircles(Mat img);
pair<vector<vector<double>>, vector<Vec3f>> circleDetection(Mat img);

#endif