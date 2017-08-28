#include "WFFeatureDescriptor.h"
#include "WindowTool/GaussianDescriptor/WFGaussianDescriptor.h"
#include "Drawing/Drawing.h"
#include "Description.h"
#include "Analysis/StatisticResults.h"
#include "Tools/CorrelationMatrix.h"

WFFeatureDescriptor::WFFeatureDescriptor(Classifier *classifier):
    classifier(classifier),
  traningDataPercent(0.8f),
  normalizeData(true),
  trainingDataReady(false),
  lastSelecFrameId(-1),
  lastSelectedObjectId(-1),
  showFrameInfo(true),
  trainResultWindowSize(1000,1000)
{

}

char WFFeatureDescriptor::classesName[10][200] =
{
    "0",
    "Pole",
    "wharf",
    "BoatHull",
    "Stone",
    "Fish",
    "Swimmer",
    "7","8","9"
};

Scalar WFFeatureDescriptor::getClassColor(float label)
{
//    if( (label - (int) label) != 0)
//    {
//        cout << "Asked class " << label << endl;
//    }

    if(label >= 0.5 && label < 1.5)
    {
        return Scalar(0.f,255.f,0.f);  // Green Pole
    }else if(label >= 1.5 && label < 2.5)
    {
        return Scalar(0.f,69.f,255.f); // Orange Wharf
    }else if(label >= 2.5 && label < 3.5)
    {
        return Scalar(0.f,0.f,255.f); // Red BoatHull

    }else if(label >= 3.5 && label < 4.5)
    {
        return Scalar(255.f,255.f,0.f); // Cyan Stone
    }else if(label >= 4.5 && label < 5.5)
    {
        return Scalar(0.f,255.f,255.f); // Yellow Fish
    }else if(label >= 5.5 && label < 6.5)
    {
        return Scalar(255.f,0.f,0.f); // Blue Swimmer
    }
}

void WFFeatureDescriptor::doTraining()
{
    Mat trainingLabelsMat, trainingDataMat;
    Mat validationLabelsMat, validationDataMat;

    makeTraningData(trainingLabelsMat,trainingDataMat,
                    validationLabelsMat,validationDataMat);

//    cout << "Training data:" << endl
//         << trainingDataMat << endl << endl
//         << " labels: " << endl
//         << trainingLabelsMat << endl << endl;


    cout << "MinVal - MaxVal:" << endl;
    // For each class take min and max value
    for(unsigned i = 0 ; i < minVal.size() ; i++)
    {
        cout << minVal[i] << " - " << maxVal[i] << endl;
    }

    classifier->training(trainingLabelsMat,trainingDataMat);

    showTrainResults();

    vector<pair<unsigned , unsigned> > results;

    cout << "\nHit statistics:" << endl;

    cout << "Validation " << computeHitPercentage(validationLabelsMat,validationDataMat,results)
         << endl;

    for(unsigned i = 0; i < results.size(); i++)
    {
        if(allSVMByClass[i].size()>0)
        {
            cout << classesName[i]
                 << " " << (float) results[i].first/ ((float)results[i].first +  results[i].second)
                 << "%" << endl;
        }
    }

    cout << "\nAll data "
         << computeHitPercentage(results)
         << "%" << endl;
    for(unsigned i = 0; i < results.size(); i++)
    {
        if(allSVMByClass[i].size()>0)
        {
            cout << classesName[i]
                 << " " << (float) results[i].first/ ((float)results[i].first +  results[i].second)
                 << "% of hit" << endl;
        }
    }


}

/**
 * @brief WFFeatureDescriptor::showTrainResults
 * When the features are 2-Dimensional it is possible to
 * draw a representative image of the Classifier model, where
 * the colors means the class/label correspondence and
 * the circles are the data. When the circle and background colors
 * are correspondent means correct classification.
 * If the data dimension is not 2-dimensional, nothings is made
 * by this method.
 */
