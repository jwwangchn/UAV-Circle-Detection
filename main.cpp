#include "stdafx.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

using namespace cv;
using namespace std;
vector<Vec3f> circles;
static void help()
{
	cout << "\nThis program demonstrates circle finding with the Hough transform.\n"
		"Usage:\n"
		"./houghcircles <image_name>, Default is pic1.png\n" << endl;
}

double dist(Point A, Point B)
{
	double dis = sqrt((A.x - B.x)*(A.x - B.x) + (A.y - B.y)*(A.y - B.y));
	return dis;
}

/**
	求两个圆之间的距离
**/
double circleDistance(Vec3f A, Vec3f B)
{
	double dis = sqrt((A[0] - B[0])*(A[0] - B[0]) + (A[1] - B[1])*(A[1] - B[1]));
	return dis;
}

/**
	返回距离输入的圆最近的圆
**/
size_t minDistanceCircle(vector<Vec3f>circlesTemp, Vec3f circleInput, size_t circleInputIndex)
{
	double minDistance = (double)INT_MAX;
	size_t minIndex = 0;
	for (size_t i = circleInputIndex; i < circlesTemp.size(); i++)
	{
		if (circleDistance(circleInput, circlesTemp[i]) < minDistance && circleInputIndex != i)
		{
			minDistance = circleDistance(circleInput, circlesTemp[i]);
			minIndex = i;
		}		
	}
	return minIndex;
}
/**
	交换两个圆
**/
void switchCircles(size_t indexA, size_t indexB)
{
	Vec3i simpleCircle;
	simpleCircle = circles[indexA];
	circles[indexA] = circles[indexB];
	circles[indexB] = simpleCircle;
}


int main()
{
	//const char* filename = argc >= 2 ? argv[1] : "../image/随机2.png";
	const char* filename = "../image/random1.png";

	Mat img = imread(filename, 0);     //img灰度的
	if (img.empty())
	{
		help();
		cout << "can not open " << filename << endl;
		return -1;
	}
	//imshow("Place", img);
	Mat cimg;
	medianBlur(img, img, 5);
	cvtColor(img, cimg, COLOR_GRAY2BGR);   //cimg彩色的

	HoughCircles(img, circles, CV_HOUGH_GRADIENT, 1, 50,
		100, 30, 1, 60 // change the last two parameters
		// (min_radius & max_radius) to detect larger circles
		);

	//寻找左上角的圆,作为起始圆
	int minDistance = INT_MAX;
	int minXYIndex = 0;
	for (int i = 0; i < circles.size(); i++)
	{
		if (circles[i][0] * circles[i][0] + circles[i][1] * circles[i][1] < minDistance)
		{
			minDistance = circles[i][0] * circles[i][0] + circles[i][1] * circles[i][1];
			minXYIndex = i;
		}
	}

	//将起始圆放到circle的起始位置
	switchCircles(0, minXYIndex);
	//依次找到后续的圆
	for (int i = 0; i < circles.size() - 1; i++)
	{
		minXYIndex = minDistanceCircle(circles, circles[i], i);
		switchCircles(i + 1, minXYIndex);
	}
	//画圆，编号
	for (size_t i = 0; i < circles.size(); i++)
	{
		Vec3f c = circles[i];	//圆心，半径
		circle(cimg, Point(c[0], c[1]), c[2], Scalar(0, 0, 255), 3, CV_AA);	//圆
		circle(cimg, Point(c[0], c[1]), 1, Scalar(0, 255, 0), 3, CV_AA);	//圆心
		char words[20];
		sprintf_s(words, "%d", i);
		putText(cimg, words, Point(c[0], c[1]), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(255, 0, 0));
		cout << "x：" << c[0] << " y: " << c[1] << " Radius:" << c[2] << endl;
	}

	//连线
	for (size_t i = 0; i < circles.size() - 1; i++)
	{
		Vec3f pointBegin = circles[i];
		Vec3f pointEnd = circles[i+1];
		line(cimg, Point(pointBegin[0], pointBegin[1]), Point(pointEnd[0], pointEnd[1]), Scalar(255, 0, 0), 1, CV_AA);
	}
	imshow("detected circles", cimg);
	waitKey(0);
	//exit(0);
	return 0;
}



////按照Y坐标的大小顺序排序
//int minIndex = 0;
//for (size_t i = 0; i < circles.size(); i++)
//{
//	Vec3i minYVec3i = circles[i];
//	minIndex = i;
//	for (size_t j = i + 1; j < circles.size(); j++)
//	{
//		Vec3i tempVec3i = circles[j];	//圆心，半径
//		if (tempVec3i[1] <= minYVec3i[1])
//		{
//			minYVec3i[1] = tempVec3i[1];
//			minIndex = j;
//		}
//	}
//	for (size_t k = 0; k < 3; k++)
//	{
//		int tempVec3i = circles[minIndex][k];
//		circles[minIndex][k] = circles[i][k];
//		circles[i][k] = tempVec3i;
//	}
//}