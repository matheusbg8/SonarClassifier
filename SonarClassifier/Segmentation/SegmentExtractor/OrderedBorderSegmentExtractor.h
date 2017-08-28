#ifndef ORDEREDBORDERSEGMENTEXTRACTOR_H
#define ORDEREDBORDERSEGMENTEXTRACTOR_H

#include "SegmentExtractor.h"

class OrderedBorderSegmentExtractor : public SegmentExtractor
{
protected:
    unsigned maxSampleSize,
             searchThreshold;

public:
    OrderedBorderSegmentExtractor();

    // SegmentExtractor interface
public:
    void createSegment(Segment *seg, Mat img16bits, unsigned row, unsigned col);    
    void load(ConfigLoader &config);
    void setThreshold(unsigned threshold);


};

#endif // ORDEREDBORDERSEGMENTEXTRACTOR_H
