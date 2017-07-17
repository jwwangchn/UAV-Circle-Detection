#include <iostream>
#include <iomanip>
#include <vector>
#include <time.h>
#include <string>
#include <cmath>
#include "DenseGraph.h"
#include "SparseGraph.h"
#include "ReadGraph.h"
#include "LazyPrimMST.h"
#include "CircleDetection.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace std;
using namespace cv;
int flagSituation = 0; //1--十字路口, 2--普通情况, 3--起点, 4--终点
void calculate(Mat img, double & intercept, double & slope);
int main()
{
	clock_t start, finish;
	double duration;
	int nCount=0;
	double slope = 0;
	double intercept = 0;
	VideoCapture cap("TestImage/video4.mp4");
	if (!cap.isOpened())
	{
		cout << "can't open the video" << endl;
		return -1;
	}
	Mat img;
	while (1)
	{
		start = clock();
		
		//cap >> img;
		//Mat img = frame;

		// const char *filename = "../image/DJI_0003.jpg";
		//const char *filename = "../image/crossroad.jpg";
		const char *filename = "../image/2000.png";
		Mat img = imread(filename, 1);
		if (img.empty())
		{
			cout << "empty image" << endl;
			return -1;
		}
		Size size(640, 360);
		resize(img, img, size);
		imshow("origin video", img);

      	calculate(img, intercept, slope);
		//计时器
		finish = clock();
		duration = CLOCKS_PER_SEC/(double)(finish - start)  ;

		//cout << "Image size: " << img.cols << " * " << img.rows << endl;
		cout << "Hz: " << duration << endl;

	}
	return 0;
}