void WFFeatureDescriptor::showTrainResults()
{
    if(minVal.size() > 2) return;

    Mat imResult(trainResultWindowSize.height, trainResultWindowSize.width,CV_8UC3,Scalar(0,0,0));
    int crop = 900;

    for(unsigned i = 0; i < trainResultWindowSize.height;i++)
    {
        for(unsigned j = 0 ; j < trainResultWindowSize.width ; j++)
        {
            Mat sampleMat(1,minVal.size(),CV_32FC1,Scalar(0));

            // From 0 to 1
            // Here we consider that all data is normalized from 0 to 1
            // , i.e. all classes are between 0 and 1
            sampleMat.at<float>(0,0) = (float) j/ (float) (trainResultWindowSize.width+crop);
            sampleMat.at<float>(0,1) = (float) i/ (float) (trainResultWindowSize.height+crop);

            float response = classifier->predict(sampleMat);

            Scalar color = getClassColor(response);

            // Drawing the background
            imResult.at<Vec3b>(i,j)  = Vec3b(color.val[0],color.val[1],color.val[2]);
        }
    }

    // Draw VMs
    for(unsigned frameId = 0; frameId < frames.size() ;frameId++)
    {
        DescriptionFrame &fr = frames[frameId];
        vector<Description> &vms = fr.vms;

        for(unsigned vmId = 0 ; vmId < vms.size() ; vmId++)
        {
            if(fr.hasData(vmId))
            {
                Description &vm = vms[vmId];

                // Normalized center
                Point2f center( (vm.data[0]-minVal[0])/(maxVal[0]-minVal[0]) ,
                                (vm.data[1]-minVal[1])/(maxVal[1]-minVal[1]) );

                // Center on screen
                Point2f screenCenter(center.x*(trainResultWindowSize.width+crop),
                                     center.y*(trainResultWindowSize.height+crop));

                if(fr.hasLabel(vmId))
                {
                    circle(imResult,screenCenter, 5,
                           getClassColor(vm.label),-1);
                    // Draw black circle contour
                    circle(imResult,screenCenter, 5,
                           Scalar(0.f,0.f,0.f),1);
                }else
                {
                    // Unknow label is black circle
//                    circle(imResult,screenCenter, 5,
//                           Scalar(255.f,0.f,255.f),-1);
                }
            }
        }
    }

    imshow("Train Results", imResult);
    imwrite("Training_Results.png", imResult);

}


void WFFeatureDescriptor::takeMinVal(vector<double> &min, vector<double> &data)
{
    if(data.size() > min.size())
        min.resize(data.size(), 999999.9);

    for(unsigned i = 0 ; i < data.size();i++)
        min[i] = std::min(min[i],data[i]);
}

void WFFeatureDescriptor::takeMaxVal(vector<double> &max, vector<double> &data)
{
    if(data.size() > max.size())
        max.resize(data.size(),-999999.9);
    for(unsigned i = 0 ; i < data.size();i++)
        max[i] = std::max(max[i],data[i]);
}

