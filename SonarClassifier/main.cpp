#include <iostream>
#include <clocale>

#include <cstdio>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "Sonar/Sonar.h"
#include "Sonar/SonarConfig/ConfigLoader.h"
#include "GroundTruth/GroundTruth.h"
#include "GraphMatcher/GraphMatcher.h"
#include "MatchViewer.h"

#include "Tools/DirOperations.h"
#include "Tools/GenericImageProcessing.h"
#include "Tools/HungarianAlgorithm.h"
#include "Tools/DatasetConversion.h"

#include "CSVMatrixGenerator.h"

using namespace std;
using namespace cv;

int main(int argc, char* argv[])
{
//    setlocale (LC_ALL,"C");
//    std::setlocale (LC_ALL,"C");
//    std::locale::global(std::locale("C"));
//     setlocale(LC_ALL, "en_US.UTF-8");
//    setlocale(LC_NUMERIC, "C");
//    setlocale(LC_ALL, "en_US.UTF-8");
//    setlocale(LC_NUMERIC, "de_DE");
//    setlocale(LC_TIME, "ja_JP");
//    setlocale(LC_ALL, "en_US");
//    wchar_t str[100];
//    time_t t = time(NULL);
//    wcsftime(str, 100, L"%A %c", localtime(&t));
//    wprintf(L"Number: %.2f\nDate: %Ls\n", 3.14, str);

//    DatasetConversion test;
//    test.KINECT_csvQuaternios2Euler();
//    return 0;

    MatchViewer mv;
    cout << "OpenCV version " << CV_VERSION << endl;

//    GenericImageProcessing gip("../../../../SonarGraphData/grayData/");
//    gip.loadFrames();
//    gip.generateAllImgDiff();

//    mv.testWindowTool();

//    mv.compareWithGroundTruth();

//    mv.compareWithGorundTruth2();

//    mv.createGroudTruth();

//    mv.standardExecution();

//    mv.matchTest();

//    mv.gaussianTest();

//    mv.generateMatlabMatrix("../../GroundTruth/Yacht_05_12_2014.txt");

    // Generate graph mathing results
//    mv.closeLoopDetection("../../../../SonarGraphData/grayData/");

//    if(argc > 1 && strcmp(argv[1], "CLD")==0) // Close Loop Detect
//    {
//        cout << "Close Loop Detect" << endl;

//        if(argc > 3)
//            mv.closeLoopDetection("../../../../SonarGraphData/grayData/",atoi(argv[2]), atoi(argv[3]));
//        else
//            mv.closeLoopDetection("../../../../SonarGraphData/grayData/");
//    }

//    if(argc > 1 && strcmp(argv[1], "CLAS")==0) // Close Loop Analisy and Save
//    {
//        cout << "Close Loop Analisy and Save" << endl;
//        mv.closeLoopAnaliseResult();
//    }

    mv.testGaussianDescriptionFeature();

//    mv.testGaussianClassificators();

//    mv.gtLoopTester();

//    mv.testSVM();

//    mv.testPaintWindow();

//    mv.testPolyMaker();

//    mv.testHuMoments();

//    mv.gtTopologicalMatch();

    // Load and evaluate deep learning results
//    mv.deepCloseLoopAnalise();
    return 0;
}
