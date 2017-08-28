#ifndef SINGLEIMAGEFEATURE_H
#define SINGLEIMAGEFEATURE_H

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "WindowTool/SingleImageWindow.h"

using namespace std;
using namespace cv;

class SingleImageFeature
{
public:
    SingleImageFeature();

    SingleImageWindow *_SIW;

    virtual void mouseDrag(int button, int dx, int dy);
    virtual void endMouseDrag(int button, int dx, int dy);
    virtual void mouseEvent(int event,int flags, int x, int y);

    virtual void mouseClick(int x, int y);
    virtual void mouseDrag(int x, int y);
    virtual void render(Mat &imgBgr);
    virtual void keyPress(char c);
    virtual void start();

};

#endif // SINGLEIMAGEFEATURE_H
