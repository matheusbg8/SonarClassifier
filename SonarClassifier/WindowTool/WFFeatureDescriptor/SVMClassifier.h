#ifndef SVMCLASSIFIER_H
#define SVMCLASSIFIER_H

#include "Classifier.h"

#include <opencv2/core/core.hpp>
#include <opencv2/ml/ml.hpp>

using namespace cv;

class SVMClassifier : public Classifier
{
public:
    SVMClassifier(bool autoTrain=false);

    CvSVM SVM;

    float gamma, C;
    int kernel_type;
    bool autoTrain; /// If it's true, the auto training procedures are called (slower)

    bool svmTrained;

    // Classifier interface
public:
    void training(Mat &labels, Mat &data);
    void predict(Mat &data, Mat &predictions);
    double predict(Mat &data);
    bool trained();
};

#endif // SVMCLASSIFIER_H
