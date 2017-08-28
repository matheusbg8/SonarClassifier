#ifndef WTRF_H
#define WTRF_H

#include <WindowTool/GaussianDescriptor/GausianDescriptorFeature.h>

#include <WindowTool/WindowFeature.h>

class WTRF : public WindowFeature, public GausianDescriptorFeature
{
public:
    WTRF();

    // GausianDescriptorFeature interface
public:
    void newGaussian(int gId, int frameId);
    void leftGaussianSelected(int gId, int frameId);
    void rightGaussianSeleced(int gId, int frameId);
    void cleanedGaussians(int frameId);

    // WindowFeature interface
public:
    void start();
    void selectedFrame(int frameId);
    void keyPress(char c);
    void mouseEvent(int event, int x, int y);
    void renderProcess(Mat &leftImg, int leftId,
                       Mat &rightImg, int rightId);
    void renderFrameTogether(Mat &screen,
                             const Scalar_<float> &el, unsigned leftFrameId,
                             const Scalar_<float> &er, unsigned rightFrameId);
    void processImages(Mat &leftImg, int leftId,
                       Mat &rightImg, int rightId);
};

#endif // WTRF_H
