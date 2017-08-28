#include "WFTopologicalVertexMatch.h"
#include "WindowTool/WindowTool.h"

#include "opencv2/video/tracking.hpp"


#include "Drawing/Drawing.h"

FrameSD *WFTopologicalVertexMatch::frame(unsigned i)
{
    // Yes, we are breaking some SOLID principles of OO Programming
    // if you know a better way to solve this, please send me a e-mail
    // matheusbg8@gmail.com, thank you!
    return (FrameSD *) wt->frames[i];
}

int WFTopologicalVertexMatch::selectGaussianOnFrame(int x, int y, int frameID, float precision)
{
    int gaussianID = -1;
    float minDist=FLT_MAX;
    vector<Gaussian> &gaussians = sds[frameID]->gaussians;

    for(unsigned g = 0 ; g < gaussians.size(); g++)
    {
        float dx = gaussians[g].x - x,
              dy = gaussians[g].y - y,
              dist = sqrt(dx*dx + dy*dy);

        if(minDist > dist )
        {
            gaussianID = g;
            minDist = dist;
        }
    }

    if(minDist > precision)
    {
        gaussianID = -1;
    }

    return gaussianID;
}

void WFTopologicalVertexMatch::removeGaussianSelections()
{
    leftSelecGaussian = rightSelecGaussian = -1;
}

void WFTopologicalVertexMatch::explore(unsigned edgeId)
{
    if(leftSelecGaussian >=0 && rightSelecGaussian>=0)
    {
        vector<MatchInfoWeighted> &edgeMatch = VertexMatchInfo.edgeMatchInfo;
        vector<GraphLink*> &leftEdges = sds[wt->currentLeftFrame]->graph[leftSelecGaussian],
                           &rightEdges = sds[wt->currentRightFrame]->graph[rightSelecGaussian];

        if(edgeId < edgeMatch.size() )
        {
            GraphLink *leftEdge = leftEdges[edgeMatch[edgeId].uID],
                      *rightEdge = rightEdges[edgeMatch[edgeId].vID];

            leftSelecGaussian = leftEdge->dest;
            rightSelecGaussian = rightEdge->dest;
        }
    }
}

void WFTopologicalVertexMatch::selectBestPairOfLeftGaussian()
{
    if(leftSelecGaussian >= 0)
    {
        unsigned leftId = wt->getLeftFrameId(),
                 rightId = wt->getRighFrameId();

        vector<Gaussian> &leftGaussians = sds[leftId]->gaussians,
                         &rightGaussians = sds[rightId]->gaussians;

        vector<vector<GraphLink*> > &leftGraph = sds[leftId]->graph,
                                    &rightGraph = sds[rightId]->graph;
        vector<MatchInfoWeighted> edgeMatchInfo;

        int bestMatchId=-1;
        unsigned nMatchs=0;
        float error, lowerError= 999999.9f;
        for(unsigned grightId=0; grightId < rightGraph.size(); grightId++)
        {
            error = sonar.computeVertexMatch(
                leftGaussians[leftSelecGaussian],
                rightGaussians[grightId],
                leftGraph[leftSelecGaussian],
                rightGraph[grightId],
                edgeMatchInfo);

            if(edgeMatchInfo.size() > nMatchs ||
               (edgeMatchInfo.size() == nMatchs && error < lowerError) )
            {
                nMatchs = edgeMatchInfo.size();
                bestMatchId = grightId;
                lowerError = error;
            }
        }
        if(bestMatchId >= 0)
        {
            rightSelecGaussian = bestMatchId;
        }
    }
}

