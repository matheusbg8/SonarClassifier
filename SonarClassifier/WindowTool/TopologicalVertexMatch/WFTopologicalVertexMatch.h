#ifndef WFTOPOLOGICALVERTEXMATCH_H
#define WFTOPOLOGICALVERTEXMATCH_H

#include "WindowTool/WindowFeature.h"
#include "WindowTool/FrameSD.h"
#include "Sonar/Sonar.h"
#include "GraphMatcher/GraphMatcher.h"

class WFTopologicalVertexMatch : public WindowFeature
{
protected:
    FrameSD *frame(unsigned i);

    vector<SonarDescritor*> sds;
    ConfigLoader sonarConfig;
    Sonar sonar;
    GraphMatcher gm;
    vector<MatchInfo> matchInfo;
    MatchInfoExtended VertexMatchInfo;

    Mat transform;

    int leftSelecGaussian, rightSelecGaussian;

    int selectGaussianOnFrame(int x,  int y , int frameID, float precision=50.f);
    void removeGaussianSelections();

    bool showVertexMatch,
         showCorrectOrientation,
         hasOdometry;

    void explore(unsigned edgeId);
    void selectBestPairOfLeftGaussian();

    void odometry(SonarDescritor *sd1, SonarDescritor *sd2,
                  vector<MatchInfo> &matchs);

    void odometryImg(SonarDescritor *sd1, SonarDescritor *sd2,
                  vector<MatchInfo> &matchs);

    double dist(Point2f p1, Point2f p2);

    void transformPoint(Point2f &p, Mat &T);
    void transformPoints(vector<Point2f> &pts, Mat &T);

    void extractMatchPoints(const vector<Gaussian> &gl, const vector<Gaussian> &gr,
                            const vector<MatchInfo> &matchs,
                            vector<Point2f> &rightMatchsPts, vector<Point2f> &leftMatchsPts);

public:
    WFTopologicalVertexMatch();

    // WindowFeature interface
public:
    void start();
    Frame *newFrame(const string &fileName, unsigned frameNumber);
    void keyPress(char c);
    void mouseEvent(int event, int x, int y);
    void renderProcess(Mat &leftImg, int leftId, Mat &rightImg, int rightId);
    void renderFrameTogether(Mat &screen,
                             const Scalar_<float> &el, unsigned leftFrameId,
                             const Scalar_<float> &er, unsigned rightFrameId);
    void processImages(Mat &leftImg, int leftId, Mat &rightImg, int rightId);

};

#endif // WFTOPOLOGICALVERTEXMATCH_H
