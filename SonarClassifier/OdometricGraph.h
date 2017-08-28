#ifndef ODOMETRICGRAPH_H
#define ODOMETRICGRAPH_H

#include <opencv2/core/core.hpp>
using namespace cv;

class OdometricGraph
{
public:
    OdometricGraph();


    void addEdge(unsigned u, unsigned v, Point2f t, float r);

};

#endif // ODOMETRICGRAPH_H