void WFTopologicalVertexMatch::odometry(SonarDescritor *sd1, SonarDescritor *sd2,
                                        vector<MatchInfo> &matchs)
{
    if(matchs.size() < 3)
    {
        cout << "No odometry!" << endl;
        return;
    }

    Point2f src[matchInfo.size()], dst[matchInfo.size()];
    vector<Gaussian> &srcGs = sd1->gaussians,
                     &dstGs = sd2->gaussians;

    for(unsigned i = 0; i < matchInfo.size(); i++)
    {
        Gaussian &srcG = srcGs[matchInfo[i].uID],
                 &dstG = dstGs[matchInfo[i].vID];

        src[i] = Point2f(srcG.x, srcG.y);
        dst[i] = Point2f(dstG.x, dstG.y);
    }

    Mat M = getAffineTransform(src,dst);
    cout << "Odometry:" << endl
         << "Translation " << M.at<double>(0,2)*30.0/781.0 << "m , " << M.at<double>(1,2)*30.0/781.0 << "m" << endl
         << "Rotation " << (atan(M.at<double>(0,0)/ M.at<double>(0,1))*180.f/M_PI)
         << " , cosA " << M.at<double>(0,0)
         << " , sinA " << M.at<double>(0,1) << endl
         << M << endl << endl;
//         << "Scale " <<
}

void WFTopologicalVertexMatch::odometryImg(SonarDescritor *sd1, SonarDescritor *sd2,
                                           vector<MatchInfo> &matchs)
{
    if(matchs.size() < 3)
    {
        cout << "No odometryImg!" << endl;
        hasOdometry = false;
        return;
    }

    vector<Point2f> src(matchInfo.size()), dst(matchInfo.size());
    vector<Gaussian> &srcGs = sd1->gaussians,
                     &dstGs = sd2->gaussians;

    for(unsigned i = 0; i < matchInfo.size(); i++)
    {
        Gaussian &srcG = srcGs[matchInfo[i].uID],
                 &dstG = dstGs[matchInfo[i].vID];

        src[i] = Point2f(srcG.x, srcG.y);
        dst[i] = Point2f(dstG.x, dstG.y);
    }

    Mat M;
//    M = getAffineTransform(dst,src);
    M = estimateRigidTransform(dst,src,false);

    if(M.rows == 0)
    {
        cout << "WFTopologicalVertexMatch::odometryImg - Problem with Rigid Transform Estimatiom!" << endl;
        return ;
    }

    Mat &leftImg = wt->leftImg;

    Point2f lP1(leftImg.cols/2, leftImg.rows),
            lP2(leftImg.cols/2, leftImg.rows/2),
            rP1(lP1.x*M.at<double>(0,0) + lP1.y*M.at<double>(0,1) + M.at<double>(0,2),
                lP1.x*M.at<double>(1,0) + lP1.y*M.at<double>(1,1) + M.at<double>(1,2)),
            rP2(lP2.x*M.at<double>(0,0) + lP2.y*M.at<double>(0,1) + M.at<double>(0,2),
                lP2.x*M.at<double>(1,0) + lP2.y*M.at<double>(1,1) + M.at<double>(1,2));

    float lDist = dist(lP2 , lP1),
          rDist = dist(rP2 , rP1);
    float lAng = atan2(lP2.y-lP1.y, lP2.x-lP1.x)*180.f/M_PI,
          rAng = atan2(rP2.y-rP1.y, rP2.x-rP1.x)*180.f/M_PI;
    float scaleM = dist(Point2f(M.at<double>(0,0),M.at<double>(0,0)),
                        Point2f(M.at<double>(1,0),M.at<double>(1,0)));


    cout << "OdometryImg:" << endl
         << "Translation M " << M.at<double>(0,2) << " , " << M.at<double>(1,2) << endl
         << "Translation P " << (lP1.x - rP1.x) << " , " << (lP1.y - rP1.y) << endl
         << "Rotation M " << (atan(M.at<double>(0,0)/ M.at<double>(0,1))*180.f/M_PI)
         << ", cosA " << M.at<double>(0,0) << ", sinA " << M.at<double>(0,1) << endl
         << "Rotation P " << fabs(lAng-rAng)
         << "lAng " << lAng << " rAng " << rAng << endl
         << "Scale M " << scaleM << endl
         << "Scale P " << lDist/rDist << endl
         << M << endl << endl;
    //         << "Scale " <<

    Mat colorImg;
    leftImg.convertTo(colorImg,CV_8UC1);
    cvtColor(colorImg,colorImg,CV_GRAY2BGR);

    Drawing::drawArrow(colorImg,lP1,lAng,100.f,Scalar(255.f,0.f,0.f));
    Drawing::drawArrow(colorImg,rP1,rAng,100.f,Scalar(0.f,0.f,255.f));

//    line(colorImg,lP1,lP2,Scalar(255.f,0.f,0.f));
//    line(colorImg,rP1,rP2,Scalar(0.f,0.f,255.f));

    imshow("Vev Result", colorImg);

    transform = M;
    hasOdometry = true;
}

