#include "ResultAnalysis.h"
#include "CSVReader/CSVReader2.h"


bool ResultAnalysis::compare(const ResultData &gt, const ResultData &r,
                              StatisticResults &sr, vector<RAData> &ra)
{
    const vector<RData> &gd = gt.data,
                        &rd = r.data;

    sr.tn = sr.tp = sr.fp = sr.fp = 0u;
    sr.total =0;

    ra.resize(gd.size());

    for(unsigned i = 0 ; i < gd.size(); i++)
    {
        const RData &gdi = gd[i],
                    &rdi = rd[i];

        if(gdi.uId != rdi.uId || gdi.vId != rdi.vId)
        {
            cout << "ResultAnalysis::Process() - Probem with index! ("
                 << gdi.uId << ',' << gdi.vId << ") != ("
                 << rdi.uId << ',' << rdi.vId << ')' << endl;
            continue;
        }

        ra[i].uId = gd[i].uId;
        ra[i].vId = gd[i].vId;

        // Binarization
        bool bg = gdi.score > sr.gTh,
             br = rdi.score > sr.rTh;

        if(br == true) // Positive?
        {
            if(bg == br) // true?
            {
                ra[i].r = RA_TP;
                sr.tp++;
            }else // false
            {
                ra[i].r = RA_FP;
                sr.fp++;
            }
        }else if(br == false) // Negative?
        {
            if(bg == br) // true?
            {
                ra[i].r = RA_TN;
                sr.tn++;
            }else // false
            {
                ra[i].r = RA_FN;
                sr.fn++;
            }
        }
    }
}

ResultAnalysis::ResultAnalysis()
{
}

/**
 * @brief ResultAnalysis::process - Internal method that do the
 * comparison between results and compute the statistics results.
 * @param gt - Espected results (ground truth)
 * @param r -
 * @param sr
 * @return - Always true.
 */
bool ResultAnalysis::process(const ResultData &gt, const ResultData &r, StatisticResults &sr)
{
    const vector<RData> &gd = gt.data,
                        &rd = r.data;

    sr.tp = sr.tn = sr.fp = sr.fn = 0u;

    for(unsigned i = 0 ; i < gd.size(); i++)
    {
        const RData &gdi = gd[i],
                    &rdi = rd[i];

        if(gdi.uId != rdi.uId || gdi.vId != rdi.vId)
        {
            cout << "ResultAnalysis::Process() - Probem with index! ("
                 << gdi.uId << ',' << gdi.vId << ") != ("
                 << rdi.uId << ',' << rdi.vId << ')' << endl;
            continue;
        }

        // Binarization
        bool bg = gdi.score > sr.gTh,
             br = rdi.score > sr.rTh;

        if(br == true) // Positive?
        {
            if(bg == br) // true?
            {
                sr.tp++;
            }else // false
            {
                sr.fp++;
            }
        }else if(br == false) // Negative?
        {
            if(bg == br) // true?
            {
                sr.tn++;
            }else // false
            {
                sr.fn++;
            }
        }
    }

    sr.computeStatistics(sr.tp,sr.tn,sr.fp,sr.fn);
    return true;
}

/**
 * @brief ResultAnalysis::fromResults - Compare and compute
 * the statistics results between soluctions for the binary problem,
 * where the possible values are false positive, false negative,
 * true positive and false negative.
 * @param gt - Ground Truth results, the espected results.
 * @param r - The achieved results by some approach.
 * @param start - Begin value of the threshold.
 * @param end - End value of the threshold.
 * @param step - Step, iteracted add to star value until end value.
 *
 */
