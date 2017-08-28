#include "TestHuMoments.h"
#include <cstdio>
#include <iostream>

TestHuMoments::TestHuMoments():
    showBestMatch(false)
{

}

int TestHuMoments::findSimilarShape()
{

    // Compute moments
//    for(unsigned i = 0 ; i < polys.size();i++)
//    {

//    }

    // Find best shape
//    for(unsigned i = 0 ; i < polys.size();i++)
//    {

//    }

    double score, bestScore=999999.9;
    unsigned bestShape;

    // Direct find best shape
    for(unsigned i = 0 ; i < polys.size();i++)
    {
        if(i == currentPoly) continue;

        score = matchShapes(polys[i],polys[currentPoly],
                            CV_CONTOURS_MATCH_I2,0);

        cout << "Score between " << currentPoly << " and " << i
             << " = " << score << endl;

        if(score < bestScore)
        {
            bestScore = score;
            bestShape = i;
        }
    }
    cout << "Similar shape of " << currentPoly << " is " << bestShape << endl;
    // Return best shape
    return bestShape;
}

void TestHuMoments::render(Mat &imgBgr)
{
    PolyMaker::render(imgBgr);
    if(showBestMatch)
    {
        char str[100];
        sprintf(str,"Best match of %d is %u.", currentPoly, bestMatchId);

        putText(imgBgr, str,
                Point2f(0,25),
                FONT_HERSHEY_COMPLEX, 0.4,
                Scalar(0,0,255), 1, 8);
    }
}

void TestHuMoments::keyPress(char c)
{
    switch(c)
    {
    case 'm':
        bestMatchId = findSimilarShape();
        showBestMatch = true;
        _SIW->refreshScreen();
    break;
    case ',':
    case '.':
        showBestMatch = false;
    break;
    }

    PolyMaker::keyPress(c);
}

