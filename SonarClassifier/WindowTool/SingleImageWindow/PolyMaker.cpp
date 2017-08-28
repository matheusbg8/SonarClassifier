#include "PolyMaker.h"

#include <cstdio>

PolyMaker::PolyMaker():
    currentPoint(0),
    currentPoly(0)
{

}

void PolyMaker::mouseClick(int x, int y)
{
    if(currentPoly >= polys.size())
    {
        polys.push_back(vector<Point>());
        currentPoly = polys.size()-1;
        currentPoint = polys[currentPoly].size();
        if(currentPoint<0) currentPoint=0;
    }
    polys[currentPoly].insert(polys[currentPoly].begin() + currentPoint,Point(x,y));
    currentPoint++;
}

void PolyMaker::render(Mat &imgBgr)
{
    _SIW->resetScren(Scalar(255,255,255));

    char str[100];
    sprintf(str,"Poly  %d", currentPoly);

    putText(imgBgr, str,
            Point2f(0,10),
            FONT_HERSHEY_COMPLEX, 0.4,
            Scalar(0,0,0), 1, 8);

    if(polys.size() == 0) return;

    const vector<Point> &poly = polys[currentPoly];

    Point ps[poly.size()];
    for(unsigned i = 0 ; i < poly.size();i++)
        ps[i] = poly[i];

    int npts[] = {poly.size()};
    const Point* ppt[1] = { ps };

    polylines(imgBgr, ppt, npts, 1,true,Scalar(0,0,255),1);

}

void PolyMaker::keyPress(char c)
{
    switch(c)
    {
    case '.': // next
        if(currentPoly < polys.size()-1)
        {
            currentPoly++;
            currentPoint=polys[currentPoly].size()-1;
            if(currentPoint<0) currentPoint =0;
            _SIW->refreshScreen();
        }
    break;
    case ',': // preview
        if(currentPoly > 0)
        {
            currentPoly--;
            currentPoint=polys[currentPoly].size()-1;
            if(currentPoint<0) currentPoint =0;
            _SIW->refreshScreen();
        }
    break;
    case 10: // Create new poly
        currentPoly = polys.size();
        currentPoint = 0;
        polys.push_back(vector<Point>());
        _SIW->refreshScreen();
    break;
    }
}

void PolyMaker::start()
{

}