double WFTopologicalVertexMatch::dist(Point2f p1, Point2f p2)
{
    float dx= p1.x - p2.x,
          dy= p1.y - p2.y;

    return sqrt(dx*dx + dy*dy);
}

/**
 * @brief WFTopologicalVertexMatch::transformPoint - Apply an Afin transfor to the point (in place)
 * @param p - Input point, output point
 * @param T - Afin transform, a 2x3Matrix
 */
void WFTopologicalVertexMatch::transformPoint(Point2f &p, Mat &T)
{
    float x, y;
    x = p.x*T.at<double>(0,0) + p.y*T.at<double>(0,1) + T.at<double>(0,2);
    y = p.x*T.at<double>(1,0) + p.y*T.at<double>(1,1) + T.at<double>(1,2);
    p.x = x; p.y = y;
}

/**
 * @brief WFTopologicalVertexMatch::transformPoints - Apply afin transform in a set of points (in place)
 * @param pts - Point vector
 * @param T - Affin transform (2x3 matrix)
 */
void WFTopologicalVertexMatch::transformPoints(vector<Point2f> &pts, Mat &T)
{
    for(unsigned i = 0 ; i < pts.size(); i++)
    {
        float x, y;
        Point2f &p = pts[i];
        x = p.x*T.at<double>(0,0) + p.y*T.at<double>(0,1) + T.at<double>(0,2);
        y = p.x*T.at<double>(1,0) + p.y*T.at<double>(1,1) + T.at<double>(1,2);
        p.x = x; p.y = y;
    }
}

/**
 * @brief WFTopologicalVertexMatch::extractMatchPoints - Extract match points from gaussians
 * @param gl - left gaussian. (input)
 * @param gr - Right gaussian. (input)
 * @param matchs - Matchs information. (input)
 * @param rightMatchsPts - Right match points. (output)
 * @param leftMatchsPts - Left match points. (output)
 */
void WFTopologicalVertexMatch::extractMatchPoints(const vector<Gaussian> &gl,const vector<Gaussian> &gr,
                                                  const vector<MatchInfo> &matchs,
                                                  vector<Point2f> &rightMatchsPts, vector<Point2f> &leftMatchsPts)
{
    rightMatchsPts.resize(matchs.size());
    leftMatchsPts.resize(matchs.size());
    for(unsigned i = 0 ; i < matchs.size(); i++)
    {
        unsigned u = matchs[i].uID,
                 v = matchs[i].vID;
        leftMatchsPts[i] = Point2f(gl[u].x,gl[u].y);
        rightMatchsPts[i] = Point2f(gr[v].x,gr[v].y);
    }
}

WFTopologicalVertexMatch::WFTopologicalVertexMatch():
    leftSelecGaussian(-1),
    rightSelecGaussian(-1),
    showVertexMatch(true),
    showCorrectOrientation(true),
    sonar(sonarConfig),
    sonarConfig("../SonarGaussian/Configs.ini"),
    hasOdometry(false)
{
    sonar.setStoreImgs(false);

    if(showVertexMatch)
    {
        namedWindow("VertexMatch", 1);
    }
}

