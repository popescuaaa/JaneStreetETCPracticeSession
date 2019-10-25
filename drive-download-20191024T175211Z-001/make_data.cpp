#include<iostream>
#include<fstream>
#include<string>

using namespace std;

int main(int argc, char** argv){
	cout<<"Creating "<< 2425 <<" samples now..."<<endl;
	ifstream fin;
	ofstream file;
	file.open(argv[1]);
	fin.open(argv[2]);
	for(auto i=0; i < 2425; i++){
		double value;
		fin >> value;
		file << i << ","<< value << endl;
	}
	file.close();
	fin.close();
	cout<<"Done creating samples!"<<endl;
	return 0;
}
