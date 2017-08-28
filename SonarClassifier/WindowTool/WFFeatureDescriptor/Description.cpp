#include "Description.h"

DescriptionFrame::DescriptionFrame()
{

}

void DescriptionFrame::allocateObjecId(unsigned objId)
{
    if(objId >= vms.size())
        vms.resize((objId+1)*2);
}

void DescriptionFrame::setData(int objId, vector<double> &data)
{
    allocateObjecId(objId);

    Description &vm = vms[objId];
    vm.objectId = objId;
    vm.data = data;
}

void DescriptionFrame::setLabel(int objId, int label)
{
    allocateObjecId(objId);

    Description &vm = vms[objId];
    vm.objectId = objId;
    vm.label = label;
}

bool DescriptionFrame::hasVm(unsigned vmId)
{
    if(vmId < vms.size())
        return true;
    return false;
}

bool DescriptionFrame::hasData(unsigned objId)
{
    if(objId < vms.size() && vms[objId].data.size() > 0)
        return true;
    return false;
}

bool DescriptionFrame::hasLabel(unsigned objId)
{
    if(objId < vms.size() && vms[objId].label != -1)
        return true;
    return false;
}

Description &DescriptionFrame::getVm(unsigned objId)
{
    return vms[objId];
}

void DescriptionFrame::clear()
{
    vms.clear();
}
