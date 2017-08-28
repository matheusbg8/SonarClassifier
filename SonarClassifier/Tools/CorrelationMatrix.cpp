#include "CorrelationMatrix.h"

/**
 * @brief calcCorrelactionMatrix - Compute CorrelationMatrix using
 * Person correlation coeficient and admiting that samples are rows indexed.
 * First row relates first sample, N row relates to N sample
 * First column relate first variable and so on.
 * @param m - The sample matrix.
 * @param mCorrelact - The computed correlation matrix.
 */
void calcCorrelactionMatrix(Mat &m, Mat &mCorrelact)
{
    // Compute mean and std
    Scalar mMean[m.cols], mStd[m.cols];
    for(unsigned i = 0 ; i < m.cols ;i++)
    {
        Mat mVal;
        transpose(m(Range::all(),Range(i,i+1)),mVal);
        meanStdDev(mVal,mMean[i],mStd[i]);
    }

    // Compute SumXY.
    double mSumXY[m.cols][m.cols];
    for(unsigned i = 0 ; i < m.cols ; i++)
    {
        for(unsigned j = i ; j < m.cols; j++)
        {
            double &sXY = mSumXY[i][j];
            sXY =0;
            for(unsigned k = 0 ; k < m.rows; k++)
                sXY += m.at<float>(k,i) * m.at<float>(k,j);
            mSumXY[j][i] = sXY;
        }
    }

    mCorrelact = Mat(m.cols,m.cols, CV_64FC1);

    // Compute Person Correlation Coef.
    for(unsigned i = 0 ; i < m.cols ; i++)
    {
        for(unsigned j = i ; j < m.cols; j++)
        {
            // Person correlation r (https://en.wikipedia.org/wiki/Pearson_correlation_coefficient)
            // r(x,y) = ( sum(xi*yi) - n*mean(x)*mean(y) ) / ( (n-1)*sx*sy )
            double &rij = mCorrelact.at<double>(i,j);

            rij = (mSumXY[i][j] - m.rows* mMean[i].val[0] * mMean[j].val[0]) /
                  ( (m.rows-1)*mStd[i].val[0] *mStd[j].val[0] ) ;

            mCorrelact.at<double>(j,i) = rij;
        }
    }
}
