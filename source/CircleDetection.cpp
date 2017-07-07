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

pair<double,double> slope(vector<Vec3f> circlesTemp, int A, int B)
{
	pair<double,double> slopeResult;
	double dy = (circlesTemp[A][1] - circlesTemp[B][1]);
	double dx = (circlesTemp[A][0] - circlesTemp[B][0]);
	slopeResult.first = atan2(dy, dx) * 180 / 3.14159;
	slopeResult.second = dy / dx;
	if (slopeResult.first < 0)
	{
		slopeResult.first = slopeResult.first + 180;
	}
	return slopeResult;
}

//画圆
void paintCircles(Mat img)
{
	Mat multiChannelImg;
	cvtColor(img, multiChannelImg, COLOR_GRAY2BGR);
	for (int i = 0; i < circles.size(); i++)
	{
		Vec3i singleCircle = circles[i];
		circle(multiChannelImg, Point(singleCircle[0], singleCircle[1]), singleCircle[2], Scalar(0, 0, 255), 3, CV_AA);
		circle(multiChannelImg, Point(singleCircle[0], singleCircle[1]), 1, Scalar(0, 255, 0), 3, CV_AA);
		char words[20];
		sprintf(words, "%d", i);
		putText(multiChannelImg, words, Point(singleCircle[0], singleCircle[1]), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(255, 0, 0));
		imshow("Detection Result", multiChannelImg);
		// cout << "x:" << c[0] << " y:" << c[1] << " Radius:" << c[2] << endl;
	}
}

pair<int,int> Y_Point_Max_Min(vector<Vec3f> circlesTemp)
{
	pair<int,int> result;
	int indexMax = 0, indexMin = 0;
	double max = circlesTemp[0][1], min = circlesTemp[0][1];
	for(int i = 0; i < circlesTemp.size(); i++)
	{
		if(max < circlesTemp[i][1])
		{
			indexMax = i;
		}
		if(min > circlesTemp[i][1])
		{
			indexMin = i;
		}
	}
	result.first = indexMax;
	result.second = indexMin;
	return result;
}

// // 寻找左下角的圆, 作为起始圆 int minDistance = INT_MAX;
// int minXYIndex = 0;
// for (int i = 0; i < circles.size(); i++)
// {
// 	if (circles[i][0] * circles[i][0] + (750 - circles[i][1]) * (750 - circles[i][1]) < minDistance)
// 	{
// 		minDistance = circles[i][0] * circles[i][0] + (750 - circles[i][1]) * (750 - circles[i][1]);
// 		minXYIndex = i;
// 	}
// }

pair<vector<vector<double>>, vector<Vec3f>> circleDetection(Mat img)
{
	
	//Devide multi channels
	vector<Mat> channels;
	Mat BlueChannel, GreenChannel, RedChannel;
	split(img, channels);
	BlueChannel = channels.at(0);
	GreenChannel = channels.at(1);
	RedChannel = channels.at(2);
	for (int i = 0; i < img.cols; i++)
	{
		for (int j = 0; j < img.rows; j++)
		{
			double Th1 = (double)(RedChannel.at<uchar>(j, i) - GreenChannel.at<uchar>(j, i)) / ((double)RedChannel.at<uchar>(j, i) + 0.1);
			double Th2 = (double)(RedChannel.at<uchar>(j, i) - BlueChannel.at<uchar>(j, i)) / ((double)RedChannel.at<uchar>(j, i) + 0.1);
			if (Th1 > R_G_R && Th2 > R_B_R)
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

	//	medianBlur(img, img, 5);
	GaussianBlur(img, img, Size(5, 5), 2, 2);
	double AveDiameter = 0;
	double Rs = 0;

	HoughCircles(img, circles, CV_HOUGH_GRADIENT, 1, 10, MAX_CANNY, MIN_CANNY, MIN_RADIUS, MAX_RADIUS);
	if (circles.size() == 0)
	{
		pair<vector<vector<double>>, vector<Vec3f>> results;
		vector<vector<double>> disMat;
		results.first = disMat;
		results.second = circles;
		return results;
	}

#ifdef SHOW_DETECTION_RESULT
	paintCircles(img);
#endif

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

	//imshow("detected circles", cimg);

	pair<vector<vector<double>>, vector<Vec3f>> results;
	results.first = disMat;
	results.second = circles;
	return results;
}