void calculate(Mat img, double & intercept, double & slope)
{
	//圆检测
		pair<vector<vector<double>>, vector<Vec3f>> results = circleDetection(img);
		vector<vector<double>> disMat = results.first;
		vector<Vec3f> circles = results.second;
		if (circles.size() <= 1)
		{
			cout << "detected circles number: " << circles.size() << endl;
			cout << "can't detect circle" << endl;
			 return ;
		}

		//图节点数量
		int V = circles.size();

		//构造图
		SparseGraph<double> g = SparseGraph<double>(V, false);
		//ReadGraph<SparseGraph<double>, double> readGraph(g, filename);

		for (int i = 0; i < V; i++)
		{
			for (int j = 0; j < V; j++)
			{
				g.addEdge(i, j, disMat[i][j]);
			}
		}

		// Test Lazy Prim MST
		//求解最小生成树

		LazyPrimMST<SparseGraph<double>, double> lazyPrimMST(g);

		//MST结果
		vector<Edge<double>> mst = lazyPrimMST.mstEdges();

		// [1] 十字路口
		vector<vector<int>> crossroadMatrix(V);

		//找到每个节点相连的节点
		for (int i = 0; i < V - 1; i++)
		{
			crossroadMatrix[mst[i].v()].push_back(mst[i].w());  // mst is the edges vector 
			crossroadMatrix[mst[i].w()].push_back(mst[i].v());
		}
		//十字路口节点距离矩阵
		vector<vector<double>> distanceCrossroad(6, vector<double>(3));
		int indexMaxBelow = 0, indexMaxUp = 0;
		int CrossTpLine=img.cols, CrossBwLine=0;
		for (int i = 0; i < V; i++)
		{
			// cout << i << " " << crossroadMatrix[i].size() << endl;
			if (crossroadMatrix[i].size() == 4)
			{
				cout << "发现十字路口!!!! " << i << " 节点是十字路口" << endl;
				flagSituation = 1;
				int k = 0;
				double maxBelow = 0, maxUp = 0;

				for (int m = 0; m < 3; m++)
				{
					for (int n = m + 1; n < 4; n++)
					{
						distanceCrossroad[k][0] = crossroadMatrix[i][m];
						if(circles[crossroadMatrix[i][m]][1]<=CrossTpLine) CrossTpLine=circles[crossroadMatrix[i][m]][1];
						if(circles[crossroadMatrix[i][m]][1]>CrossBwLine) CrossBwLine=circles[crossroadMatrix[i][m]][1];
						distanceCrossroad[k][1] = crossroadMatrix[i][n];
						if(circles[crossroadMatrix[i][n]][1]<=CrossTpLine) CrossTpLine=circles[crossroadMatrix[i][n]][1];
						if(circles[crossroadMatrix[i][n]][1]>CrossBwLine) CrossBwLine=circles[crossroadMatrix[i][n]][1];
		
						distanceCrossroad[k][2] = circleDistance(circles[crossroadMatrix[i][m]], circles[crossroadMatrix[i][n]]);
						if (distanceCrossroad[k][2] > maxUp)
						{
							maxUp = distanceCrossroad[k][2];
							indexMaxUp = k;
						}
						else if (distanceCrossroad[k][2] > maxBelow)
						{
							maxBelow = distanceCrossroad[k][2];
							indexMaxBelow = k;
						}
						k++;
					}
				}
				//cout << distanceCrossroad[indexMaxUp][0] << " " <<distanceCrossroad[indexMaxUp][1] << " " << distanceCrossroad[indexMaxBelow][0] << " " << distanceCrossroad[indexMaxBelow][1] << endl;
			}
		}
		//十字路口进入
		if (flagSituation == 1&&(MAX_LINE+MIN_LINE)/2>CrossTpLine&&(MAX_LINE+MIN_LINE)/2<CrossBwLine)
		{
			double crossRoad_Line1_Slope = slopeAndIntercept(circles, distanceCrossroad[indexMaxUp][0], distanceCrossroad[indexMaxUp][1], MAX_LINE, MIN_LINE).first;
			double crossRoad_Line2_Slope = slopeAndIntercept(circles, distanceCrossroad[indexMaxBelow][0], distanceCrossroad[indexMaxBelow][1], MAX_LINE, MIN_LINE).first;
			int crossRoadPoint1 = 0, crossRoadPoint2 = 0;
			if (crossRoad_Line1_Slope > 75 && crossRoad_Line1_Slope < 105)
			{
				crossRoadPoint1 = distanceCrossroad[indexMaxUp][0];
				crossRoadPoint2 = distanceCrossroad[indexMaxUp][1];
			}
			else
			{
				crossRoadPoint1 = distanceCrossroad[indexMaxBelow][0];
				crossRoadPoint2 = distanceCrossroad[indexMaxBelow][1];
			}
			pair<double, double> slopeAndInterceptResult = slopeAndIntercept(circles, crossRoadPoint1, crossRoadPoint2, (circles[crossRoadPoint1][1] + circles[crossRoadPoint2][1]) / 2, (circles[crossRoadPoint1][1] + circles[crossRoadPoint2][1]) / 2);
			slope = slopeAndInterceptResult.first;
			intercept = slopeAndInterceptResult.second;
			cout << "十字路口: " << crossRoadPoint1 << " ------crossRoad------ " << crossRoadPoint2 << endl;
			cout << "intercept: " << intercept << endl;
			cout << "slope: " << slope << endl;
		}
		else  //control line is not in the cross road
		{
		//[2] 点--线--点
		vector<vector<double>> thetaMatrix;
		vector<double> theta(3);
		thetaMatrix.clear();
		for (int i = 0; i < mst.size(); i++)
		{
			// cout << "start: " << mst[i].v() << " end: " << mst[i].w() << " weight: " << mst[i].wt() << " theta : " << theta[2] << endl;
			if ((circles[mst[i].v()][1] < MIN_LINE && circles[mst[i].w()][1] > MAX_LINE) || (circles[mst[i].w()][1] < MIN_LINE && circles[mst[i].v()][1] > MAX_LINE))
			{
				theta[0] = mst[i].v();
				theta[1] = mst[i].w();
				pair<double, double> slopeAndInterceptResult = slopeAndIntercept(circles, mst[i].v(), mst[i].w(), MAX_LINE, MIN_LINE);
				theta[2] = slopeAndInterceptResult.first;
				slope = slopeAndInterceptResult.first;
				intercept = slopeAndInterceptResult.second;
				thetaMatrix.push_back(theta);

				cout << "普通: " << mst[i].v() << "---------------" << mst[i].w() << endl;
				cout << "intercept: " << intercept << endl;
				cout << "slope: " << slope << endl;
			}
		}
		//[3-4] 起点和终点
		double thetaSinglePoint = 0;
		double distanceSinglePoint = 0;
		pair<int, int> result_Max_Min = Y_Point_Max_Min(circles);
		//Up(起点)
		if ((MAX_LINE + MIN_LINE) / 2.0 > circles[result_Max_Min.first][1])
		{
			slope = slopeAndIntercept(circles, result_Max_Min.first, crossroadMatrix[result_Max_Min.first][0], MAX_LINE, MIN_LINE).first;
			intercept = 320 - circles[result_Max_Min.first][0];
			cout << "起点: " << result_Max_Min.first << "--------------- " << endl;
			//cout << "intercept: " << intercept << endl;
			//cout << "slope: " << slope << endl;
		}
		//Down(终点)
		if ((MAX_LINE + MIN_LINE) / 2.0 < circles[result_Max_Min.second][1])
		{
			slope = slopeAndIntercept(circles, result_Max_Min.second, crossroadMatrix[result_Max_Min.second][0], MAX_LINE, MIN_LINE).first;
			intercept = 320 - circles[result_Max_Min.second][0];
			cout << "终点: "
				 << "--------------- " << result_Max_Min.second << endl;
			//cout << "intercept: " << intercept << endl;
			//cout << "slope: " << slope << endl;
		}
		}
#ifdef SHOW_DETECTION_RESULT
		for (int i = 0; i < circles.size() - 1; i++)
		{
			Vec3i pointBegin = circles[mst[i].v()];
			Vec3i pointEnd = circles[mst[i].w()];
			line(img, Point(pointBegin[0], pointBegin[1]), Point(pointEnd[0], pointEnd[1]), Scalar(0, 0, 255), 1, CV_AA);
		}
		imshow("Paint Line Result", img);
		waitKey(1);
#endif
}
