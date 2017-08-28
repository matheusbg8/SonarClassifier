#include "StatisticResults.h"
#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;

StatisticResults::StatisticResults():
    gTh(0.f),rTh(0.f),tp(0),tn(0),fp(0),fn(0),total(0),
    ftp(0.f),ftn(0.f),ffp(0.f),ffn(0.f),
    acy(0.f),sen(0.f),spe(0.f),effc(0.f),ppv(0.f),npv(0.f),mCoef(0.f)
{

}

void StatisticResults::computeStatistics(unsigned tp, unsigned tn, unsigned fp, unsigned fn)
{
    reset();
    this->tp = tp; this->tn = tn;
    this->fp = fp; this->fn = fn;
    total = total + tp + tn + fp + fn; // The total sum at beginner (always 0) force a cast to ull integer

    // Normalization
    ftp = tp/(float)total;
    ftn = tn/(float)total;
    ffp = fp/(float)total;
    ffn = fn/(float)total;

    acy = (ftp+ftn);//  / 1.f (total = 1.f);  // Accuracy (ACC)
    sen = ftp   / (ftp+ffn); // Sensitivity, recall, hit rate or tru positive rate (TPR)
    spe = ftn   / (ftn + ffp); // Specificity or True Negative Rate (TNR)
    effc = (sen+spe)/2.f; // Efficiency
    ppv = ftp / (ftp+ffp); // Positive Prediction Value (PPV)
    npv = ftn / (ftn+ffn); // Negative Prediction Value
    mCoef = (ftp*ftn - ffp*ffn)/sqrt((ftp+ffp)*(ftp+ffn)*(ftn+ffp)*(ftn+ffn)); // Matthews Correlation Coefficient (MCC)
}

void StatisticResults::computeStatistics()
{
    computeStatistics(tp,tn,fp,fn);
}

void StatisticResults::reset()
{
    tp = tn = fp = fn = 0;
    ftp = ftn = ffp = ffn =
    acy = sen = spe = effc = ppv = npv = mCoef = 0.f;
    total =0u;
}

void StatisticResults::print()
{
    cout << endl
         << setprecision(5) << fixed << endl
         << "True Positive = " << tp << " (" << ftp*100.f  << "%)" << endl
         << "True Negative = " << tn << " (" << ftn*100.f  << "%)" << endl
         << "False Positive = " << fp << " (" << ffp*100.f  << "%)" << endl
         << "False Negative = " << fn << " (" << ffn*100.f  << "%)" << endl
         << "-----------------------------" << endl
         << "Accuracy = " << acy*100.f << "%" << endl
         << "Sensitivity = " << sen*100.f << "%" << endl
         << "Specificity = " << spe*100.f << "%" << endl
         << "Efficiency = " << effc*100.f << "%" << endl
         << "Positive Predition = " << ppv*100.f << "%" << endl
         << "Negative Predition = " << npv*100.f << "%" << endl
         << "Matthews Coef (phi)= " << mCoef << endl
         << "-----------------------------" << endl
         << "Result Threshold = " << rTh << "%" << endl
         << "Ground Truth Threshold = " << gTh << "%" << endl;
}
