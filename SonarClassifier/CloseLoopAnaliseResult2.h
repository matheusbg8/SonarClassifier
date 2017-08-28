#ifndef CLOSELOOPANALISERESULT2_H
#define CLOSELOOPANALISERESULT2_H

#include <vector>
#include <string>
using namespace std;

class CLR2Data
{
public:
    unsigned uId, vId;
    float gtScore, rScore, feturesCount;
};


/**
 * @brief The CloseLoopAnaliseResult2 class analise
 * Loope Clusure Results using a matrix instead of a graph
 * as in CloseLoopAnaliseResult.
 */
class CloseLoopAnaliseResult2
{
protected:
    vector<CLR2Data> data;

    string workPath;
    /**< Path to save the analise results */

public:
    CloseLoopAnaliseResult2(const char *workPath);

    void process(const char *csvGTName,const char *csvResultName);
    void processResultFromImage(const char *csvGTName,const char *greyImgResultName);

    bool loadGT(const char *csvFileName);

    bool loadResults(const char *csvFileName);
    bool loadResultsFromTriangGreyImage(const char *img8BitFileName);
    bool loadFeaturesCountFromTriangGreyImage(const char *img8BitFileName);

    void generateGTImage();
    void generateResultImage();
};

#endif // CLOSELOOPANALISERESULT2_H