void ResultAnalysis::fromResults(const ResultData &gt, const ResultData &r,
                                 float gtThStart, float gtThEnd, float gtStep,
                                 float rThStart, float rThEnd, float rStep)
{
    if(gt.data.size() == 0 || r.data.size() == 0)
    {
        cout << "ResultAnalysis::fromResults() - Error, empty results!" << endl;
        return;
    }

    int gtParamChanges = (gtThEnd-gtThStart)/gtStep+1,
        rParamChanges = (rThEnd-rThStart)/rStep+1,
        size = (gtParamChanges)*(rParamChanges);

    statisticResults.resize((int) size);
    unsigned line=0;
    float gtTh, rTh;

    for(int i = 0; i < gtParamChanges; i++)
    {
        gtTh = gtThStart + i*gtStep; // This equation solve some precision issues
        for(int j = 0; j < rParamChanges; j++)
        {
            rTh = rThStart + j*rStep;
            StatisticResults &sr = statisticResults[line];
            sr.gTh = gtTh;
            sr.rTh = rTh;
            process(gt,r,sr);
            line++;
        }
    }
}

bool ResultAnalysis::fromCSV(const char *csvFileName)
{
    CSVReader2 csv;

    csv.ignoreLine(); // Traditional commentary line

    if(!csv.open(csvFileName,18,',',
             CSV_FLOAT,CSV_FLOAT,
             CSV_INT,CSV_INT,CSV_INT,CSV_INT,CSV_LLU,
             CSV_FLOAT,CSV_FLOAT,CSV_FLOAT,CSV_FLOAT,
             CSV_FLOAT,CSV_FLOAT,CSV_FLOAT,CSV_FLOAT,CSV_FLOAT,CSV_FLOAT,CSV_FLOAT))
    {
        return false;
    }

    StatisticResults sr;
    while(csv.read(&sr.gTh,&sr.rTh,
                   &sr.tp,&sr.tn,&sr.tn,&sr.fp,&sr.fn,&sr.total,
                   &sr.ftp,&sr.ftn,&sr.ffp,&sr.ffn,
                   &sr.acy,&sr.sen,&sr.spe,&sr.effc,&sr.ppv,&sr.npv,&sr.mCoef))
    {
        statisticResults.push_back(sr);
    }
    return true;
}

StatisticResults ResultAnalysis::bestMatewCoef()
{
    float bestCoef = 0.f;
    unsigned bestR =0;
    for(unsigned i = 0; i < statisticResults.size(); i++)
    {
        StatisticResults &sr = statisticResults[i];

        if(fabs(sr.gTh - 0.1) < 0.001 && fabs(sr.mCoef) > fabs(bestCoef))
        {
            bestCoef = fabs(sr.mCoef);
            bestR = i;
        }
    }
    return statisticResults[bestR];
}

StatisticResults ResultAnalysis::bestMachado()
{
    float bestCoef = 0.f;
    unsigned bestR =0;
    for(unsigned i = 0; i < statisticResults.size(); i++)
    {
        StatisticResults &sr = statisticResults[i];
        if(fabs(sr.gTh - 0.2) < 0.001 &
           sr.ppv > bestCoef)
        {
            bestCoef = sr.ppv;
            bestR = i;
        }
    }
    return statisticResults[bestR];
}

StatisticResults ResultAnalysis::bestPositivePreditionValue()
{
    float bestPPV = 0.f;
    unsigned bestR =0;
    for(unsigned i = 0; i < statisticResults.size(); i++)
    {
        StatisticResults &sr = statisticResults[i];
        if(sr.ppv > bestPPV)
        {
            bestPPV = sr.ppv;
            bestR = i;
        }
    }
    return statisticResults[bestR];
}

StatisticResults ResultAnalysis::bestPPVandTP()
{
    float bestPPV = 0.f,
          bestTP = 0.f;
    unsigned bestR =0;
    for(unsigned i = 0; i < statisticResults.size(); i++)
    {
        StatisticResults &sr = statisticResults[i];

        if(sr.ppv > bestPPV || (sr.ppv == bestPPV && sr.tp > bestTP))
        {
            bestPPV = sr.ppv;
            bestTP = sr.tp;
            bestR = i;
        }
    }
    return statisticResults[bestR];
}

