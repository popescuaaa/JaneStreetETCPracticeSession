#include<iostream>
#include<mlpack/core.hpp>
#include<mlpack/methods/linear_regression/linear_regression.hpp>

using namespace std;
using namespace mlpack;
using namespace arma;

int main(int argc, char** argv){
    if(argc<3) throw runtime_error("error : less no. of args passed");

    mat predictors; rowvec responses;

    //load data
    data::Load(argv[1], predictors);
    data::Load(argv[2], responses);

    regression::LinearRegression regressor;
    regressor.Train(predictors, responses);

    colvec4 point; rowvec predictions;

    srand(10);
    point<<rand()<<rand()<<rand()<<rand();

    regressor.Predict(point, predictions);
    rowvec expected;
    expected<<4.458945*point(0)+8.5433218*point(1)-15.457454645*point(2)+0.245798*point(3)-45.2589746313;


    cout<<"expected   : "<<expected;
    cout<<"prediction : "<<predictions;

    return EXIT_SUCCESS;
}