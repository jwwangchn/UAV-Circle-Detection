#include "CircleDetection.h"

using namespace cv;
using namespace std;

vector<Vec3f> circles;
/**
求两个圆心之间的距离
**/
double circleDistance(Vec3i A, Vec3i B)
{
	double dis = sqrt((A[0] - B[0]) * (A[0] - B[0]) + (A[1] - B[1]) * (A[1] - B[1]));
	return dis;
}


pair<vector<vector<double>>, vector<Vec3f>> circleDetection(Mat img)
{
    Mat Gimg;							 //灰度备份
	cvtColor(img, Gimg, COLOR_BGR2GRAY); //Gimg灰度的

	cout << " row: " << Gimg.rows << " col: " << Gimg.cols << endl; //显示图像高和宽

	vector<Mat> channels;
	Mat BlueChannel, GreenChannel, RedChannel;
	split(img, channels);
	BlueChannel = channels.at(0);
	GreenChannel = channels.at(1);
	RedChannel = channels.at(2);
	for (int i = 0; i < Gimg.cols; i++)
	{
		for (int j = 0; j < Gimg.rows; j++)
		{
			double Th1 = (double)(RedChannel.at<uchar>(j, i) - GreenChannel.at<uchar>(j, i)) / ((double)RedChannel.at<uchar>(j, i) + 0.1);
			double Th2 = (double)(RedChannel.at<uchar>(j, i) - BlueChannel.at<uchar>(j, i)) / ((double)RedChannel.at<uchar>(j, i) + 0.1);
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
	cvtColor(img, cimg, COLOR_GRAY2BGR); //cimg彩色的

	double AveDiameter = 0;
	double Rs = 0;

	HoughCircles(img, circles, CV_HOUGH_GRADIENT, 1, 10,
				 100, 18, 8, 20 // change the last two parameters
				 // (min_radius & max_radius) to detect larger circles
				 );

	//寻找左下角的圆,作为起始圆
	//int minDistance = INT_MAX;
	//int minXYIndex = 0;
	//for (int i = 0; i < circles.size(); i++)
	//{
	//	if (circles[i][0] * circles[i][0] + (750 - circles[i][1]) * (750 - circles[i][1]) < minDistance)
	//	{
	//		minDistance = circles[i][0] * circles[i][0] + (750 - circles[i][1]) * (750 - circles[i][1]);
	//		minXYIndex = i;
	//	}
	//}

	////将起始圆放到circle的起始位置
	//if (circles.size() <= 0)
	//{
	//	cout << "no circle found" << endl;
	//	return 0;
	//}
	//switchCircles(0, minXYIndex);

	////依次找到后续的圆
	//for (int i = 0; i < circles.size() - 1; i++)
	//{
	//	minXYIndex = minDistanceCircle(circles, circles[i], i);
	//	switchCircles(i + 1, minXYIndex);
	//}

	//画圆
	for (size_t i = 0; i < circles.size(); i++)
	{
		Vec3i c = circles[i];
		circle(cimg, Point(c[0], c[1]), c[2], Scalar(0, 0, 255), 3, CV_AA);
		circle(cimg, Point(c[0], c[1]), 1, Scalar(0, 255, 0), 3, CV_AA);
		char words[20];
		sprintf(words, "%d", i);
		putText(cimg, words, Point(c[0], c[1]), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(255, 0, 0));
		cout << "x:" << c[0] << " y:" << c[1] << " Radius:" << c[2] << endl;
	}

	////计算图中圆直径的均值以及扫描阈值Rs
	//for (size_t i = 0; i < circles.size(); i++)
	//{
	//	AveDiameter = AveDiameter + circles[i][2];
	//}
	//AveDiameter = 2 * AveDiameter / circles.size();
	//Rs = 4 * AveDiameter;
	//cout << "平均直径： " << AveDiameter << "  扫描半径： " << Rs << endl;

	////连线
	//for (size_t i = 0; i < circles.size() - 1; i++)
	//{
	//	Vec3i pointBegin = circles[i];
	//	Vec3i pointEnd = circles[i + 1];
	//	line(cimg, Point(pointBegin[0], pointBegin[1]), Point(pointEnd[0], pointEnd[1]), Scalar(255, 0, 0), 1, CV_AA);
	//}

	//计算斜率及偏角

	//计算距离矩阵
	vector<vector<double>> disMat(circles.size());
	for (int i = 0; i < circles.size(); i++)
	{
		disMat[i].resize(circles.size());
	}

	for (int i = 0; i < circles.size(); i++)
	{
		for (int j = 0; j < circles.size(); j++)
		{
			if (i != j)
				disMat[i][j] = circleDistance(circles[i], circles[j]);
			else
				disMat[i][j] = INT_MAX;
		}
	}
	cout << " disMat[1][2]: " << disMat[1][2] << endl;

	//imshow("detected circles", cimg);

	pair<vector<vector<double>>, vector<Vec3f>> results;
	results.first = disMat;
	results.second = circles;
    return results;
}