void WFFeatureDescriptor::makeTraningData(Mat &traningLabels, Mat &trainingData,
                                          Mat &validationLabels, Mat &validationData)
{
    vector<GaussianFrame> &gFrs = _WFGD->frames;
    unsigned vmCount=0, labeledFrames=0;
    vector<double> data;
    bool frameHasLabel=false;

    // Reset min max values used to normalize after
    minVal.clear(); maxVal.clear();
    // Update min max value of every dimension and compute the
    // amount of vector with label (only vector with label are included
    // in min max computation)

    allSVMByClass.clear();

    // Computing support vector machine vectors
    for(unsigned frameId = 0 ; frameId < frames.size(); frameId++)
    {
        vector<Gaussian> &gs = gFrs[frameId].gaussians;
        DescriptionFrame &SVMfr = frames[frameId];

        frameHasLabel=false;

        for(unsigned i = 0 ; i < gs.size(); i++)
        {
            computeVector(data,gs[i]);
            SVMfr.setData(i,data);

//            takeMaxVal(maxVal,data);
//            takeMinVal(minVal,data);

            if(SVMfr.hasLabel(i))
            {
                vmCount++;
                frameHasLabel=true;
                takeMaxVal(maxVal,data);
                takeMinVal(minVal,data);

                Description &svnObj = SVMfr.getVm(i);

                if(svnObj.label >= allSVMByClass.size())
                {
                    allSVMByClass.resize((svnObj.label+1)*2);
                }
                allSVMByClass[svnObj.label].push_back( svnObj );
            }
        }

        if(frameHasLabel)
            labeledFrames++;
    }

    cout << "Training information:" << endl
         << "* Vector dimension = " << minVal.size() << endl
         << "* Number of labeled VMs = " << vmCount << endl
         << "* Number of labeled frames = " << labeledFrames << endl;

    vector<int> trainingClassSize(allSVMByClass.size(),0);
    int trainingSize = 0,
        validationSize = 0;

    // Counting data
    for(unsigned i = 0 ; i < allSVMByClass.size(); i++)
    {
        if(allSVMByClass[i].size() == 0) continue;
        trainingSize+= trainingClassSize[i] = allSVMByClass[i].size()*traningDataPercent;
        validationSize+= allSVMByClass[i].size()-trainingClassSize[i];
    }

    traningLabels = Mat(trainingSize, 1, CV_32FC1, Scalar(0.f));
    trainingData = Mat(trainingSize, minVal.size() , CV_32FC1, Scalar(0.f));

    validationLabels = Mat(validationSize, 1, CV_32FC1, Scalar(0.f));
    validationData = Mat(validationSize, minVal.size() , CV_32FC1, Scalar(0.f));

    int trainingCount = 0, validationCount = 0;

    // Making training and validation vectors
    for(unsigned labelId = 0 ; labelId < allSVMByClass.size(); labelId++)
    {
        for(unsigned svmId = 0 ; svmId < allSVMByClass[labelId].size(); svmId++)
        {
            Description &vm = allSVMByClass[labelId][svmId];

            if(svmId < trainingClassSize[labelId])
            {
                traningLabels.at<float>(trainingCount,0) = vm.label;
                vector<double> &vmData = vm.data;
                for(unsigned k = 0 ; k < vmData.size() ; k++)
                {
                    // Normalized data
                    trainingData.at<float>(trainingCount,k) = (float) ((vmData[k] - minVal[k]) / (maxVal[k]-minVal[k]));
                }
                trainingCount++;
            }else
            {
                validationLabels.at<float>(validationCount,0) = vm.label;
                vector<double> &vmData = vm.data;
                for(unsigned k = 0 ; k < vmData.size() ; k++)
                {
                    // Normalized data
                    validationData.at<float>(validationCount,k) = (float) ((vmData[k] - minVal[k]) / (maxVal[k]-minVal[k]));
                }
                validationCount++;
            }
        }
    }

    cout << "Class count: Training | Validatio" << endl;
    for(unsigned i =0 ; i < allSVMByClass.size() ; i++)
    {
        if(allSVMByClass[i].size() > 0)
            cout << classesName[i] << " = " << allSVMByClass[i].size()
                 << " T " << (int)(allSVMByClass[i].size()*traningDataPercent)
                 << " V " << (allSVMByClass[i].size() - (int)(allSVMByClass[i].size()*traningDataPercent))
                 <<endl;
    }

    // Calculate Covariance Matrix of the sample found
    Mat correlaction;
    calcCorrelactionMatrix(trainingData,correlaction);
    cout << "Correlation Matrix: " << endl;
    for(unsigned i = 0 ; i < correlaction.rows; i++)
    {
        for(unsigned j = 0 ; j < correlaction.cols; j++)
        {
            printf("\t%.2f", correlaction.at<double>(i,j)*100);
        }
        printf("\n");
    }

    cout << "Min and Max correlation:" << endl;
    for(unsigned i = 0 ; i < correlaction.rows; i++)
    {
        float min=9999, max=0;
        int iMin, iMax;
        for(unsigned j = 0 ; j < correlaction.cols; j++)
        {
            if(i==j) continue;
            if(min > fabs(correlaction.at<double>(i,j)))
            {
                min = fabs(correlaction.at<double>(i,j));
                iMin = j;
            }
            if(max < fabs(correlaction.at<double>(i,j)))
            {
                max = fabs(correlaction.at<double>(i,j));
                iMax = j;
            }
        }
        cout << i << " max: " << iMax << " (" << max
             << ") min: " << iMin << " (" << min << ")" << endl;
    }
    cout << endl;

    trainingDataReady = true;
}

