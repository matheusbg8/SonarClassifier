#include "DatasetConversion.h"
#include "CSVReader/CSVReader2.h"

#include <iostream>
#include <cmath>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "Drawing.h"

using namespace std;
using namespace cv;

DatasetConversion::DatasetConversion()
{

}

void DatasetConversion::KINECT_csvQuaternios2Euler()
{
    CSVReader2 csv;
    FILE *f = fopen("/media/matheusbg/Dados/Dataset/Kinect/rgbd_dataset_freiburg2_pioneer_slam/groundtruth_euler.txt","w");
    if(f == 0x0) return;

    fprintf(f,"#ground,truth,trajectory\n"
              "#file:rgbd_dataset_freiburg2_pioneer_slam.bag\n"
              "#timestamp,tx,ty,tz,roll,pitch,yaw\n");

    csv.open("/media/matheusbg/Dados/Dataset/Kinect/rgbd_dataset_freiburg2_pioneer_slam/groundtruth.txt",8,',',
             CSV_DOUBLE,CSV_DOUBLE,CSV_DOUBLE,CSV_DOUBLE,CSV_DOUBLE,CSV_DOUBLE,CSV_DOUBLE,CSV_DOUBLE);

    vector<vector<double> > data;

    csv.ignoreLine();  csv.ignoreLine(); csv.ignoreLine();


    double timeStamp, tx,ty,tz, qx,qy,qz,qw,
           roll,pitch,yaw;

    // Drawing control
    double mTx=99999, mTy=99999,
           MTx=0, MTy=0,
           ex,ey;
    unsigned w=800,h=600;
    // End drawing control

    cout << "Starting KINECT_csvQuaternios2Euler..." << endl;

    while(csv.read(&timeStamp,&tx,&ty,&tz,
                   &qx,&qy,&qz,&qw))
    {
        double ysqr = qy * qy;

        // roll (x-axis rotation)
        double t0 = +2.0 * (qw * qx + qy * qz);
        double t1 = +1.0 - 2.0 * (qx * qx + ysqr);
        roll = std::atan2(t0, t1);

        // pitch (y-axis rotation)
        double t2 = +2.0 * (qw * qy - qz * qx);
        t2 = t2 > 1.0 ? 1.0 : t2;
        t2 = t2 < -1.0 ? -1.0 : t2;
        pitch = std::asin(t2);

        // yaw (z-axis rotation)
        double t3 = +2.0 * (qw * qz + qx * qy);
        double t4 = +1.0 - 2.0 * (ysqr + qz * qz);
        yaw = std::atan2(t3, t4);

        // Save data to validation
        vector<double> line(3);
        line[0] = tx; line[1] = ty; line[2] = yaw;
        MTx = max(MTx,tx); MTy = max(MTy,ty);
        mTx = min(mTx,tx); mTy = min(mTy,ty);

        data.push_back(line);

        printf("%lf\n",timeStamp);

        fprintf(f,"%.4lf,%.4lf,%.4lf,%.4lf,%.4lf,%.4lf,%.4lf\n",
                timeStamp,tx,ty,tz,
                roll,pitch,yaw);
    }

    ex = w/(MTx-mTx); ey = h/(MTy-mTy);
    Mat path(h,w,CV_8UC3,Scalar(255,255,255));
    cout << ex << " , " << ey << endl
         << "Mx " << MTx << " mx " << mTx
         << "My " << MTy << " my " << mTy << endl;
    char c;
    for(unsigned i = 1 ; i < data.size() & c != 27 ; i ++)
    {
        Point2f ini((data[i-1][0] - mTx) * ex ,(data[i-1][1] - mTy) * ey),
                fim( (data[i][0] - mTx) * ex , (data[i][1] - mTy) * ey);
//        cout << "dataX " << data[i][0] << " dataY " << data[i][1]
//             << " sX " << fim.x << " sY " << fim.y << endl;

        line(path,
             ini,
             fim,
             Scalar(255,0,0));

        Mat screen = path.clone();

        Drawing::drawArrow(screen,
                           fim,
                           360 + data[i][2]*180/M_PI,
                           40,
                           Scalar(0,0,255));

        imshow("Data Viewer",screen);

//        c = waitKey(1);
//        cout << (int) c << ": " << 360 + data[i][2]*180/M_PI << endl;
    }
    waitKey();

    fclose(f);
}

