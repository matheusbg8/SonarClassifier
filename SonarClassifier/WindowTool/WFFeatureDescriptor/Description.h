#ifndef DESCRIPTION_H
#define DESCRIPTION_H

#include <vector>
using namespace std;

class Description
{
public:
    int label, objectId;
    vector<double> data;
    Description(){ label = objectId = -1; }
};

class DescriptionFrame
{
public:
    DescriptionFrame();

    vector<Description> vms;
    int frameId,info;

    void allocateObjecId(unsigned objId);

    void setData(int objId, vector<double> &data);
    void setLabel(int objId, int label);

    bool hasVm(unsigned vmId);

    bool hasData(unsigned objId);
    bool hasLabel(unsigned objId);

    Description & getVm(unsigned objId);

    void clear();

};

#endif // DESCRIPTION_H
