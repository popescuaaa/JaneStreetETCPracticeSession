//simple lr
#include <iostream>
#include <mlpack/core.hpp>
#include <vector>
#include <math.h>

using namespace std;
using namespace mlpack;

void regression(double& b0, double&b1, arma::mat& matrix);

int main(int argc, char** argv){
	double b0, b1;
	arma::mat matrix;
	data::Load(argv[1], matrix);
	regression(b0,b1,matrix);
	cout<<"f(x)="<<b0<<"+"<<b1<<"x"<<endl;

	double sum = 1000;
	ifstream fin;
	fin.open(argv[2]);
	vector<double> values;

	for (int i = 0; i < 2425; i++) {
		double curren_value;
		fin >> curren_value;
		values.push_back(curren_value);
	}

	bool has_stock = false;
	double stock;
	for (int i = 1000; i < 2425; i++) {
		double expected_value = b0*i + b1;
		if (values[i] < expected_value && !has_stock && (sum - values[i]) > 0) {
			sum -= values[i];
			has_stock = true;
			stock = values[i];
			cout << sum << " " <<values[i] <<  endl;
		} else if (stock > values[i] && has_stock) {
			sum += values[i];
			has_stock = false;
			cout << sum << " " <<values[i] <<  endl;
		}
	}

	cout << "The profit is: " << sum - 1000 << endl;

	return 0;
}

void regression(double& b0, double& b1, arma::mat& matrix){
	double num=0, den=0, y=0, x=0;
	for(unsigned i=0; i<1000; i++){
		y+=matrix(1,i);
		x+=matrix(0,i);
	}
	y/=1000;
	x/=1000;
	for(unsigned i=0; i<1000; i++){
		num+=(y-matrix(1,i))*matrix(0,i);
		den+=(x-matrix(0,i))*matrix(0,i);
	}
	b1=num/den;
	b0=y-b1*x;
}