void WFTopologicalVertexMatch::start()
{
//        wt->currentLeftFrame = 129;
//        wt->currentRightFrame = 180;
}

Frame *WFTopologicalVertexMatch::newFrame(const string &fileName, unsigned frameNumber)
{
    // New empty Sonar Descriptor
    sds.push_back(0x0);
    return new FrameSD(fileName,frameNumber);
}

void WFTopologicalVertexMatch::keyPress(char c)
{
    switch(c)
    {
    case 'u':
        sonar.segmentationCalibUI(wt->selectedFrameImg());
    break;
    case '1':
        explore(0);
    break;
    case '2':
        explore(1);
    break;
    case '3':
        explore(2);
    break;
    case '4':
        explore(3);
    break;
    case 'b':
        selectBestPairOfLeftGaussian();
    break;
    }
}

void WFTopologicalVertexMatch::mouseEvent(int event, int x, int y)
{
    if(event == CV_EVENT_LBUTTONUP)
    {
        if(x < wt->windowSize.width*0.5f) // Left Frame
        {
            x = (x-wt->el.val[2])/wt->el.val[0];
            y = (y-wt->el.val[3])/wt->el.val[1];

            leftSelecGaussian = selectGaussianOnFrame(x,y,wt->currentLeftFrame);

            cout << "Left transform:"
                 << " dx " << wt->el.val[2]
                 << " dy " << wt->el.val[3]
                 << " ex " << wt->el.val[0]
                 << " ey " << wt->el.val[1]
                 << " mx " << x
                 << " my " << y
                 << " left gaussian " << leftSelecGaussian
                 << endl;

            cout << "selected gaussian"
                 << sds[wt->currentLeftFrame]->gaussians[leftSelecGaussian]
                 << endl;

        }else // Right Frame
        {
            x = (x-wt->er.val[2])/wt->er.val[0];
            y = (y-wt->er.val[3])/wt->er.val[1];

            rightSelecGaussian = selectGaussianOnFrame(x,y,wt->currentRightFrame);

            cout << "Right transform:"
                 << " dx " << wt->er.val[2]
                 << " dy " << wt->er.val[3]
                 << " ex " << wt->er.val[0]
                 << " ey " << wt->er.val[1]
                 << " mx " << x
                 << " my " << y
                 << " right gaussian " << rightSelecGaussian
                 << endl;

            cout << "selected gaussian"
                 << sds[wt->currentRightFrame]->gaussians[rightSelecGaussian]
                 << endl;

        }
    }
}