double WFFeatureDescriptor::computeHitPercentage(Mat &labels, Mat &data, vector<pair<unsigned, unsigned> > &results)
{
    if(!classifier->trained() || labels.rows == 0) return 0.0;

    unsigned hit =0,wrong=0;

    results.clear();
    results.resize(allSVMByClass.size(),pair<unsigned, unsigned>(0,0));

    Mat resultsMat;
    classifier->predict(data, resultsMat);

    for(unsigned i = 0; i < data.rows; i++)
    {
        if(std::fabs(resultsMat.at<float>(i,0) - labels.at<float>(i,0)) < 0.5f ) // Correct class?
        {
            results[(int)labels.at<float>(i,0)].first++;
            hit++;
        }else
        {
            results[(int)labels.at<float>(i,0)].second++;
            wrong++;
        }
    }
    return hit / (double) (hit+wrong);
}

double WFFeatureDescriptor::computeHitPercentage(vector<pair<unsigned, unsigned> > &results)
{
    if(!classifier->trained()) return 0.0;

    unsigned hit =0,wrong=0;
    results.clear();
    results.resize(allSVMByClass.size(),pair<unsigned, unsigned>(0,0));

    for(unsigned frameId = 0; frameId < frames.size() ;frameId++)
    {
        DescriptionFrame &fr = frames[frameId];
        vector<Description> &vms = fr.vms;

        for(unsigned vmId = 0 ; vmId < vms.size() ; vmId++)
        {
            if(fr.hasData(vmId) && fr.hasLabel(vmId))
            {
                Description &vm = vms[vmId];
                vector<double> &vmData = vm.data;
                Mat sampleMat(1, vmData.size(), CV_32FC1, Scalar(0.f));

                for(unsigned i = 0 ; i < vmData.size() ; i++)
                    sampleMat.at<float>(0,i) = (float) (vmData[i]-minVal[i])/(maxVal[i]-minVal[i]);

                float response = classifier->predict(sampleMat);

                if(std::fabs(response - vm.label) < 0.5f ) // Correct class?
                {
                    results[vm.label].first++;
                    hit++;
                }else
                {
                    results[vm.label].second++;
                    wrong++;
                }
            }
        }
    }

    return hit / (double) (hit+wrong);
}