bool ResultAnalysis::toCSV(const char *csvFileName)
{
    FILE *f = fopen(csvFileName, "w");
    if(f == 0x0)
    {
        cout << "ResultAnalysis::toCSV- It can't' write csv file " << csvFileName << endl;
        return false;
    }

    fprintf(f,"#GtThreshold,ResultThreshold,"   // Thresholds
              "TruePositiveCount,TrueNegativeCount,FalsePositiveCount," // Counters
              "FalseNegativeCount,TotalCount,"  // Counters
              "TPPercent,TNPercent,FPPercent,FNPercent," // Percentages
              "Acy,Sen,Spe,Effc,PPV,NPV,MCoef"); // Statistics Results

    for(unsigned i = 0 ; i < statisticResults.size();i++)
    {
        StatisticResults &st = statisticResults[i];
        fprintf(f,"%g,%g,"  // Thresholds
                  "%d,%d,%d,%d,%llu," // Counters
                  "%g,%g,%g,%g,"  // Percentages
                  "%g,%g,%g,%g,%g,%g,%g\n", // Statistics Results
                  st.gTh, st.rTh,
                  st.tp, st.tn,st.fp,st.fn,st.total,
                  st.ftp,st.ftn,st.ffp,st.ffn,
                  st.acy,st.sen,st.spe,st.effc,st.ppv,st.npv,st.mCoef
                );
    }
}

