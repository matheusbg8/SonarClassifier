#ifndef STATISTICRESULTS_H
#define STATISTICRESULTS_H

class StatisticResults
{
public:
    float gTh, rTh;

    unsigned tp, tn, fp, fn;
    unsigned long long int total;
    float ftp, ftn, ffp, ffn,
    acy,sen,spe,effc,ppv,npv,mCoef;

    StatisticResults();

    void computeStatistics(unsigned tp,
                           unsigned tn,
                           unsigned fp,
                           unsigned fn);
    void computeStatistics();

    void reset();

    void print();
};

#endif // STATISTICRESULTS_H
