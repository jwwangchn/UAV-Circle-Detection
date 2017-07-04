#include <iostream>
#include <iomanip>
#include "DenseGraph.h"
#include "SparseGraph.h"
#include "ReadGraph.h"
#include "LazyPrimMST.h"

using namespace std;

// 测试最小生成树算法
int main()
{
    string filename = "testG1.txt";
    int V = 8;

    SparseGraph<double> g = SparseGraph<double>(V, false);
    //ReadGraph<SparseGraph<double>, double> readGraph(g, filename);
    g.addEdge(0, 1, 2);
    g.addEdge(1, 2, 2);
    g.addEdge(3, 4, 2);
    g.addEdge(4, 1, 2); 

    g.addEdge(0, 3, 1);
    g.addEdge(1, 3, 1);
    g.addEdge(2, 3, 1);
    g.addEdge(4, 3, 1);
    

    // Test Lazy Prim MST
    cout << "Test Lazy Prim MST:" << endl;
    LazyPrimMST<SparseGraph<double>, double> lazyPrimMST(g);
    vector<Edge<double>> mst = lazyPrimMST.mstEdges();
    for (int i = 0; i < mst.size(); i++)
        cout << mst[i] << endl;
    cout << "The MST weight is: " << lazyPrimMST.result() << endl;
    cout << endl;
    return 0;
}