void ResultAnalysis::toStatisticImgs(const string &imgPrefixName, unsigned cellSize)
{
    unsigned size = ceil(sqrt(statisticResults.size()));

    Mat mTP(size,size,CV_8UC1),
        mTN(size,size,CV_8UC1),
        mFP(size,size,CV_8UC1),
        mFN(size,size,CV_8UC1);

    Mat mAcy(size,size,CV_8UC1),
        mSen(size,size,CV_8UC1),
        mSpe(size,size,CV_8UC1),
        mEffc(size,size,CV_8UC1),
        mPpv(size,size,CV_8UC1),
        mNpv(size,size,CV_8UC1),
        mMCoef(size,size,CV_8UC1);

    unsigned iMatch=0;
    for(unsigned i = 0; i < statisticResults.size() ; i++)
    {
        for(unsigned j = 0; j < statisticResults.size() ; j++)
        {
            StatisticResults &sr = statisticResults[iMatch];
            mTP.at<uchar>(i,j) = sr.ftp*255.f;
            mTN.at<uchar>(i,j) = sr.ftn*255.f;
            mFP.at<uchar>(i,j) = sr.ffp*255.f;
            mFN.at<uchar>(i,j) = sr.ffn*255.f;

            mAcy.at<uchar>(i,j) = sr.acy*255.f;
            mSen.at<uchar>(i,j) = sr.sen*255.f;
            mSpe.at<uchar>(i,j) = sr.spe*255.f;
            mEffc.at<uchar>(i,j) = sr.effc*255.f;
            mPpv.at<uchar>(i,j) = sr.ppv*255.f;
            mNpv.at<uchar>(i,j) = sr.npv*255.f;
            mMCoef.at<uchar>(i,j) = sr.mCoef*255.f;
            iMatch++;
        }
    }

    int imgSize = size*cellSize;

    resize(mTP,mTP,Size(imgSize,imgSize),0,0,INTER_NEAREST);
    resize(mTN,mTN,Size(imgSize,imgSize),0,0,INTER_NEAREST);
    resize(mFP,mFP,Size(imgSize,imgSize),0,0,INTER_NEAREST);
    resize(mFN,mFN,Size(imgSize,imgSize),0,0,INTER_NEAREST);

    resize(mAcy,mAcy,Size(imgSize,imgSize),0,0,INTER_NEAREST);
    resize(mSen,mSen,Size(imgSize,imgSize),0,0,INTER_NEAREST);
    resize(mSpe,mSpe,Size(imgSize,imgSize),0,0,INTER_NEAREST);
    resize(mEffc,mEffc,Size(imgSize,imgSize),0,0,INTER_NEAREST);
    resize(mPpv,mPpv,Size(imgSize,imgSize),0,0,INTER_NEAREST);
    resize(mNpv,mNpv,Size(imgSize,imgSize),0,0,INTER_NEAREST);
    resize(mMCoef,mMCoef,Size(imgSize,imgSize),0,0,INTER_NEAREST);

    normalize(mTP, mTP,0,255,NORM_MINMAX);
    applyColorMap(mTP,mTP,COLORMAP_JET);

    normalize(mTN, mTN,0,255,NORM_MINMAX);
    applyColorMap(mTN,mTN,COLORMAP_JET);

    normalize(mFP, mFP,0,255,NORM_MINMAX);
    applyColorMap(mFP,mFP,COLORMAP_JET);

    normalize(mFN, mFN,0,255,NORM_MINMAX);
    applyColorMap(mFN,mFN,COLORMAP_JET);

    applyColorMap(mAcy,mAcy,COLORMAP_JET);
    applyColorMap(mSen,mSen,COLORMAP_JET);
    applyColorMap(mSpe,mSpe,COLORMAP_JET);
    applyColorMap(mEffc,mEffc,COLORMAP_JET);
    applyColorMap(mPpv,mPpv,COLORMAP_JET);
    applyColorMap(mNpv,mNpv,COLORMAP_JET);
    applyColorMap(mMCoef,mMCoef,COLORMAP_JET);

    // Draw grid
    for(unsigned lPos = 0 ; lPos < imgSize; lPos+=cellSize)
    {
        // Horizontal lines
        line(mTP,Point(0,lPos),Point(mTP.cols,lPos),Scalar(0.f,0.f,0.f));
        line(mTN,Point(0,lPos),Point(mTN.cols,lPos),Scalar(0.f,0.f,0.f));
        line(mFP,Point(0,lPos),Point(mFP.cols,lPos),Scalar(0.f,0.f,0.f));
        line(mFN,Point(0,lPos),Point(mFN.cols,lPos),Scalar(0.f,0.f,0.f));

        line(mAcy,Point(0,lPos),Point(mAcy.cols,lPos),Scalar(0.f,0.f,0.f));
        line(mSen,Point(0,lPos),Point(mSen.cols,lPos),Scalar(0.f,0.f,0.f));
        line(mSpe,Point(0,lPos),Point(mSpe.cols,lPos),Scalar(0.f,0.f,0.f));
        line(mEffc,Point(0,lPos),Point(mEffc.cols,lPos),Scalar(0.f,0.f,0.f));
        line(mPpv,Point(0,lPos),Point(mPpv.cols,lPos),Scalar(0.f,0.f,0.f));
        line(mNpv,Point(0,lPos),Point(mNpv.cols,lPos),Scalar(0.f,0.f,0.f));
        line(mMCoef,Point(0,lPos),Point(mMCoef.cols,lPos),Scalar(0.f,0.f,0.f));

        // Vertical lines
        line(mTP,Point(lPos,0),Point(lPos,mTP.rows),Scalar(0.f,0.f,0.f));
        line(mTN,Point(lPos,0),Point(lPos,mTN.rows),Scalar(0.f,0.f,0.f));
        line(mFP,Point(lPos,0),Point(lPos,mFP.rows),Scalar(0.f,0.f,0.f));
        line(mFN,Point(lPos,0),Point(lPos,mFN.rows),Scalar(0.f,0.f,0.f));

        line(mAcy,Point(lPos,0),Point(lPos,mAcy.rows),Scalar(0.f,0.f,0.f));
        line(mSen,Point(lPos,0),Point(lPos,mSen.rows),Scalar(0.f,0.f,0.f));
        line(mSpe,Point(lPos,0),Point(lPos,mSpe.rows),Scalar(0.f,0.f,0.f));
        line(mEffc,Point(lPos,0),Point(lPos,mEffc.rows),Scalar(0.f,0.f,0.f));
        line(mPpv,Point(lPos,0),Point(lPos,mPpv.rows),Scalar(0.f,0.f,0.f));
        line(mNpv,Point(lPos,0),Point(lPos,mNpv.rows),Scalar(0.f,0.f,0.f));
        line(mMCoef,Point(lPos,0),Point(lPos,mMCoef.rows),Scalar(0.f,0.f,0.f));
    }

    imwrite(imgPrefixName + "TruePositive.png",mTP);
    imwrite(imgPrefixName + "TrueNegative.png",mTN);
    imwrite(imgPrefixName + "FalsePositive.png",mFP);
    imwrite(imgPrefixName + "FalseNegative.png",mFN);

    imwrite(imgPrefixName + "Accuracy.png",mAcy);
    imwrite(imgPrefixName + "Sensitivity.png",mSen);
    imwrite(imgPrefixName + "Specificity.png",mSpe);
    imwrite(imgPrefixName + "Efficiency.png",mEffc);
    imwrite(imgPrefixName + "PositivePredition.png",mPpv);
    imwrite(imgPrefixName + "NegativePredition.png",mNpv);
    imwrite(imgPrefixName + "MatthewsCoef.png",mMCoef);

    imshow("TruePositive.png",mTP);
    imshow("TrueNegative.png",mTN);
    imshow("FalsePositive.png",mFP);
    imshow("FalseNegative.png",mFN);

    imshow("Accuracy.png",mAcy);
    imshow("Sensitivity.png",mSen);
    imshow("Specificity.png",mSpe);
    imshow("Efficiency.png",mEffc);
    imshow("PositivePredition.png",mPpv);
    imshow("NegativePredition.png",mNpv);
    imshow("MatthewsCoef.png",mMCoef);

    waitKey();
}

