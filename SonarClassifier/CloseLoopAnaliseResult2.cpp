#include "CloseLoopAnaliseResult2.h"
#include "CSVReader/CSVReader2.h"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/contrib/contrib.hpp>

#include <iostream>
using namespace std;
using namespace cv;


CloseLoopAnaliseResult2::CloseLoopAnaliseResult2(const char *destPath):
    workPath(destPath)
{

}

void CloseLoopAnaliseResult2::process(const char *csvGTName, const char *csvResultName)
{
    if(!loadGT((workPath+csvGTName).c_str()))
    {
        cout << "Problem to load " << csvGTName << endl;
        return;
    }
    loadResults((workPath+csvResultName).c_str());

    generateGTImage();
    generateResultImage();
}

void CloseLoopAnaliseResult2::processResultFromImage(const char *csvGTName, const char *greyImgResultName)
{
    if(!loadGT((workPath+csvGTName).c_str()))
    {
        cout << "Problem to load " << csvGTName << endl;
        return;
    }
    loadResultsFromTriangGreyImage((workPath+greyImgResultName).c_str());

    generateGTImage();
    generateResultImage();
}

bool CloseLoopAnaliseResult2::loadGT(const char *csvFileName)
{
    CSVReader2 csv;

    if(!csv.open(csvFileName,3,',',CSV_INT,CSV_INT,CSV_FLOAT))
    {
        return false;
    }

    CLR2Data d;
    d.rScore = 0.f;
    d.feturesCount = 0.f;
    while(csv.read(&d.uId,&d.vId,&d.gtScore))
    {
        data.push_back(d);
    }
    return true;
}

bool CloseLoopAnaliseResult2::loadResults(const char *csvFileName)
{
    CSVReader2 csv;

    if(!csv.open(csvFileName,1,',',CSV_FLOAT))
    {
        return false;
    }

    unsigned line=0;
    float score;
    while(csv.read(&score))
    {
        data[line].rScore = score;
        line++;
    }
    return true;
}

bool CloseLoopAnaliseResult2::loadResultsFromTriangGreyImage(const char *img8BitFileName)
{
    Mat result;
    result = imread(img8BitFileName, CV_LOAD_IMAGE_ANYCOLOR | CV_LOAD_IMAGE_ANYDEPTH);
    if(result.rows == 0)
    {
        cout << "Grey 16bits image " << img8BitFileName << " not found!" << endl;
        return false;
    }

    imshow("Result" , result);
    waitKey();

    for(unsigned i = 0 ; i < data.size(); i++)
    {
        CLR2Data &d = data[i];
        if(d.uId > result.rows)
        {
            cout << "Load ResultsFrom Image Error: It was not found ID " << d.uId << " in grey image!" << endl;
            continue;
        }
        if(d.vId > result.rows)
        {
            cout << "Load ResultsFrom Image Error: It was not found ID " << d.uId << " in grey image!" << endl;
            continue;
        }
        uchar p = result.at<uchar>(d.uId , d.vId);
        float score = (float)p/255.f;
        d.rScore = score;
    }

    return true;
}

bool CloseLoopAnaliseResult2::loadFeaturesCountFromTriangGreyImage(const char *img8BitFileName)
{
    Mat result;
    result = imread(workPath +img8BitFileName, CV_LOAD_IMAGE_ANYCOLOR | CV_LOAD_IMAGE_ANYDEPTH);
    if(result.rows == 0)
    {
        cout << "Grey 16bits image " << img8BitFileName << "not found!" << endl;
        return false;
    }

    for(unsigned i = 0 ; i < data.size(); i++)
    {
        CLR2Data &d = data[i];
        if(d.uId > result.rows)
        {
            cout << "Load ResultsFrom Image Error: It was not found ID " << d.uId << " in grey image!" << endl;
            continue;
        }
        if(d.vId > result.rows)
        {
            cout << "Load ResultsFrom Image Error: It was not found ID " << d.uId << " in grey image!" << endl;
            continue;
        }
        d.feturesCount = (float)result.at<uchar>(d.uId , d.vId)/255.f;
    }

    return true;
}

void CloseLoopAnaliseResult2::generateGTImage()
{
    unsigned nMathchs = data.size();
    unsigned imgSz = ceil(sqrt(nMathchs));

    Mat gtImage(imgSz,imgSz, CV_8UC1, Scalar(0));

    for(unsigned iMatch = 0; iMatch < nMathchs ; iMatch++)
    {
        unsigned i = iMatch/imgSz, j = iMatch%imgSz;
        float score = data[iMatch].gtScore;
        if(score>1.f) score = 1.f;
        if(score<0.f) score = 0.f;
        gtImage.at<uchar>(i,j) = score*255;
    }

    Mat colorImg;
    applyColorMap(gtImage,colorImg,COLORMAP_JET);

    imwrite("gtImage.png",gtImage);
    imwrite("gtImageColor.png",colorImg);

    namedWindow("GT Image",WINDOW_NORMAL);
    imshow("GT Image", colorImg);
    waitKey();
}

void CloseLoopAnaliseResult2::generateResultImage()
{
    unsigned nMathchs = data.size();
    unsigned imgSz = ceil(sqrt(nMathchs));

    Mat rImage(imgSz,imgSz, CV_8UC1, Scalar(0));

    for(unsigned iMatch = 0; iMatch < nMathchs ; iMatch++)
    {
        unsigned i = iMatch/imgSz, j = iMatch%imgSz;
        float score = data[iMatch].rScore;
        if(score>1.f) score = 1.f;
        if(score<0.f) score = 0.f;

        rImage.at<uchar>(i,j) = score*255;
    }

    Mat colorImg;
    applyColorMap(rImage,colorImg,COLORMAP_JET);

    imwrite("ResultImage.png",rImage);
    imwrite("ResultImageColor.png",colorImg);

    namedWindow("Result Image",WINDOW_NORMAL);
    imshow("Result Image", colorImg);
    waitKey();
}