void WFFeatureDescriptor::computeVector(vector<double> &v, Gaussian &g)
{
    v.clear();

    // Only axes
//    v.resize(2);
//    v[0] = g.dx;
//    v[1] = g.dy;

    // We trusth
//    v.resize(4);
//    v[0] = g.dx;  // Width
//    v[1] = g.dy;  // Height
//    v[2] = g.dx / g.dy; // Inertia Ratio - circularty (circle = 1 line = 0)
//    v[3] = g.N; // Inertia Ratio - circularty (circle = 1 line = 0)

    // Hu moments
//    v.resize(7);
//    v[0] = g.hu[0];
//    v[1] = g.hu[1];
//    v[2] = g.hu[2];
//    v[3] = g.hu[3];
//    v[4] = g.hu[4];
//    v[5] = g.hu[5];
//    v[6] = g.hu[6];

    // Areas
//    v.resize(2);
//    v[0] = g.area / g.convexHullArea; //Convexity
//    v[1] = g.dx / g.dy; // Inertia Ratio - circularty (circle = 1 line = 0)

    // Areas
//    v.resize(2);
//    v[0] = g.area / g.convexHullArea; //Convexity
//    v[1] = g.dx / g.dy; // Inertia Ratio - circularty (circle = 1 line = 0)

    // Everything good
    v.resize(10);
    v[0] = g.dx;  // Width
    v[1] = g.dy;  // Height
    v[2] = g.dx / g.dy; // Inertia Ratio - circularty (circle = 1 line = 0)
    v[3] = g.di; // Std Intensity
    v[4] = g.intensity; // Mean Intensity
    v[5] = g.area;  // Area
    v[6] = g.convexHullArea;  // Hull Area
    v[7] = g.area / g.convexHullArea; //Convexity
    v[8] = g.perimeter;  // Perimeter
    v[9] = g.N;  // Pixel Count

//    v.resize(2);
//    v[0] = g.intensity;
    //    v[1] = g.di;
}

void WFFeatureDescriptor::renderFrame(Mat &img, int frameId)
{
    if(frameId >= frames.size()) return;

    DescriptionFrame &fr = frames[frameId];
    vector<Description> &vms = fr.vms;
    vector<Gaussian> &gs = _WFGD->frames[frameId].gaussians;

    Point2f gtLabelPos(20.f,-10.f), // Ground Truth label position
            svmLabelPos(20.f,-25.f); // SVM label position

    unsigned minIdRange = std::min(gs.size(),vms.size());

    for(unsigned gId = 0 ; gId < minIdRange; gId++)
    {
        Description &vm = fr.getVm(gId);

        // If Vm has an label
        if(fr.hasLabel(gId))
        {
            Drawing::drawGaussianText(img,gs[gId],Scalar(0.f,255.f,255.f),
                                      classesName[vm.label],gtLabelPos);
        }

        if(classifier->trained() && fr.hasData(gId))
        {
            vector<double> &vmData = vm.data;
            Mat sampleMat(1, vmData.size(), CV_32FC1, Scalar(0.f));

            for(unsigned i = 0 ; i < vmData.size() ; i++)
                sampleMat.at<float>(0,i) = (float) (vmData[i]-minVal[i])/(maxVal[i]-minVal[i]);

            float response = classifier->predict(sampleMat);

            Scalar color;
            if(!fr.hasLabel(gId))
            {
                color = Scalar(255.f,0.f,255.f); // Magenta (unknow)
            }
            else if(std::fabs(response - vm.label) < 0.5f ) // Correct class?
            {
                color = Scalar(0.f,255.f,0.f); // Green (correct)
            }else
            {
                color = Scalar(0.f,0.f,255.f); // Red (wrong)
            }

            int clasId = (int) round(response);
            Drawing::drawGaussianText(img,gs[gId],color,
                                      classesName[clasId],svmLabelPos);
        }
    }
}

void WFFeatureDescriptor::_renderFrameTogether(Mat &screen, const Scalar_<float> &e, unsigned frameId)
{
    if(showFrameInfo)
    {
        Point2f txtPosition(e.val[2] + 10.f,e.val[3] + screen.rows-10);
        Scalar txtColor(255.f,255.f,255.f);
        switch(frames[frameId].info)
        {
            case -1:// Unknow
                putText(screen,"Unknow Frame",
                       txtPosition,
                       FONT_HERSHEY_COMPLEX,0.5,
                       txtColor,2);
            break;
            case 0: // To use
                putText(screen,"To cross-validation",
                       txtPosition,
                       FONT_HERSHEY_COMPLEX,0.5,
                       txtColor,2);
            break;
            case 1: // To train
                putText(screen,"To train frame",
                       txtPosition,
                       FONT_HERSHEY_COMPLEX,0.5,
                       txtColor,2);
            break;
            case 2: // No used
                putText(screen,"No used frame",
                       txtPosition,
                       FONT_HERSHEY_COMPLEX,0.5,
                       txtColor,2);
            break;
        }
    }
}

