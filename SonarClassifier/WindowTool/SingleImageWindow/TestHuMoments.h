#ifndef TESTHUMOMENTS_H
#define TESTHUMOMENTS_H

#include "PolyMaker.h"

class TestHuMoments : public PolyMaker
{
public:
    TestHuMoments();

    int findSimilarShape();

    bool showBestMatch;
    unsigned bestMatchId;

    // SingleImageFeature interface
public:
    void render(Mat &imgBgr);
    void keyPress(char c);
};

#endif // TESTHUMOMENTS_H
