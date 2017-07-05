#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#define D_DEGUB

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
	������Բ֮��ľ���
**/
double circleDistance(Vec3f A, Vec3f B)
{
	double dis = sqrt((A[0] - B[0])*(A[0] - B[0]) + (A[1] - B[1])*(A[1] - B[1]));
	return dis;
}

/**
	���ؾ��������Բ�����Բ
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
	��������Բ
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
	//const char* filename = argc >= 2 ? argv[1] : "../image/���2.png";
	const char* filename = "../image/random1.png";

	Mat img = imread(filename, 0);     //img�Ҷȵ�
	if (img.empty())
	{
		help();
		cout << "can not open " << filename << endl;
		return -1;
	}
	//imshow("Place", img);
	Mat cimg;
	medianBlur(img, img, 5);
	cvtColor(img, cimg, COLOR_GRAY2BGR);   //cimg��ɫ��

	HoughCircles(img, circles, CV_HOUGH_GRADIENT, 1, 50,
		100, 30, 1, 60 // change the last two parameters
		// (min_radius & max_radius) to detect larger circles
		);

	//Ѱ�����Ͻǵ�Բ,��Ϊ��ʼԲ
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

	//����ʼԲ�ŵ�circle����ʼλ��
	switchCircles(0, minXYIndex);
	//�����ҵ�������Բ
	for (int i = 0; i < circles.size() - 1; i++)
	{
		minXYIndex = minDistanceCircle(circles, circles[i], i);
		switchCircles(i + 1, minXYIndex);
	}
	//��Բ�����
	for (size_t i = 0; i < circles.size(); i++)
	{
		Vec3f c = circles[i];	//Բ�ģ��뾶
		circle(cimg, Point(c[0], c[1]), c[2], Scalar(0, 0, 255), 3, CV_AA);	//Բ
		circle(cimg, Point(c[0], c[1]), 1, Scalar(0, 255, 0), 3, CV_AA);	//Բ��
		char words[20];
		sprintf(words, "%d", i);
		putText(cimg, words, Point(c[0], c[1]), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(255, 0, 0));
		cout << "x��" << c[0] << " y: " << c[1] << " Radius:" << c[2] << endl;
	}

	//����
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



////����Y����Ĵ�С˳������
//int minIndex = 0;
//for (size_t i = 0; i < circles.size(); i++)
//{
//	Vec3i minYVec3i = circles[i];
//	minIndex = i;
//	for (size_t j = i + 1; j < circles.size(); j++)
//	{
//		Vec3i tempVec3i = circles[j];	//Բ�ģ��뾶
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









// // CircleDetection.cpp : ??????????��????????
// //

// #include "stdafx.h"
// #include "opencv2/highgui/highgui.hpp"
// #include "opencv2/imgproc/imgproc.hpp"
// #include <vector>
// #include <iostream>
// #include <time.h>
// #include <string>


// using namespace cv;
// using namespace std;


// vector<Vec3f> circles;


// static void help()
// {
// 	cout << "\nThis program demonstrates circle finding with the Hough transform.\n"
// 		"Usage:\n"
// 		"./houghcircles <image_name>, Default is pic1.png\n" << endl;
// }


// /**
// ????????????????
// **/
// double circleDistance(Vec3i A, Vec3i B)
// {
// 	double dis = sqrt((A[0] - B[0])*(A[0] - B[0]) + (A[1] - B[1])*(A[1] - B[1]));
// 	return dis;
// }

// ///**
// //???????????????????
// //**/
// //size_t minDistanceCircle(vector<Vec3f>circlesTemp, Vec3i circleInput, size_t circleInputIndex)
// //{
// //	double minDistance = (double)INT_MAX;
// //	size_t minIndex = 0;
// //	for (size_t i = circleInputIndex; i < circlesTemp.size(); i++)
// //	{
// //		if (circleDistance(circleInput, circlesTemp[i]) < minDistance && circleInputIndex != i)
// //		{
// //			minDistance = circleDistance(circleInput, circlesTemp[i]);
// //			minIndex = i;
// //		}
// //	}
// //	return minIndex;
// //}
// //
// ///**
// //?????????
// //**/
// //void switchCircles(size_t indexA, size_t indexB)
// //{
// //	Vec3i simpleCircle;
// //	simpleCircle = circles[indexA];
// //	circles[indexA] = circles[indexB];
// //	circles[indexB] = simpleCircle;
// //}


// int _tmain(int argc, _TCHAR* argv[])
// {
// 	//	const char* filename = argc >= 2 ? argv[1] : "U?????.png";
// 	//	const char* filename = argc >= 2 ? argv[1] : "???��??.png";
// 	//	const char* filename = argc >= 2 ? argv[1] : "?????.png";
// 	//	const char* filename = argc >= 2 ? argv[1] : "???.png";
// 	//	const char* filename = argc >= 2 ? argv[1] : "???3.png";
// 	//  const char* filename = argc >= 2 ? argv[1] : "test1.png";
// 	const char* filename = argc >= 2 ? argv[1] : "???4.png";

// 	Mat img = imread(filename, 1);     //img??????
// 	if (img.empty())
// 	{
// 		help();
// 		cout << "can not open " << filename << endl;
// 		return -1;
// 	}
// 	Mat Gimg;    //??????
// 	cvtColor(img, Gimg, COLOR_BGR2GRAY);   //Gimg????

// 	cout << " row: " << Gimg.rows << " col: " << Gimg.cols << endl;  //?????????
// 	imshow("Input", img);