void WFFeatureDescriptor::alocateFrame(unsigned frameId)
{
    if(frameId >= frames.size())
        frames.resize((frameId+1)*2);

    frames[frameId].frameId = frameId;
}

void WFFeatureDescriptor::showSVMFeatures(unsigned frameId, unsigned svmId)
{
    if(!trainingDataReady)
    {
        cout << "WFFeatureDescriptor::showSVMFeatures - Warnning traning data not ready yet!!" << endl;
        return;
    }

    vector<double> &data = frames[frameId].vms[svmId].data;
    vector<double> normData(data.size());
    vector<string> labels(10);
    labels[0] = "Widht";
    labels[1] = "Height";
    labels[2] = "Inertia Ratio";
    labels[3] = "Std Intensity";
    labels[4] = "Mean Intensity";
    labels[5] = "Area";
    labels[6] = "Hull Area";
    labels[7] = "Convexity";
    labels[8] = "Perimeter";
    labels[9] = "Pixel Count";

    for(unsigned i= 0 ; i < data.size();i++)
    {
        normData[i] = (data[i] - minVal[i]) / (maxVal[i]-minVal[i]);
    }

    cout << "Size " << data.size() << endl;
    Mat rImg;
    Drawing::polarPlot(rImg, normData, Size(600,600),labels);
    imshow("SVM Feature View", rImg);
    imwrite("SVMFeature.png", rImg);
}

void WFFeatureDescriptor::start()
{

}

/**
 * @brief WFFeatureDescriptor::selectedFrame - save last selected frame.
 *
 * @param frameId - Currente selected frame ID.
 */
void WFFeatureDescriptor::selectedFrame(int frameId)
{
    lastSelecFrameId = frameId;
}

/**
 * @brief WFFeatureDescriptor::keyPress - Behaviors:
 * Numbers from 1 to 9 , define a manual label to the selected feature (gaussian).
 * key '[' - Clear all labels of the frame.
 * key ';' - Save all data in csv files.
 * key 't' - Start training.
 * key 'y' - Start auto training.
 * key 's' - Search similar feature using euclidian distance.
 * @todo - Add a key to clear the label of a selected gaussian.
 * @param c
*/
void WFFeatureDescriptor::keyPress(char c)
{
    int num = c- '0';

    if(num > 0 && num < 10 && lastSelectedObjectId >= 0)
    {
        cout << "Gaussian " << lastSelectedObjectId << " from frame " << lastSelecFrameId
             << " is class " << classesName[num] << endl;
        frames[lastSelecFrameId].setLabel(lastSelectedObjectId,num);
    }

    switch(c)
    {
        case '[':
            if(lastSelecFrameId>=0)
                frames[lastSelecFrameId].clear();
        break;
        case ';':
            autoSave();
        break;
        case 'h':
            if(lastSelecFrameId>=0)
                frames[lastSelecFrameId].info = 0;
        break;
        case 'j':
            if(lastSelecFrameId>=0)
                frames[lastSelecFrameId].info = 1;
        break;
        case 'k':
            if(lastSelecFrameId>=0)
                frames[lastSelecFrameId].info = 2;
        break;
        case 't':
            doTraining();
        break;
        case 's':
            if(lastSelecFrameId>=0)
                searchSimilar();
        break;
    }
}

void WFFeatureDescriptor::mouseEvent(int event, int x, int y)
{

}

void WFFeatureDescriptor::renderProcess(Mat &leftImg, int leftId, Mat &rightImg, int rightId)
{
    renderFrame(leftImg,leftId);
    renderFrame(rightImg,rightId);
}