void WFTopologicalVertexMatch::renderProcess(Mat &leftImg, int leftId, Mat &rightImg, int rightId)
{
    SonarDescritor *rightSD = sds[rightId],
                   *leftSD = sds[leftId];

    Drawing::drawDescriptorColor(rightImg,rightSD,Scalar(0,0,255),Scalar(0,255,0),false,false,false,true,false,false);
    Drawing::drawDescriptorColor(leftImg,leftSD,Scalar(0,0,255),Scalar(0,255,0),false,false,false,true,false,false);

//    for(unsigned i = 0; i < matchInfo.size(); i++)
//    {
//        unsigned next = (i+1)%matchInfo.size();
//        const Gaussian &lg = leftSD->gaussians[matchInfo[i].uID],
//                       &nlg = leftSD->gaussians[matchInfo[next].uID],
//                       &rg = rightSD->gaussians[matchInfo[i].vID],
//                       &nrg = rightSD->gaussians[matchInfo[next].vID];

//        line(leftImg,
//             Point2f(lg.x,lg.y),
//             Point2f(nlg.x, nlg.y),
//             Drawing::color[i%Drawing::nColor],4);

//        line(rightImg,
//             Point2f(rg.x,rg.y),
//             Point2f(nrg.x, nrg.y),
//             Drawing::color[i%Drawing::nColor],4);

//    }

//    if(matchInfo.size() > 0)
//    {
//        const Gaussian &leftRef = leftSD->gaussians[matchInfo[0].uID],
//                       &rightRef = rightSD->gaussians[matchInfo[0].vID];

//        for(unsigned i = 1; i < matchInfo.size(); i++)
//        {
//            const Gaussian &lg = leftSD->gaussians[matchInfo[i].uID],
//                           &rg = rightSD->gaussians[matchInfo[i].vID];

//            line(leftImg,
//                 Point2f(leftRef.x,leftRef.y),
//                 Point2f(lg.x, lg.y),
//                 Scalar(0.f,255.f,255.f),4);

//            line(rightImg,
//                 Point2f(rightRef.x,rightRef.y),
//                 Point2f(rg.x, rg.y),
//                 Scalar(0.f,255.f,255.f),4);

//        }
//    }

    if(leftSelecGaussian >= 0)
    {
        vector<Gaussian> &leftGaussians = sds[leftId]->gaussians;
        Drawing::drawGaussian(leftImg,leftGaussians[leftSelecGaussian],
                              Scalar(0,255,0),Scalar(0,0,255),leftSelecGaussian);

//        Drawing::drawVertex(leftImg,sds[leftId],leftSelecGaussian,
//                            Scalar(0,255,0),Scalar(0,0,255));
    }

    if(rightSelecGaussian >= 0)
    {

        vector<Gaussian> &rightGaussians = sds[rightId]->gaussians;
        Drawing::drawGaussian(rightImg,rightGaussians[rightSelecGaussian],
                              Scalar(0,255,0),Scalar(0,0,255),rightSelecGaussian);

//        Drawing::drawVertex(rightImg,sds[rightId],rightSelecGaussian,
//                            Scalar(0,255,0),Scalar(0,0,255));

    }

    if(showVertexMatch && leftSelecGaussian >=0 && rightSelecGaussian>=0)
    {
//        gm.computeEdgeMatch(sds[leftId]->graph[leftSelecGaussian],
//                            sds[rightId]->graph[rightSelecGaussian],
//                            matchs[0].edgeMatchInfo);
        VertexMatchInfo.uID = leftSelecGaussian;
        VertexMatchInfo.vID = rightSelecGaussian;
        VertexMatchInfo.sBestScore = 99999.9f;

        /// @todo - Maybe use a specific Vertex Match here
        VertexMatchInfo.edgeMatchInfo.clear();

        VertexMatchInfo.bestScore = sonar.computeVertexMatch(
            sds[leftId]->gaussians[leftSelecGaussian],
            sds[rightId]->gaussians[rightSelecGaussian],
            sds[leftId]->graph[leftSelecGaussian],
            sds[rightId]->graph[rightSelecGaussian],
            VertexMatchInfo.edgeMatchInfo);

        vector<MatchInfoWeighted> &edgeMatchs = VertexMatchInfo.edgeMatchInfo;
        cout << "Edges match found:" << endl;
        for(unsigned i = 0; i < edgeMatchs.size() ; i++)
        {
            cout << edgeMatchs[i].uID << " -> " << edgeMatchs[i].vID<< endl;
        }

        Mat imgVertexMatch;

        Drawing::drawVertexMatch(imgVertexMatch,Size2i(1000,600),
                                 *sds[leftId],*sds[rightId],
                                 leftSelecGaussian,rightSelecGaussian,
                                 VertexMatchInfo.edgeMatchInfo,
                                 false,true,
                                 Scalar(0,0,255),Scalar(0,255,0),2);

        imshow("VertexMatch",imgVertexMatch);

//        vector<MatchInfoExtended> graphMatch(1,initialVertexMatch);

//        gm.findGraphMatchWithInitialGuess(sds[leftId]->graph[leftSelecGaussian],
//                                          sds[rightId]->graph[rightSelecGaussian],
//                                          initialVertexMatch,graphMatch);


        Drawing::drawGraphVertexMatchs(leftImg,rightImg,
                                    *sds[leftId],*sds[rightId],
                                    VertexMatchInfo,2);

//        Mat imgVertexMatch(800,1000,CV_8UC3,Scalar_<uchar>(0,0,0));

//        Drawing::drawVertex(imgVertexMatch,sds[leftId],leftSelecGaussian,
//                            Rect(0,0,500,800),
//                            Scalar(0,0,255),Scalar(0,255,0),true);

        Drawing::drawVertex(imgVertexMatch,sds[rightId],rightSelecGaussian,
                            Rect(500,0,500,800),
                            Scalar(0,0,255),Scalar(0,255,0),true);


    }

    if(showCorrectOrientation && hasOdometry)
    {
        vector<Point2f> sonPts;
        Drawing::getSonarShape(Point2f(rightImg.cols/2, rightImg.rows),130.f,
                               rightImg.rows,180.f,sonPts,10);

//        Drawing::drawPoly(leftImg,sonPts,Scalar(0,255,0));

//        imshow("Left Transform", leftImg);

        transformPoints(sonPts,transform);

        Point2f mp,Mp;

        Drawing::bouldingBox(sonPts,mp,Mp);

        transform.at<double>(0,2) -= mp.x;
        transform.at<double>(1,2) -= mp.y;

        Mat tmp(Mp.y-mp.y,Mp.x-mp.x,CV_8UC3);

        warpAffine(rightImg,tmp,transform,tmp.size());

        rightImg = tmp;
        imshow("Righ Transform", tmp);
    }
}

