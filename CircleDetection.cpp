// CircleDetection.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <vector>
#include <iostream>
#include <time.h>
#include <string>


using namespace cv;
using namespace std;


vector<Vec3f> circles;


static void help()
{
	cout << "\nThis program demonstrates circle finding with the Hough transform.\n"
		"Usage:\n"
		"./houghcircles <image_name>, Default is pic1.png\n" << endl;
}


/**
求两个圆心之间的距离
**/
double circleDistance(Vec3i A, Vec3i B)
{
	double dis = sqrt((A[0] - B[0])*(A[0] - B[0]) + (A[1] - B[1])*(A[1] - B[1]));
	return dis;
}

/**
返回距离输入的圆最近的圆
**/
size_t minDistanceCircle(vector<Vec3f>circlesTemp, Vec3i circleInput, size_t circleInputIndex)
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


int _tmain(int argc, _TCHAR* argv[])
{
	//	const char* filename = argc >= 2 ? argv[1] : "U形转弯.png";
	//	const char* filename = argc >= 2 ? argv[1] : "十字路口.png";
	//	const char* filename = argc >= 2 ? argv[1] : "直角弯.png";
	//	const char* filename = argc >= 2 ? argv[1] : "直线.png";
	//	const char* filename = argc >= 2 ? argv[1] : "随机3.png";
	//const char* filename = argc >= 2 ? argv[1] : "test1.png";
	const char* filename = argc >= 2 ? argv[1] : "真实4.png";

	Mat img = imread(filename, 1);     //img是彩色的
	if (img.empty())
	{
		help();
		cout << "can not open " << filename << endl;
		return -1;
	}
	Mat Gimg;    //灰度备份
	cvtColor(img, Gimg, COLOR_BGR2GRAY);   //Gimg灰度的

	cout << " row: " << Gimg.rows << " col: " << Gimg.cols << endl;  //显示图像高和宽
	imshow("Input", img);


	vector<Mat> channels;
	Mat  BlueChannel, GreenChannel, RedChannel;
	split(img, channels);
	BlueChannel = channels.at(0);
	GreenChannel = channels.at(1);
	RedChannel = channels.at(2);
	for (int i = 0; i < Gimg.cols; i++)
	{
		for (int j = 0; j < Gimg.rows; j++)
		{
			double Th1 = (double)(RedChannel.at<uchar>(j, i) - GreenChannel.at<uchar>(j, i)) / ((double)RedChannel.at<uchar>(j, i)+0.1 );
			double Th2 = (double)(RedChannel.at<uchar>(j, i) - BlueChannel.at<uchar>(j, i)) / ((double)RedChannel.at<uchar>(j, i)+0.1 );
			if (Th1 > 0.6 && Th2 > 0.5)
			{
				RedChannel.at<uchar>(j, i) = 0;
				GreenChannel.at<uchar>(j, i) = 0;
				BlueChannel.at<uchar>(j, i) = 0;
			}
			else
			{
				RedChannel.at<uchar>(j, i) = 255;
				GreenChannel.at<uchar>(j, i) = 255;
				BlueChannel.at<uchar>(j, i) = 255;
			}

		}
	}

	merge(channels, img);
	cvtColor(img, img, COLOR_BGR2GRAY);
	
	Mat cimg;
	//	medianBlur(img, img, 5);
	GaussianBlur(img, img, Size(5, 5), 2, 2);
	cvtColor(img, cimg, COLOR_GRAY2BGR);   //cimg彩色的



	double AveDiameter = 0;
	double Rs = 0;


	//imshow("source gray", cimg);
	//waitKey();
	HoughCircles(img, circles, CV_HOUGH_GRADIENT, 1, 10,
		100, 18, 8, 20 // change the last two parameters
		// (min_radius & max_radius) to detect larger circles
		);

	//寻找左下角的圆,作为起始圆
	int minDistance = INT_MAX;
	int minXYIndex = 0;
	for (int i = 0; i < circles.size(); i++)
	{
		if (circles[i][0] * circles[i][0] + (750 - circles[i][1]) * (750 - circles[i][1]) < minDistance)
		{
			minDistance = circles[i][0] * circles[i][0] + (750 - circles[i][1]) * (750 - circles[i][1]);
			minXYIndex = i;
		}
	}

	//将起始圆放到circle的起始位置
	if (circles.size() <= 0)
	{
		cout << "no circle found" << endl;
		return 0;
	}
	switchCircles(0, minXYIndex);

	//依次找到后续的圆
	for (int i = 0; i < circles.size() - 1; i++)
	{
		minXYIndex = minDistanceCircle(circles, circles[i], i);
		switchCircles(i + 1, minXYIndex);
	}

	//画圆
	for (size_t i = 0; i < circles.size(); i++)
	{
		Vec3i c = circles[i];
		circle(cimg, Point(c[0], c[1]), c[2], Scalar(0, 0, 255), 3, CV_AA);
		circle(cimg, Point(c[0], c[1]), 1, Scalar(0, 255, 0), 3, CV_AA);
		char words[20];
		sprintf_s(words, "%d", i);
		putText(cimg, words, Point(c[0], c[1]), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(255, 0, 0));
		cout << "x：" << c[0] << " y: " << c[1] << " Radius:" << c[2] << endl;
	}

	//计算图中圆直径的均值以及扫描阈值Rs
	for (size_t i = 0; i < circles.size(); i++)
	{
		AveDiameter = AveDiameter + circles[i][2];
	}
	AveDiameter = 2 * AveDiameter / circles.size();
	Rs = 4 * AveDiameter;
	cout << "平均直径： " << AveDiameter << "  扫描半径： " << Rs << endl;

	//连线
	for (size_t i = 0; i < circles.size() - 1; i++)
	{
		Vec3i pointBegin = circles[i];
		Vec3i pointEnd = circles[i + 1];
		line(cimg, Point(pointBegin[0], pointBegin[1]), Point(pointEnd[0], pointEnd[1]), Scalar(255, 0, 0), 1, CV_AA);
	}

	//计算



	//	namedWindow("detected circles", CV_WINDOW_NORMAL);
	imshow("detected circles", cimg);
	//	imwrite("test1_result.png", cimg);
	waitKey();

	return 0;
}



