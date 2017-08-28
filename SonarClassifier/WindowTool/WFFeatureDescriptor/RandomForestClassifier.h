#ifndef RANDOMFORESTCLASSIFIER_H
#define RANDOMFORESTCLASSIFIER_H

#include "Classifier.h"

#include <opencv2/core/core.hpp>
#include <opencv2/ml/ml.hpp>

using namespace std;
using namespace cv;

class RandomForestClassifier : public Classifier
{
public:
    RandomForestClassifier();

    RandomTrees randomTrees;
    bool isTrained;

    // Classifier interface
public:
    void training(Mat &labels, Mat &data);
    void predict(Mat &data, Mat &predictions);
    double predict(Mat &data);
    bool trained();
};

#endif // RANDOMFORESTCLASSIFIER_H
