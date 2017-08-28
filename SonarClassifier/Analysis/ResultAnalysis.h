#ifndef RESULTANALYSIS_H
#define RESULTANALYSIS_H

#include "ResultData.h"
#include "StatisticResults.h"

enum RAType
{
    RA_TP, RA_TN, RA_FP, RA_FN
};

class RAData
{
public:
    unsigned uId, vId;
    RAType r;
};


/**
 * @brief The LoopClosureResultAnalysis class analyze
 *
 */
class ResultAnalysis
{
    vector<StatisticResults> statisticResults;
    bool compare(const ResultData &gt, const ResultData &r,
                 StatisticResults &sr, vector<RAData> &ra);

public:
    ResultAnalysis();

    bool process(const ResultData &gt, const ResultData &r,StatisticResults &sr);

    void fromResults(const ResultData &gt, const ResultData &r,
                    float gtThStart, float gtThEnd, float gtStep, float rThStart, float rThEnd, float rStep);

    bool fromCSV(const char *csvFileName);

    StatisticResults bestMatewCoef();
    StatisticResults bestMachado();
    StatisticResults bestPositivePreditionValue();
    StatisticResults bestPPVandTP();

    bool toCSV(const char *csvFileName);
    void toStatisticImgs(const string &imgPrefixName, unsigned cellSize=10);

    void toSquaredImg(const string &imgName,
                      const ResultData &gt, const ResultData &r,
                      float gtTh, float rTh);

    void toTriangImg(const string &imgName,
                      const ResultData &gt, const ResultData &r,
                      float gtTh, float rTh);

    void toFastTriangImg(const string &imgName,
                      const ResultData &gt, const ResultData &r,
                      float gtTh, float rTh);
};

#endif // RESULTANALYSIS_H
