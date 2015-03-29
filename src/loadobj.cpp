#include "simulation.h"
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>

//2d -> 1d indexing
#define ALT(x,y) (map[(x)*ysize+(y)])

using namespace std;

bool AltitudeMap::loadOBJ(char path[]){

	showProgressbar(); 

	fstream  file;
	file.open(path);
	vector<double> heightVec(0); 
	fstream output; 

	if (!(file)) { // auf ios::failbit pruefen
		return false;
	}

	string buffer;
	int vertRow = 0;
	double vertLoc[3];
	while (getline(file, buffer)) { 
		if (file.fail())  { break; }
		while (buffer[0] == ' ') //erase first token while its a space
		{
			buffer.erase(0, 1); 
		}
		if (buffer[0] == 'v' && buffer[1] == ' ') { 
			int pos = 2; // first position with numbers

			vertLoc[0] = parseValues(buffer, pos);
			vertLoc[1] = parseValues(buffer, pos);

			vertRow++;
			heightVec.push_back(vertLoc[1]);
		}


	}

	dialog->Update(50);
	int edgeLength = static_cast<int>(sqrt((double)heightVec.size()));
	
	delete[] map; 
	map = new double[edgeLength * edgeLength];
	xsize = edgeLength;
	ysize = edgeLength;

	pair<double, double> range = findRange(heightVec);
	double rangeMin = range.first;
	double rangeMax = range.second;
	double plusY = 0;
	if (rangeMin > 0) { // -z is pointing up 
		plusY = rangeMin;
	}


	for (int x = 0; x < xsize; ++x){
		for (int y = 0; y < ysize; ++y){
			// shift height to above 0, scale in interval [0,1] and inverse the .obj specific -z 
			ALT(x, y) = 1.0 - ((heightVec[x* edgeLength + y] + rangeMin) / (abs(rangeMax) - abs(rangeMin)));

			if (ALT(x, y) < 0.0)
				ALT(x, y) = 0.0;
			else if (ALT(x, y) > 1.0)
				ALT(x, y) = 1.0;
		}
	}

	dialog->Update(90);
	file.close();

	dialog->Destroy();
	return true;
}

double inline AltitudeMap::parseValues(string str, int& pos){
	char buf[15];
	int count = 0;
	if (str[pos] == ' ') 
		pos++; 

	while (pos <= (signed)str.length())
	{
		std::vector<char> v{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',  '.',  '-'};
		auto findPos = std::find(std::begin(v), std::end(v), str[pos]);

		if (findPos != std::end(v))
			buf[count] = str[pos];
		else 
			break; 

		pos++;
		count++;
	}

	return atof(buf);
}

inline pair<double, double> AltitudeMap::findRange(vector<double> heightvec){
	double smallest = 10; 
	double biggest = -10;

	for (auto it = heightvec.begin(); it != heightvec.end(); ++it) {
		if (*it > biggest)
			biggest = *it;
		else if (*it < smallest)
			smallest = *it;
	}

	return pair<double, double>(smallest, biggest);
}

bool  AltitudeMap::saveOBJ(char path[]){
	showProgressbar();
	fstream  fileStream;
	fileStream.open(path, ios::out | ios::trunc);
	if (!fileStream) 
		return false;

	dialog->Update(30);
	fileStream << " ### Max Reimanns terrain generator .obj file #### " << endl;

	for (int x = xsize; x > 0; --x){
		for (int y = ysize; y > 0; --y) {
			// obj is in form x,-z,y , vertices are counted starting with 1
			fileStream << "v " << -static_cast<double>(x) / 100.0 << " " 
				<< -static_cast<double>(ALT(x, y)) << " " 
				<< static_cast<double>(y) / 100.0 << endl; 
		}
		dialog->Update(30 + (xsize-x)/(xsize)*30);
	}

	for (int x = 1; x < (xsize * ysize - ysize); ++x){
		if ((x % xsize) != 0){ //dont generate face from outer edge
			fileStream << "f " << x << " " << x + xsize << " " << x + xsize + 1 << " " << x + 1 << endl;
		}
	}

	dialog->Update(99);
	fileStream.close();
	dialog->Destroy();

	return true;
}




