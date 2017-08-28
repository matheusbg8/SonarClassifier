#ifndef KNEARESTCLASSIFIER_H
#define KNEARESTCLASSIFIER_H

#include "Classifier.h"

#include <opencv2/core/core.hpp>
#include <opencv2/ml/ml.hpp>

using namespace std;
using namespace cv;

class KNearestClassifier : public Classifier
{
public:
    KNearestClassifier(unsigned K=10);

    KNearest Kn;
    unsigned K;
    bool isTrained;

    // Classifier interface
public:
    void training(Mat &labels, Mat &data);
    void predict(Mat &data, Mat &predictions);
    double predict(Mat &data);
    bool trained();
};

#endif // KNEARESTCLASSIFIER_H
