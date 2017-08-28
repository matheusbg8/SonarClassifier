#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include <vector>
#include <opencv2/core/core.hpp>

using namespace cv;
using namespace std;

class Classifier
{
public:
    Classifier();

    virtual void training(Mat &labels, Mat &data) =0;
    virtual void predict(Mat &data, Mat &predictions) =0;
    virtual double predict(Mat &data) =0;
    virtual bool trained() =0;

};

#endif // CLASSIFIER_H
