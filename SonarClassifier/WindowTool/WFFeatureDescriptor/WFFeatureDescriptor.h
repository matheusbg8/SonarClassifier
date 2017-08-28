#ifndef WFFEATUREDESCRIPTOR_H
#define WFFEATUREDESCRIPTOR_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>

#include "WindowTool/WindowFeature.h"
#include "WindowTool/GaussianDescriptor/GausianDescriptorFeature.h"
#include "Description.h"
#include "Classifier.h"

/**
 * @brief The WFFeatureDescriptor class - It implements the classification
 * feature to Window Tool. It allows a manual classification of the
 * sonar objects detected by GaussianDescriptonFeature. It allows the
 * use of the classification data to train some classifier algorithms
 * that implement the Classifier interface, some results can be generated
 * and visualized.
 *
 */
class WFFeatureDescriptor : public WindowFeature,
                            public GausianDescriptorFeature
{
public:
    WFFeatureDescriptor(Classifier *classifier);

    float traningDataPercent; /// normalized (0 to 1) percentage value of data used on the training step (standard= 1)

    bool normalizeData, /// If it's true, all data are normalized before training (mandatory for SVM classifier)
         trainingDataReady; /// Flag that start false and become true after calling makeTraningData

    // The vector above are only avalible after makeTrainingData call (trainingDataReady=true)
    vector<double> minVal, maxVal;

    Classifier *classifier;

    static char classesName[10][200];
    Scalar getClassColor(float label);

    void doTraining();
    void showTrainResults();

    void takeMinVal(vector<double> &min, vector<double> &data);
    void takeMaxVal(vector<double> &max, vector<double> &data);

    void makeTraningData(Mat &traningLabels, Mat &trainingData,
                         Mat &validationLabels, Mat &validationData);

    double computeHitPercentage(Mat &labels, Mat &data, vector<pair<unsigned, unsigned> > &results);
    double computeHitPercentage(vector<pair<unsigned, unsigned> > &results);

    void computeVector(vector<double> &v, Gaussian &g);
    //===============

    int lastSelectedObjectId,lastSelecFrameId;
    bool showFrameInfo;
    Size trainResultWindowSize;

    vector<DescriptionFrame> frames; // Only avalible after makeTrainData call

    vector< vector<Description> > allSVMByClass; /** It's valid only after svmTrained= true by
                                                   * calling doTraining() or makeTraningData() methods.
                                                   * The data are organized such that allSVMByClass[labelId][DescriptionIterator].
                                                   */

    void renderFrame(Mat &img, int frameId);
    void _renderFrameTogether(Mat &screen,const Scalar_<float> &e,unsigned frameId);

    void alocateFrame(unsigned frameId);

    void showSVMFeatures(unsigned frameId, unsigned svmId);

    // WindowFeature interface
public:
    void start();

//    Frame *newFrame(const string &fileName, unsigned frameNumber); // not used
    void selectedFrame(int frameId);
    void keyPress(char c);
    void mouseEvent(int event, int x, int y);
    void renderProcess(Mat &leftImg, int leftId, Mat &rightImg, int rightId);
    void renderFrameTogether(Mat &screen,
                             const Scalar_<float> &el, unsigned leftFrameId,
                             const Scalar_<float> &er, unsigned rightFrameId);
    void processImages(Mat &leftImg, int leftId, Mat &rightImg, int rightId);

    double euclidianDist(vector<double> v1, vector<double> v2);

    void searchSimilar();

    // GausianDescriptorFeature interface
public:
    void newGaussian(int gId, int frameId);
    void leftGaussianSelected(int gId, int frameId);
    void rightGaussianSeleced(int gId, int frameId);
    void cleanedGaussians(int frameId);


// Save and load config
    bool save(const char *fileName);
    bool load(const char *fileName);

    bool loadGt(const char *fileName);
    bool saveGt(const char *fileName);

    string workFileName;
    void setWorkFile(const char *fileName);
    void autoSave();
    bool autoLoad();
};

#endif // WFFEATUREDESCRIPTOR_H
