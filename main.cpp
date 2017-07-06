#include <iostream>
#include <iomanip>
#include <vector>
#include <time.h>
#include <cmath>
#include <string>
#include "DenseGraph.h"
#include "SparseGraph.h"
#include "ReadGraph.h"
#include "LazyPrimMST.h"
#include "CircleDetection.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

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

	//打开图
	//const char *filename = "../image/real3.jpg";

	 const char *filename = "../image/realTest4.png";
	Mat img = imread(filename, 1); //img是彩色的
	//if (img.empty())
	//{
	//	help();
	//	cout << "can not open " << filename << endl;
	//	return -1;
	//}

	// Build video capturer  
	// cap("../image/test1.mov"); // read from file  
	//VideoCapture cap(0); // read from camera  

	// Check if video is open  
	/*if (!cap.isOpened()){
		cout << "Cannot open video/camera!" << endl;
		return -1;
	}*/
	//Mat img;
	int frameNum = 0;
	//while (1)
	{
		// read frame and check  
		/*cap >> img;
		if (img.empty()){
			cout << "frame " << frameNum << " is empty!" << endl;
			break;
		}*/
		//imshow("Orignal", img);
		/*Size size(640, 360);
		resize(img, img, size);*/

		//检测圆，得到圆心距离和数量
		pair<vector<vector<double>>, vector<Vec3f>> results = circleDetection(img);
		vector<vector<double>> disMat = results.first;
		vector<Vec3f> circles = results.second;

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

			cout << "start: " << mst[i].v() << " end: " << mst[i].w() << " weight: " << mst[i].wt() << " theta : " << theta[2] << endl;
		}
		//cout << mst[i] << " " << mst[i].v() << endl;
		cout << "The MST weight is: " << lazyPrimMST.result() << endl;
		cout << endl;

		for (int i = 0; i < circles.size() - 1; i++)
		{
			Vec3i pointBegin = circles[mst[i].v()];
			Vec3i pointEnd = circles[mst[i].w()];
			line(img, Point(pointBegin[0], pointBegin[1]), Point(pointEnd[0], pointEnd[1]), Scalar(0, 0, 255), 1, CV_AA);
		}

		// update frame number  
		frameNum++;
		// show image  
		imshow("hello", img);
		int delay = 1; // ms  
		waitKey(0);
		/*if (27 == key || 'Q' == key || 'q' == key)
			break;*/
	}
	return 0;
}