void ResultAnalysis::toSquaredImg(const string &imgName,
                                  const ResultData &gt, const ResultData &r,
                                  float gtTh, float rTh)
{
    const vector<RData> &gd = gt.data,
                        &rd = r.data;

    unsigned nMathchs = gd.size();
    unsigned imgSz = ceil(sqrt(nMathchs));

    Mat result(imgSz,imgSz, CV_8UC3, Scalar(255,0,255));

    for(unsigned iMatch = 0 ; iMatch < gd.size(); iMatch++)
    {
        const RData &gdi = gd[iMatch],
                    &rdi = rd[iMatch];

        if(gdi.uId != rdi.uId || gdi.vId != rdi.vId)
        {
            cout << "ResultAnalysis::toSquaredImg - Probem with index! ("
                 << gdi.uId << ',' << gdi.vId << ") != ("
                 << rdi.uId << ',' << rdi.vId << ')' << endl;
            continue;
        }

        unsigned i = iMatch/imgSz, j = iMatch%imgSz;
        Vec3b &p = result.at<Vec3b>(i,j);

        // Binarization
        bool bg = gdi.score > gtTh,
             br = rdi.score > rTh;

        if(br == true) // Positive?
        {
            if(bg == br) // true?
            {
                // Green
                p[0] = 0; p[1] = 255; p[2] = 0;
            }else // false
            {
                // Red
                p[0] = 0; p[1] = 0; p[2] = 255;
//                cout << "FP , " << gdi.uId << " , " << gdi.vId
//                     << " , "
//                     << gdi.score << " , " << rdi.score << endl;
            }
        }else if(br == false) // Negative?
        {
            if(bg == br) // true?
            {
                // Black
                p[0] = 0; p[1] = 0; p[2] = 0;
            }else // false
            {
                // Blue
                p[0] = 255; p[1] = 0; p[2] = 0;
//                cout << "FN , " << gdi.uId << " , " << gdi.vId
//                     << " , "
//                     << gdi.score << " , " << rdi.score << endl;
            }
        }
    }

    imwrite(imgName,result);

    namedWindow("Result Comp",WINDOW_NORMAL);
    imshow("Result Comp", result);
    waitKey();
}

