#include "KNearestClassifier.h"

KNearestClassifier::KNearestClassifier(unsigned K):
    isTrained(false),
    K(K)
{

}

void KNearestClassifier::training(Mat &labels, Mat &data)
{
    Kn.train(data,labels,Mat(),true,K,false);
    isTrained = true;
}

void KNearestClassifier::predict(Mat &data, Mat &predictions)
{
    int nsamples_all = data.rows;
    predictions = Mat(data.rows,1,CV_32F);

    for(int i = 0; i < nsamples_all; i++ )
    {
        Mat sample = data.row(i);
        predictions.at<float>(i,0) = Kn.find_nearest(sample,K);
    }
}

double KNearestClassifier::predict(Mat &data)
{
    return Kn.find_nearest(data,K);
}

bool KNearestClassifier::trained()
{
    return isTrained;
}