void WFTopologicalVertexMatch::renderFrameTogether(Mat &screen, const Scalar_<float> &el, unsigned leftFrameId,
                                                                const Scalar_<float> &er, unsigned rightFrameId)
{
    // Draw texts
    char tempStr[200];
    sprintf(tempStr,
            "Left Gaussian %d",
            leftSelecGaussian);
    putText(screen,tempStr,
           Point2f(30.f, wt->windowSize.height-10),
           FONT_HERSHEY_COMPLEX,0.5,
            Scalar(255,255,255),2);

    sprintf(tempStr,
            "Right Gaussian %d",
            rightSelecGaussian);
    putText(screen,tempStr,
           Point2f(wt->windowSize.width*0.5f + 30.f, wt->windowSize.height-10),
           FONT_HERSHEY_COMPLEX,0.5,
            Scalar(255,255,255),2);

    vector<Point2f> rightPts,
                    leftPts;

    extractMatchPoints(sds[leftFrameId]->gaussians,
                       sds[rightFrameId]->gaussians,
                       matchInfo, rightPts,leftPts);

    if(showCorrectOrientation && hasOdometry)
    {
        transformPoints(rightPts,transform);
    }

    Drawing::drawMatchings(screen,el,er,
                           leftPts,rightPts,Scalar(255.f,255.f,0));

    imwrite("Salva.png",screen);
}

void WFTopologicalVertexMatch::processImages(Mat &leftImg, int leftId, Mat &rightImg, int rightId)
{
    switch(wt->selecFrame)
    {
        case -1: removeGaussianSelections(); break;
        case 0: leftSelecGaussian = -1; break;
        case 1: rightSelecGaussian = -1; break;
    }

    if(sds[leftId] == 0x0)
    {
        sds[leftId] = sonar.newImageDirect(leftImg);
    }

    if(sds[rightId] == 0x0)
    {
        sds[rightId] = sonar.newImageDirect(rightImg);
    }

    matchInfo.clear();
    sonar.computeMatchs(sds[leftId], sds[rightId], matchInfo);

    odometry(sds[leftId], sds[rightId], matchInfo);
    odometryImg(sds[leftId], sds[rightId], matchInfo);

    cout << "Processed frames rightId "
         << rightId
         << " and leftId "
         << leftId
         << endl;
}
