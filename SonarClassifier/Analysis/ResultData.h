#ifndef RESULTDATA_H
#define RESULTDATA_H

#include "CSVReader/CSVReader2.h"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/contrib/contrib.hpp>

#include <iostream>
using namespace std;
using namespace cv;


class RData
{
public:
    unsigned uId, vId;
    float score;
};

class ResultData
{    
    string workPath;
    /**< Path to save the analise results */

public:
    vector<RData> data;

    ResultData();

    bool fromCSV(const char *csvFileName);
    bool fromCSV(const ResultData &baseIndex, const char *csvFileName);

    bool fromTriangularGreyImg(const char *img8BitFileName);
    bool fromTriangularGreyImg(const ResultData &baseIndex, const char *img8BitFileName);

    void toSquaredImgResult();
    bool toCSV(const char *csvFileName);
    void toTriangularGreyImg(const char *img8BitFileName);
};


#include "Analysis/ResultData.h"

//int main(int argc, char* argv[])
//{
//    ResultData gt, rDeep, rSG;
//    gt.fromCSV("../../../../SonarGraphData/grayData/Pedro/GTValidation.csv");

//    rDeep.fromCSV(gt,"../../../../SonarGraphData/grayData/Pedro/DeepdevilABS_eval.csv");

//    rSG.fromTriangularGreyImg(gt,"../../../../SonarGraphData/grayData/Pedro/ResultsGTLoop/ResultImage_New_tri.png");

//    gt.toSquaredImgResult();
//    rDeep.toSquaredImgResult();
//    rSG.toSquaredImgResult();

//    gt.toTriangularGreyImg("GroundTruth");
//    rDeep.toTriangularGreyImg("DeepLearning");
//    rSG.toTriangularGreyImg("SonarGraph");

//    return 0;
//}


#endif // RESULTDATA_H
