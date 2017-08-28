#ifndef CLOSELOOPANALISERESULT_H
#define CLOSELOOPANALISERESULT_H

#include <vector>
#include <string>
using namespace std;

#include "GraphMatcher/MatchInfo/MatchInfoWeighted.h"


/**
 * @brief This class do result analise
 *
 */
class CloseLoopAnaliseResult
{
private:
    typedef pair<unsigned,unsigned> PUU;
    typedef pair<unsigned,float> PUF;

    vector< vector< PUF > > resultGraph;
    /**< Obtained Results, resultGraph[FirstFrameID][edgeID](first=SecondFrameID, second=score(e.g. Number of correspondent vertex between the frames) */

    vector< PUU > frResults;
    /**< Frame Despription Information, frResults[FrameID](Firs=Number of Vertexs, Second=Number of Edges) */

    vector< vector< PUF > > gtGraph;
    /**< Ground Truth Results, first = dest frame ID , second = score (nomalized distance until img center) */

    string destPath;
    /**< Path to save the analise results */

    vector<unsigned> remap;

    void loadFrameDescriptionInformation(const char *fileName= "Results/ResultFramesInformations.csv");
    void loadResultOfMatch(const char *prefix="Results/LoopDetections/MatchResults_fr");
    void loadDirectResult(const char *csvFileName, unsigned nFrames);

    void loadGtMatch(const char *gtFileName="GTMatchs.csv");
    void remapGtMatch();

    void loadGtMatchRemap(const char *gtFileName="GTMatchs.csv");

    void normalizeResult();
    void normalizeResult2(unsigned maxValue);

    float findGtScore(unsigned uFr, unsigned vFr);

public:

    CloseLoopAnaliseResult(const char *destPath);

    void processDeepLearningResults(const char *csvGTName, const char *csvResultsName);

    void saveResults(const char *fileName = "Results/ResultFramesInformations.csv");
    void saveSplitedGTResults(unsigned uFr);
    void saveSplitedGTResults();

    void loadAnalisyAndSave();

    void saveAnalisyResults(const char *fileName= "CompareResult.csv");

    void splitGTResults();

    void generateGTImage();
    void generateResultImage();
    void generatePairFeaturesCount();

};

#endif // CLOSELOOPANALISERESULT_H