// 	vector<Mat> channels;
// 	Mat  BlueChannel, GreenChannel, RedChannel;
// 	split(img, channels);
// 	BlueChannel = channels.at(0);
// 	GreenChannel = channels.at(1);
// 	RedChannel = channels.at(2);
// 	for (int i = 0; i < Gimg.cols; i++)
// 	{
// 		for (int j = 0; j < Gimg.rows; j++)
// 		{
// 			double Th1 = (double)(RedChannel.at<uchar>(j, i) - GreenChannel.at<uchar>(j, i)) / ((double)RedChannel.at<uchar>(j, i)+0.1 );
// 			double Th2 = (double)(RedChannel.at<uchar>(j, i) - BlueChannel.at<uchar>(j, i)) / ((double)RedChannel.at<uchar>(j, i)+0.1 );
// 			if (Th1 > 0.6 && Th2 > 0.5)
// 			{
// 				RedChannel.at<uchar>(j, i) = 0;
// 				GreenChannel.at<uchar>(j, i) = 0;
// 				BlueChannel.at<uchar>(j, i) = 0;
// 			}
// 			else
// 			{
// 				RedChannel.at<uchar>(j, i) = 255;
// 				GreenChannel.at<uchar>(j, i) = 255;
// 				BlueChannel.at<uchar>(j, i) = 255;
// 			}

// 		}
// 	}

// 	merge(channels, img);
// 	cvtColor(img, img, COLOR_BGR2GRAY);
	
// 	Mat cimg;
// 	//	medianBlur(img, img, 5);
// 	GaussianBlur(img, img, Size(5, 5), 2, 2);
// 	cvtColor(img, cimg, COLOR_GRAY2BGR);   //cimg?????



// 	double AveDiameter = 0;
// 	double Rs = 0;


// 	//imshow("source gray", cimg);
// 	//waitKey();
// 	HoughCircles(img, circles, CV_HOUGH_GRADIENT, 1, 10,
// 		100, 18, 8, 20 // change the last two parameters
// 		// (min_radius & max_radius) to detect larger circles
// 		);

// 	//??????????,???????
// 	//int minDistance = INT_MAX;
// 	//int minXYIndex = 0;
// 	//for (int i = 0; i < circles.size(); i++)
// 	//{
// 	//	if (circles[i][0] * circles[i][0] + (750 - circles[i][1]) * (750 - circles[i][1]) < minDistance)
// 	//	{
// 	//		minDistance = circles[i][0] * circles[i][0] + (750 - circles[i][1]) * (750 - circles[i][1]);
// 	//		minXYIndex = i;
// 	//	}
// 	//}

// 	////?????????circle?????��??
// 	//if (circles.size() <= 0)
// 	//{
// 	//	cout << "no circle found" << endl;
// 	//	return 0;
// 	//}
// 	//switchCircles(0, minXYIndex);

// 	////??????????????
// 	//for (int i = 0; i < circles.size() - 1; i++)
// 	//{
// 	//	minXYIndex = minDistanceCircle(circles, circles[i], i);
// 	//	switchCircles(i + 1, minXYIndex);
// 	//}

// 	//???
// 	for (size_t i = 0; i < circles.size(); i++)
// 	{
// 		Vec3i c = circles[i];
// 		circle(cimg, Point(c[0], c[1]), c[2], Scalar(0, 0, 255), 3, CV_AA);
// 		circle(cimg, Point(c[0], c[1]), 1, Scalar(0, 255, 0), 3, CV_AA);
// 		char words[20];
// 		sprintf_s(words, "%d", i);
// 		putText(cimg, words, Point(c[0], c[1]), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(255, 0, 0));
// 		cout << "x??" << c[0] << " y: " << c[1] << " Radius:" << c[2] << endl;
// 	}

// 	////????????????????????????Rs
// 	//for (size_t i = 0; i < circles.size(); i++)
// 	//{
// 	//	AveDiameter = AveDiameter + circles[i][2];
// 	//}
// 	//AveDiameter = 2 * AveDiameter / circles.size();
// 	//Rs = 4 * AveDiameter;
// 	//cout << "???????? " << AveDiameter << "  ?????? " << Rs << endl;

// 	////????
// 	//for (size_t i = 0; i < circles.size() - 1; i++)
// 	//{
// 	//	Vec3i pointBegin = circles[i];
// 	//	Vec3i pointEnd = circles[i + 1];
// 	//	line(cimg, Point(pointBegin[0], pointBegin[1]), Point(pointEnd[0], pointEnd[1]), Scalar(255, 0, 0), 1, CV_AA);
// 	//}

// 	//????��??????

// 	//??????????
// 	Vector<Vector<double>> disMat(circles.size());
// 	for (int i = 0; i<circles.size(); i++)
// 	{
// 		disMat[i].resize(circles.size());
// 	}

// 	for (int i = 0; i < circles.size(); i++)
// 	{
// 		for (int j = 0; j < circles.size(); j++)
// 		{
// 			if (i != j)
// 				disMat[i][j] = circleDistance(circles[i], circles[j]);
// 			else
// 				disMat[i][j] = INT_MAX;
			
// 		}
// 	}
// 	cout << " disMat[1][2]: " << disMat[1][2] << endl;

// 	//	namedWindow("detected circles", CV_WINDOW_NORMAL);
// 	  imshow("detected circles", cimg);
// 	//	imwrite("test1_result.png", cimg);
// 	waitKey();
// 	return 0;
// }