void ResultAnalysis::toTriangImg(const string &imgName,
                                 const ResultData &gt, const ResultData &r,
                                 float gtTh, float rTh)
{
    const vector<RData> &gd = gt.data,
                        &rd = r.data;
    if(gd.size() == 0 || rd.size() == 0)
    {
        cout << "ResultAnalysis::toTriangImg() - Error, empty results!" << endl;
        return ;
    }

    unsigned sz = 0;
    for(unsigned iMatch = 0 ; iMatch < gd.size(); iMatch++)
    {
        const RData &gdi = gd[iMatch];
        if(sz < gdi.uId)
            sz = gdi.uId;
        if(sz < gdi.vId)
            sz = gdi.vId;
    }
    sz = sz+1;

    Mat result(sz,sz,CV_8UC3);
    for(unsigned iMatch = 0 ; iMatch < gd.size(); iMatch++)
    {
        const RData &gdi = gd[iMatch],
                    &rdi = rd[iMatch];

        if(gdi.uId != rdi.uId || gdi.vId != rdi.vId)
        {
            cout << "ResultAnalysis::toTriangImg() - Probem with index! ("
                 << gdi.uId << ',' << gdi.vId << ") != ("
                 << rdi.uId << ',' << rdi.vId << ')' << endl;
            continue;
        }

        Vec3b &p = result.at<Vec3b>(gdi.uId,gdi.vId);

        // Binarization
        bool bg = gdi.score > gtTh,
             br = rdi.score > rTh;

        if(br == true) // Positive?
        {
            if(bg == br) // true?
            {
                // Green
                p[0] = 0; p[1] = 255; p[2] = 0;
            }else // false
            {
                // Red
                p[0] = 0; p[1] = 0; p[2] = 255;
            }
        }else if(br == false) // Negative?
        {
            if(bg == br) // true?
            {
                // Black
                p[0] = 0; p[1] = 0; p[2] = 0;
            }else // false
            {
                // Blue
                p[0] = 255; p[1] = 0; p[2] = 0;
            }
        }
        // Also, define reverse triangle
        result.at<Vec3b>(gdi.vId,gdi.uId) = p;
    }
    imwrite(imgName,result);

    namedWindow("Result Comp",WINDOW_NORMAL);
    imshow("Result Comp", result);
    waitKey();
}

void ResultAnalysis::toFastTriangImg(const string &imgName,
                                 const ResultData &gt, const ResultData &r,
                                 float gtTh, float rTh)
{
    const vector<RData> &gd = gt.data,
                        &rd = r.data;
    if(gd.size() == 0 || rd.size() == 0)
    {
        cout << "ResultAnalysis::toTriangImg() - Error, empty results!" << endl;
        return ;
    }


//            img size and PA sum
//      n + n²                    sqrt(8sn+1)-1
//    ---------   = sn   ;  n =   --------------
//        2                             2

    unsigned sz = (sqrt(8*gd.size()+1) -1)/2;
    Mat result(sz,sz,CV_8UC3);

    for(unsigned iMatch = 0 ; iMatch < gd.size(); iMatch++)
    {
        const RData &gdi = gd[iMatch],
                    &rdi = rd[iMatch];

        if(gdi.uId != rdi.uId || gdi.vId != rdi.vId ||
           gdi.uId >= sz || gdi.vId >= sz)
        {
            cout << "ResultAnalysis::toTriangImg() - Probem with index! ("
                 << gdi.uId << ',' << gdi.vId << ") != ("
                 << rdi.uId << ',' << rdi.vId << ')' << endl;
            continue;
        }

        Vec3b &p = result.at<Vec3b>(gdi.uId,gdi.vId);

        // Binarization
        bool bg = gdi.score > gtTh,
             br = rdi.score > rTh;

        if(br == true) // Positive?
        {
            if(bg == br) // true?
            {
                // Green
                p[0] = 0; p[1] = 255; p[2] = 0;
            }else // false
            {
                // Red
                p[0] = 0; p[1] = 0; p[2] = 255;
            }
        }else if(br == false) // Negative?
        {
            if(bg == br) // true?
            {
                // Black
                p[0] = 0; p[1] = 0; p[2] = 0;
            }else // false
            {
                // Blue
                p[0] = 255; p[1] = 0; p[2] = 0;
            }
        }
        // Also, define reverse triangle
        result.at<Vec3b>(gdi.vId,gdi.uId) = p;
    }
    imwrite(imgName,result);

    namedWindow("Result Comp",WINDOW_NORMAL);
    imshow("Result Comp", result);
    waitKey();
}
