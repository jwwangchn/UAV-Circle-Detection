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

		const char *filename = "../image/DJI_0003.jpg";
		img = imread(filename, 1); //img是彩色的
		if (img.empty())
		{
			cout << "empty image" << endl;
			return -1;
		}
		Size size(640, 360);
		resize(img, img, size);
		imshow("origin video", img);
		//检测圆，得到圆心距离和数量
		pair<vector<vector<double>>, vector<Vec3f>> results = circleDetection(img);
		vector<vector<double>> disMat = results.first;
		vector<Vec3f> circles = results.second;
		if (circles.size() <= 1)
		{
			cout << "detected circles number: " << circles.size() << endl;
			cout << "can't detect circle" << endl;
			// continue;
		}

		//图节点数量
		int V = circles.size();

		//定义图
		SparseGraph<double> g = SparseGraph<double>(V, false);
		//ReadGraph<SparseGraph<double>, double> readGraph(g, filename);

		//构造图
		for (int i = 0; i < V; i++)
		{
			for (int j = 0; j < V; j++)
			{
				g.addEdge(i, j, disMat[i][j]);
			}
		}

		// Test Lazy Prim MST
		cout << "Test Lazy Prim MST:" << endl;
		//计算最小生成树

		LazyPrimMST<SparseGraph<double>, double> lazyPrimMST(g);

		//打印结果
		vector<Edge<double>> mst = lazyPrimMST.mstEdges();

		vector<vector<double>> thetaMatrix;
		vector<double> theta(3);
		thetaMatrix.clear();
		for (int i = 0; i < mst.size(); i++)
		{
			//cout << "start: " << mst[i].v() << " end: " << mst[i].w() << " weight: " << mst[i].wt() << " theta : " << theta[2] << endl;
			if ((circles[mst[i].v()][1] < MIN_LINE && circles[mst[i].w()][1] > MAX_LINE) || (circles[mst[i].w()][1] < MIN_LINE && circles[mst[i].v()][1] > MAX_LINE))
			{
				theta[0] = mst[i].v();
				theta[1] = mst[i].w();
				double dy = (circles[mst[i].v()][1] - circles[mst[i].w()][1]);
				double dx = (circles[mst[i].v()][0] - circles[mst[i].w()][0]);
				theta[2] = atan2(dy, dx) * 180 / 3.14159;
				if (theta[2] < 0)
				{
					theta[2] = theta[2] + 180;
				}
				thetaMatrix.push_back(theta);
				double intercept = ((double)circles[mst[i].v()][1] / (double)circles[mst[i].v()][0] - (double)circles[mst[i].w()][1] / (double)circles[mst[i].w()][0]) * (1 / ((double)circles[mst[i].v()][0]) - 1 / ((double)circles[mst[i].w()][0]));
				double distance = ((MAX_LINE + MIN_LINE) / 2.0 - intercept) / (dy / dx) + 240;
				cout << mst[i].v() << "---------------" << mst[i].w() << endl;
				cout << "Jieju: " << distance << endl;
				cout << "Line theta: " << theta[2] << endl;
			}
		}
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