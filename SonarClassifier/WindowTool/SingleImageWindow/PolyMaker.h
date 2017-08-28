#ifndef POLYMAKER_H
#define POLYMAKER_H

#include "SingleImageFeature.h"

class PolyMaker : public SingleImageFeature
{
public:
    PolyMaker();

    vector< vector<Point> > polys;
    int currentPoly, currentPoint;

    // SingleImageFeature interface
public:
    virtual void mouseClick(int x, int y);
    virtual void render(Mat &imgBgr);
    virtual void keyPress(char c);
    virtual void start();
};

#endif // POLYMAKER_H
