#include "ResultData.h"

ResultData::ResultData()
{
}

bool ResultData::fromCSV(const char *csvFileName)
{
    CSVReader2 csv;

    // FrameID 1 , FrameID 2, Score
    if(!csv.open(csvFileName,3,',',CSV_INT,CSV_INT,CSV_FLOAT))
    {
        return false;
    }

    RData d;
    while(csv.read(&d.uId,&d.vId,&d.score))
    {
        data.push_back(d);
    }
    return true;
}

bool ResultData::fromCSV(const ResultData &baseIndex, const char *csvFileName)
{
    CSVReader2 csv;

    // Score  (Index based on preview results)
    if(!csv.open(csvFileName,1,',',CSV_FLOAT))
    {
        return false;
    }

    data = baseIndex.data;
    unsigned line=0;
    while(csv.read(&data[line].score))
    {
        line++;
    }
    return true;
}

bool ResultData::fromTriangularGreyImg(const char *img8BitFileName)
{
    Mat result;
    result = imread(img8BitFileName, CV_LOAD_IMAGE_ANYCOLOR | CV_LOAD_IMAGE_ANYDEPTH);
    if(result.rows == 0)
    {
        cout << "8-bits GreyScale Image " << img8BitFileName << " not found!" << endl;
        return false;
    }

//    imshow("Result Data" , result);
//    waitKey();

    data.resize((result.rows*(result.rows+1))/2);
    unsigned line =0;
    for(unsigned uId = 0; uId < result.rows ; uId++)
    {
        for(unsigned vId = 0; vId <= uId ; vId++)
        {
            RData &d = data[line];
            d.score = (float)result.at<uchar>(uId , vId)/255.f;
            d.uId = uId; d.vId = vId;
            line++;
        }
    }
    return true;
}

bool ResultData::fromTriangularGreyImg(const ResultData &baseIndex, const char *img8BitFileName)
{
    Mat result;
    result = imread(img8BitFileName, CV_LOAD_IMAGE_ANYCOLOR | CV_LOAD_IMAGE_ANYDEPTH);
    if(result.rows == 0)
    {
        cout << "8-bits GreyScale Image " << img8BitFileName << " not found!" << endl;
        return false;
    }

//    imshow("Result Data" , result);
//    waitKey();

    data = baseIndex.data;
    for(unsigned i = 0 ; i < data.size(); i++)
    {
        RData &d = data[i];
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
        d.score = (float)result.at<uchar>(d.uId , d.vId)/255.f;
    }
    return true;
}

void ResultData::toSquaredImgResult()
{
    unsigned nMathchs = data.size();
    unsigned imgSz = ceil(sqrt(nMathchs));

    Mat result(imgSz,imgSz, CV_8UC1, Scalar(0));

    for(unsigned iMatch = 0; iMatch < nMathchs ; iMatch++)
    {
        unsigned i = iMatch/imgSz, j = iMatch%imgSz;
        float score = data[iMatch].score;
        if(score>1.f) score = 1.f;
        if(score<0.f) score = 0.f;
        result.at<uchar>(i,j) = score*255u;
    }

    Mat colorImg;
    applyColorMap(result,colorImg,COLORMAP_JET);

    imwrite("gtImage.png",result);
    imwrite("gtImageColor.png",colorImg);

    namedWindow("GT Image",WINDOW_NORMAL);
    imshow("GT Image", colorImg);
    waitKey();
}

bool ResultData::toCSV(const char* csvFileName)
{
    FILE *f = fopen(csvFileName,"w");

    if(f == 0x0) return false;

    for(unsigned i = 0 ; i < data.size();i++)
    {
        fprintf(f, "%d,%d,%g\n",data[i].uId, data[i].vId, data[i].score);
    }
    return true;
}

void ResultData::toTriangularGreyImg(const char *img8BitFileName)
{
    unsigned idMax=0;
    for(unsigned i = 0 ; i < data.size(); i++)
    {
        if(data[i].uId > idMax)
            idMax = data[i].uId;
        if(data[i].vId > idMax)
            idMax = data[i].vId;
    }
    unsigned nFr = idMax+1;
    Mat result(nFr,nFr, CV_8UC1, Scalar(0));

    for(unsigned i = 0 ; i < data.size(); i++)
    {
        RData &d = data[i];
        result.at<uchar>(d.uId,d.vId) = d.score*255.f;
        result.at<uchar>(d.vId,d.uId) = d.score*255.f;
    }

    Mat colorImg;
    applyColorMap(result,colorImg,COLORMAP_JET);

    imwrite(string(img8BitFileName) + ".png", result);
    imwrite(string(img8BitFileName) + "_Color.png", colorImg);

    namedWindow(img8BitFileName,WINDOW_NORMAL);
    imshow(string(img8BitFileName) + "_Color", colorImg);
    waitKey();
}
