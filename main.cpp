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

#define MIN_LINE 180
#define MAX_LINE 180

using namespace std;
using namespace cv;

static void help()
{
	cout << "\nThis program demonstrates circle finding with the Hough transform.\n"
			"Usage:\n"
			"./houghcircles <image_name>, Default is pic1.png\n"
		 << endl;
}
int main()
{
	clock_t start, finish;
	double duration;
	VideoCapture cap("../video/DJI_0002.MOV");
	if (!cap.isOpened())
	{
		cout << "can't open the video" << endl;
		return -1;
	}
	//while (1)
	{
		start = clock();
		Mat frame;
		//cap >> frame;
		Mat img = frame;

		// const char *filename = "../image/DJI_0003.jpg";
		// const char *filename = "../image/crossroad.jpg";
		const char *filename = "../image/down.jpg";		
		img = imread(filename, 1); //img�ǲ�ɫ��
		if (img.empty())
		{
			cout << "empty image" << endl;
			return -1;
		}
		Size size(640, 360);
		resize(img, img, size);
		imshow("origin video", img);
		//���Բ���õ�Բ�ľ��������
		pair<vector<vector<double>>, vector<Vec3f>> results = circleDetection(img);
		vector<vector<double>> disMat = results.first;
		vector<Vec3f> circles = results.second;
		if (circles.size() <= 1)
		{
			cout << "detected circles number: " << circles.size() << endl;
			cout << "can't detect circle" << endl;
			// continue;
		}

		//ͼ�ڵ�����
		int V = circles.size();

		//����ͼ
		SparseGraph<double> g = SparseGraph<double>(V, false);
		//ReadGraph<SparseGraph<double>, double> readGraph(g, filename);

		//����ͼ
		for (int i = 0; i < V; i++)
		{
			for (int j = 0; j < V; j++)
			{
				g.addEdge(i, j, disMat[i][j]);
			}
		}

		// Test Lazy Prim MST
		cout << "Test Lazy Prim MST:" << endl;
		//������С������

		LazyPrimMST<SparseGraph<double>, double> lazyPrimMST(g);

		//��ӡ���
		vector<Edge<double>> mst = lazyPrimMST.mstEdges();


		// CrossRoad
		vector<vector<int>> crossroadMatrix(V);

		for (int i = 0; i < V - 1; i++)
		{
			crossroadMatrix[mst[i].v()].push_back(mst[i].w());
			crossroadMatrix[mst[i].w()].push_back(mst[i].v());
		}

		vector<vector<double>> distanceCrossroad(6,vector<double>(3));
		int indexMaxBelow = 0, indexMaxUp = 0;
		for (int i = 0; i < V; i++)
		{
			cout << i << " " << crossroadMatrix[i].size() << endl;
			if (crossroadMatrix[i].size() == 4)
			{
				cout << "find crossroad!!!! " << i << " is the crossroad" << endl;
				int k = 0;
				double maxBelow = 0, maxUp = 0;
				
				for (int m = 0; m < 3; m++)
				{
					for (int n = m + 1; n < 4; n++)
					{
						distanceCrossroad[k][0] = crossroadMatrix[i][m];
						distanceCrossroad[k][1] = crossroadMatrix[i][n];
						distanceCrossroad[k][2] = circleDistance(circles[crossroadMatrix[i][m]], circles[crossroadMatrix[i][n]]);
						if(distanceCrossroad[k][2] > maxUp)
						{
							maxUp = distanceCrossroad[k][2];
							indexMaxUp = k;
						}
						else if(distanceCrossroad[k][2] > maxBelow)
						{
							maxBelow = distanceCrossroad[k][2];
							indexMaxBelow = k;
						}
						k++;
					}
				}
				cout << distanceCrossroad[indexMaxUp][0] << " " <<distanceCrossroad[indexMaxUp][1] << " " << distanceCrossroad[indexMaxBelow][0] << " " << distanceCrossroad[indexMaxBelow][1] << endl;
			}
		}
		double crossRoad_Line1_Slope = slope(circles, distanceCrossroad[indexMaxUp][0], distanceCrossroad[indexMaxUp][1]).first;
		double crossRoad_Line2_Slope = slope(circles, distanceCrossroad[indexMaxBelow][0], distanceCrossroad[indexMaxBelow][1]).first;
		int crossRoadPoint1 = 0, crossRoadPoint2 = 0;
		if(crossRoad_Line1_Slope > 75 && crossRoad_Line1_Slope < 105)
		{
			crossRoadPoint1 = distanceCrossroad[indexMaxUp][0];
			crossRoadPoint2 = distanceCrossroad[indexMaxUp][1];
		}
		else
		{
			crossRoadPoint1 = distanceCrossroad[indexMaxBelow][0];
			crossRoadPoint2 = distanceCrossroad[indexMaxBelow][1];
		}
		cout << crossRoadPoint1 << " ------crossRoad------ " << crossRoadPoint2 << endl;


		//point1 line point2
		vector<vector<double>> thetaMatrix;
		vector<double> theta(3);
		thetaMatrix.clear();
		for (int i = 0; i < mst.size(); i++)
		{
			cout << "start: " << mst[i].v() << " end: " << mst[i].w() << " weight: " << mst[i].wt() << " theta : " << theta[2] << endl;
			if ((circles[mst[i].v()][1] < MIN_LINE && circles[mst[i].w()][1] > MAX_LINE) || (circles[mst[i].w()][1] < MIN_LINE && circles[mst[i].v()][1] > MAX_LINE))
			{
				theta[0] = mst[i].v();
				theta[1] = mst[i].w();
				pair<double,double> slopeResult = slope(circles, mst[i].v(), mst[i].w());
				theta[2] = slopeResult.first;
				double k = slopeResult.second;
				thetaMatrix.push_back(theta);

				double intercept = ((double)circles[mst[i].v()][1] / (double)circles[mst[i].v()][0] - (double)circles[mst[i].w()][1] / (double)circles[mst[i].w()][0]) * (1 / ((double)circles[mst[i].v()][0]) - 1 / ((double)circles[mst[i].w()][0]));
				
				double distance = 320 - ((MAX_LINE + MIN_LINE) / 2.0 - (circles[mst[i].v()][1] - k * circles[mst[i].v()][0])) / k;
				cout << mst[i].v() << "---------------" << mst[i].w() << endl;
				cout << "distance: " << distance << endl;
				cout << "theta: " << theta[2] << endl;
			}
		}
		//point1 line point2
		double thetaSinglePoint = 0;
		double distanceSinglePoint = 0;
		pair<int,int> result_Max_Min = Y_Point_Max_Min(circles);
		cout << circles[result_Max_Min.first][1] << " &&& " << circles[result_Max_Min.second][1] << endl;
		//Up
		if((MAX_LINE + MIN_LINE) / 2.0 > circles[result_Max_Min.first][1])
		{
			
			thetaSinglePoint = slope(circles, result_Max_Min.first, crossroadMatrix[result_Max_Min.first][0]).first;
			distanceSinglePoint = 320 - circles[result_Max_Min.first][0];
		}
		//Down
		if((MAX_LINE + MIN_LINE) / 2.0 < circles[result_Max_Min.second][1])
		{
			// cout << result_Max_Min.first << " ---- " << crossroadMatrix[result_Max_Min.first][0] << endl;
			thetaSinglePoint = slope(circles, result_Max_Min.second, crossroadMatrix[result_Max_Min.second][0]).first;
			distanceSinglePoint = 320 - circles[result_Max_Min.second][0];
		}
		cout << "thetaSinglePoint: " << thetaSinglePoint <<  " distanceSinglePoint: "<<distanceSinglePoint<<endl;
		cout << "The MST weight is: " << lazyPrimMST.result() << endl;
		cout << endl;

		for (int i = 0; i < circles.size() - 1; i++)
		{
			Vec3i pointBegin = circles[mst[i].v()];
			Vec3i pointEnd = circles[mst[i].w()];
			line(img, Point(pointBegin[0], pointBegin[1]), Point(pointEnd[0], pointEnd[1]), Scalar(0, 0, 255), 1, CV_AA);
		}

		finish = clock();
		duration = (double)(finish - start) / CLOCKS_PER_SEC;

		cout << "Image size: " << img.cols << " * " << img.rows << endl;
		cout << "Used time: " << duration << endl;
		imshow("Paint Line Result", img);
		waitKey(0);
	}
	return 0;
}