void WFFeatureDescriptor::renderFrameTogether(Mat &screen, const Scalar_<float> &el, unsigned leftFrameId, const Scalar_<float> &er, unsigned rightFrameId)
{
    _renderFrameTogether(screen,el,leftFrameId);
    _renderFrameTogether(screen,er,rightFrameId);
}

void WFFeatureDescriptor::processImages(Mat &leftImg, int leftId, Mat &rightImg, int rightId)
{
    alocateFrame(leftId);
    alocateFrame(rightId);
}

double WFFeatureDescriptor::euclidianDist(vector<double> v1, vector<double> v2)
{
    if(v1.size() != v2.size())
        return 0.0;

    double sum=0.0, diff=0.0;
    unsigned nv = v1.size();

    for(unsigned i =0 ; i < nv ; i++)
    {
        diff = v1[i] - v2[i];
        sum += diff*diff;
    }
    return sqrt(sum);
}

void WFFeatureDescriptor::searchSimilar()
{
    // Take frames and gaussians information
    int leftFrame = wt->getLeftFrameId(),
        righFrame = wt->getRighFrameId(),
        leftGaussian = _WFGD->getLeftSelectedGaussian(),
        righGaussian = _WFGD->getRighSelectedGaussian();

    if(lastSelectedObjectId == leftGaussian && lastSelecFrameId == leftFrame)
    {
        DescriptionFrame &leftFr = frames[leftFrame];
        if(!leftFr.hasVm(leftGaussian))
            return ;

        Description &srcVm = leftFr.getVm(leftGaussian);

        int minId =-1;
        double minScore = 99999.99, score;


        DescriptionFrame &rightFr = frames[righFrame];
        unsigned nVms = rightFr.vms.size();
        for(unsigned i = 0 ; i < nVms; i++)
        {
            if(rightFr.hasVm(i))
            {
                score = euclidianDist(rightFr.getVm(i).data,srcVm.data);
                if(score < minScore)
                {
                    minScore = score;
                    minId = i;
                }
                cout << "ID " << i << " = " << score << endl;
            }
        }
        cout << "Winner ID " << minId << " = " << minScore << endl;

        _WFGD->hightlightRightGaussian = minId;
    }
}

void WFFeatureDescriptor::newGaussian(int gId, int frameId)
{

}

void WFFeatureDescriptor::leftGaussianSelected(int gId, int frameId)
{
    lastSelecFrameId = frameId;
    lastSelectedObjectId = gId;

    if(gId >= 0)
        showSVMFeatures(frameId,gId);
}

void WFFeatureDescriptor::rightGaussianSeleced(int gId, int frameId)
{
    lastSelecFrameId = frameId;
    lastSelectedObjectId = gId;

    if(gId == -1)
        return ;

    Gaussian &g = _WFGD->frames[frameId].gaussians[gId];

    cout << "Hu Moments of gaussian " << gId << ":" << endl;
    for(unsigned i =0  ; i < 7 ; i++)
    {
        cout << "hu[" << i << "] = "
             << g.hu[i] << endl;
    }
}

void WFFeatureDescriptor::cleanedGaussians(int frameId)
{
    frames[frameId].clear();
}

