#ifndef SONARODOMETRY_H
#define SONARODOMETRY_H

#include "Sonar.h"

class SonarOdometry
{
    Sonar s;
public:
    SonarOdometry();

    void processImgs(const char *imgsFileName);
};

#endif // SONARODOMETRY_H
