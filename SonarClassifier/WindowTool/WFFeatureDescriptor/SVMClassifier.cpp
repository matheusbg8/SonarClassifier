#include "SVMClassifier.h"

SVMClassifier::SVMClassifier(bool autoTrain):
    gamma(41.55),
    C(28.45),
    kernel_type(CvSVM::LINEAR),
    autoTrain(autoTrain),
    svmTrained(false)
{
    CvParamGrid gC = CvSVM::get_default_grid(CvSVM::C);
    CvParamGrid gGamma = CvSVM::get_default_grid(CvSVM::GAMMA);

    cout << "Default C grid parameters: max = " << gC.max_val
         << " min = " << gC.min_val
         << " step = " << gC.step
         << endl;

    cout << "Default Gammagrid parameters: max = " << gGamma.max_val
         << " min = " << gGamma.min_val
         << " step = " << gGamma.step
         << endl;
}

void SVMClassifier::training(Mat &labels,
                             Mat &data)
{
    // Set up SVM's parameters
    CvSVMParams params;
    params.svm_type    = CvSVM::C_SVC;
    params.kernel_type = kernel_type;
    params.gamma = gamma;
    params.degree = 1;
    params.C = C;
    params.term_crit   = cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6);

    if(autoTrain)
    {
//        SVM.train_auto(dataMat, labelsMat, Mat(), Mat(), params,3);
        int k = 5;
        SVM.train_auto(data, labels, Mat(), Mat(), params,k
                       /*,CvSVM::get_default_grid(CvSVM::C)*/,CvParamGrid(0.1,60.0,0.1)  // C parameters
                       /*,CvSVM::get_default_grid(CvSVM::GAMMA)*/,CvParamGrid(0.1,60.0,0.1)  // Gamma parameters
                       ,CvSVM::get_default_grid(CvSVM::P)  // PGrid
                       ,CvSVM::get_default_grid(CvSVM::NU)  // nuGrid
                       ,CvSVM::get_default_grid(CvSVM::COEF)  // coeffGrid
                       ,CvSVM::get_default_grid(CvSVM::DEGREE)  // degreeGrid
                       ,false // Balanced
                       );
//    train_auto(data, labels, Mat(), Mat(), params, int k_fold=10, CvParamGrid Cgrid=CvSVM::get_default_grid(CvSVM::C), CvParamGrid gammaGrid=CvSVM::get_default_grid(CvSVM::GAMMA), CvParamGrid pGrid=CvSVM::get_default_grid(CvSVM::P), CvParamGrid nuGrid=CvSVM::get_default_grid(CvSVM::NU), CvParamGrid coeffGrid=CvSVM::get_default_grid(CvSVM::COEF), CvParamGrid degreeGrid=CvSVM::get_default_grid(CvSVM::DEGREE), bool balanced=false)

        CvSVMParams resParams = SVM.get_params();
        C = resParams.C;
        gamma = resParams.gamma;

        cout << "AutoTrain:" << endl
             << "k = " << k << endl
             << "Gamma = " << resParams.gamma << " for POLY / RBF / SIGMOID" << endl
             << "C = " << resParams.C << " for optimization problem (C_SVC / EPS_SVR / NU_SVR)" << endl
             << "p = " << resParams.p << " for optimization problem (EPS_SVR)" << endl
             << "nu = " << resParams.nu << " for NU_SVC / ONE_CLASS / NU_SVR" << endl
             << "coef0 = " << resParams.coef0 << " for POLY / SIGMOID" << endl
             << "degree = " << resParams.degree << " for POLY" << endl;
    }else
    {
        SVM.train(data, labels, Mat(), Mat(), params);
    }

    svmTrained = true;
}

void SVMClassifier::predict(Mat &data, Mat &predictions)
{
    SVM.predict(data,predictions);
}

double SVMClassifier::predict(Mat &data)
{
    return SVM.predict(data);
}

bool SVMClassifier::trained()
{
    return svmTrained;
}