bool WFFeatureDescriptor::save(const char *fileName)
{
    setlocale(LC_NUMERIC, "C");
    FILE *f = fopen(fileName, "w");
    if(f == 0x0)
    {
        cout << "WFFeatureDescriptor: The file " << fileName << " can not be saved. May be not permission?" << endl;
        return false;
    }

    unsigned frameId=0, vmId;

    fprintf(f,"#Gaussian descriptions file:\n");
    fprintf(f,"#First line indicate the number of frames (NFr), each frame (iFr) is individually described in a few lines\n");
    fprintf(f,"#Each frame description start with a line with 3 integers: frame ID (FrID), frame Info (FrInf) and the number of saussians (FrGs)\n");
    fprintf(f,"#The next (FrGs) lines describe each frame's gaussian with 3 integers: Description ID, Manual Label ID and Gaussian ID\n");

    // NUmber of frames
    fprintf(f,"%u\n", frames.size());
    for(frameId = 0 ; frameId < frames.size() ; frameId++)
    {
        DescriptionFrame &fr = frames[frameId];

        // VM Descriptions
        vector<Description> &vms = fr.vms;

        unsigned maxVMCount = std::min(_WFGD->frames[frameId].gaussians.size(), vms.size());

        // Frame description
        // frameID, frameInfo, NumberOfGaussians
        fprintf(f,"\n%u,%d,%u\n", frameId, fr.info, maxVMCount);
        for(vmId = 0 ; vmId < maxVMCount; vmId++)
        {
            Description &vm = vms[vmId];

            // classId, ObjectId, vectorSize
            fprintf(f,"%u,%d,%d\n",
                vmId, vm.label, vm.objectId);
        }
    }
    fclose(f);

    return true;
}

bool WFFeatureDescriptor::load(const char *fileName)
{
    setlocale(LC_NUMERIC, "C");
    FILE *f = fopen(fileName, "r");
    if(f == 0x0)
    {
        cout << "WFFeatureDescriptor: File " << fileName << " not found to load" << endl;
        return false;
    }
    // Ignore comments lines
    fscanf(f, "%*[^\n]\n", NULL);
    fscanf(f, "%*[^\n]\n", NULL);
    fscanf(f, "%*[^\n]\n", NULL);
    fscanf(f, "%*[^\n]\n", NULL);

    unsigned frameId=0, vmId=0,frameCount=0,vmCount=0;

    // Number of Frames on this GroundTrutuh
    fscanf(f,"%u", &frameCount);
    frames.resize(frameCount);
    for(frameId = 0 ; frameId < frames.size() ; frameId++)
    {
        DescriptionFrame &fr = frames[frameId];
        fr.frameId = frameId;

        // Frame description
        // FrameId, FrameInfo, vms count in this frame
        fscanf(f,"%*u,%d,%u",&fr.info,&vmCount);

        if(vmCount == 0) continue;

        // VM Description
        vector<Description> &vms = fr.vms;
        vms.resize(vmCount);
        for(vmId = 0 ; vmId < vmCount; vmId++)
        {
            Description &vm = vms[vmId];

            fscanf(f,"\n%*u,%d,%d\n",
                    &vm.label, &vm.objectId);
        }
    }
    fclose(f);
    return true;
}

bool WFFeatureDescriptor::loadGt(const char *fileName)
{
    string sufix;

    sufix = "_wt.csv";
    if(!wt->load((fileName + sufix).c_str()))
        return false;

    sufix = "_gd.csv";
    if(!_WFGD->load((fileName + sufix).c_str()))
        return false;

    sufix = "_fts.csv";
    if(!load((fileName + sufix).c_str()))
        return false;
}

bool WFFeatureDescriptor::saveGt(const char *fileName)
{
    string sufix;

    sufix = "_wt.csv";

    // Save image file names and IDs
    if(!wt->save((fileName + sufix).c_str()))
        return false;

    // Save extracted gaussians
    sufix = "_gd.csv";
    if(!_WFGD->save((fileName + sufix).c_str()))
        return false;

    // Save features
    sufix = "_fts.csv";
    if(!save((fileName + sufix).c_str()))
        return false;
}

void WFFeatureDescriptor::setWorkFile(const char *fileName)
{
    workFileName = fileName;
}

void WFFeatureDescriptor::autoSave()
{
    saveGt(workFileName.c_str());
}

bool WFFeatureDescriptor::autoLoad()
{
    if(!loadGt(workFileName.c_str()))
    {
       cout << "Loading default sonar images" << endl;
       wt->loadEmptyFrames("../../Datasets/yacht_05_12_img16bits_especifico.txt");
    }
}

