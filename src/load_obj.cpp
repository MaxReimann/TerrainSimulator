#include "simulation.h"
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>


//Makro um eine Hoenkarte in ein eindimensionales Array zu packen
#define ALT(x,y) (map[(x)*ysize+(y)])

using namespace std;

bool AltitudeMap::Load_obj(char path[]){
	
	Progressbar(); //AltitudeMap::Progressbar aufrufen

	fstream  file;
	file.open(path); // datei oeffnen
	//double *copy =  new double [xsize * ysize];
	vector<double> heightvec(0); //vector mit der groesse erstellen, da anzahl der verts noch nicht bekannt 

	fstream output; // nur fuer test
	//output.open("obj_test.txt");
	
	if (!(file)) { // auf ios::failbit pruefen
		return false;
	}
	// zeilenweise einlesen
	string buffer; 
	int ver_zeile = 0;
	double vert_loc [3];
	char leer [] = " ";
	while (getline(file,buffer) ) { //solange EOF bit == dateiende nicht gesezt ist
		if (file.fail() )  { break ; }
		//# rotate X90: (x,-z,y) //nur z wert interresiert
		while(buffer[0] == ' ') //wenn ein leerzeichen das erste zeichen, die '' zeichen sind SEEEHR wichtig, da bei "" ein const char* gemacht wird statt einem char, buffer[0] gibt char zurueck
		{
			buffer.erase(0,1); // wird es geloescht
		}
		if (buffer[0] == 'v' && buffer[1] == ' ') { //zeile gibt einen vertice an
			int pos = 2; // position im text ist erster wert mit zahlen

			vert_loc[0] =  split_text_doubles(buffer,pos); //pos wird in der funktion erhoeht
			vert_loc[1] =  split_text_doubles(buffer,pos);
			//vert_loc[2] =  split_text_doubles(buffer,pos);//eigentlich nicht noetig

			ver_zeile++;
			heightvec.push_back(vert_loc[1]);
			
			//kantenlaengen angibt, -vert_loc[1] = z
			//output << vert_loc[0] << vert_loc[1] << vert_loc[2] << endl;
		}
		 

	}
	dialog->Update(80); // die endzahl ist 100

	int kanten_len  = static_cast<int>(sqrt((double)heightvec.size()));//.size() gibt anzahl der elemente, nimmt exakt gleich lange kanten, da .obj keine
	delete [] map; //alte karte leoschen
	map = new double [kanten_len * kanten_len];
	xsize  = kanten_len;
	ysize = kanten_len;

	pair<double,double> range = find_range(heightvec);
	double range_min = range.first;
	double range_max = range.second;
	double fac =  abs(range_max) - abs(range_min); //der intervall zwischen min und max wert
	double plusy = 0;
	if (range_min > 0) { // -z ist nach oben ,daher ist +z kleiner
		plusy = range_min;
	}



	for (int x = 0; x  < kanten_len;++x){ 
		for (int y = 0; y  < kanten_len ;++y){
			//der wert an der stelle aus heightvec genommen, + range_min um negative werte zu vermeiden
			//   / fac (intervall)  um die werte auf das intervall [0,1] zu normalisieren
			// 1.0 - den wert: um die im obj format ueblichen -z darstellung wieder umzudrehen
			double wert =  1.0 - ((heightvec[x* kanten_len+y] + range_min) / fac); 
			ALT(x,y) = wert;//nach dem muster von map, jedoch -
		}
	}
	dialog->Update(80);

	for (int x = 0; x < xsize;++x) {
		for (int y = 0; y < ysize;++y) {
			if (ALT(x,y) < 0.0) {
				ALT(x,y) = 0.0;
			}
			else if (ALT(x,y) > 1.0) {
				ALT(x,y) = 1.0;
			}
		}
	}
	dialog->Update(90);
	file.close();

	dialog->Destroy();
	return true;
}

double inline AltitudeMap::split_text_doubles(string str,int& pos){
	double output;
	char buf [15];
	if (str[pos] == ' ') {pos++;} //
	int count = 0;
	while (pos <= (signed) str.length())
	{
		bool b = str[pos] == '0' || str[pos] == '1' || str[pos] == '2' || str[pos] == '3' || str[pos] == '4' || str[pos] == '5' || str[pos] == '6' || str[pos] == '7' || str[pos] == '8' || str[pos] == '9' || str[pos] == '.' || str[pos] == '-';
		if (b){ //wenn eine zahl oder .- zeichen
			buf[count] = str[pos];
		}
		else {break;} //zahl zuende
		pos++;
		count++;
	}
	output = atof(buf); //konvertiert string -> c_str -> double
	return output;
}

inline pair<double,double> AltitudeMap::find_range(vector<double> heightvec){
	double smallest = 10 ; //hier "umgekerhte" werte benutzen
	double biggest = -10;
	for (vector<double>::iterator myiter = heightvec.begin();
		myiter != heightvec.end(); ++myiter ) {
			if (*myiter > biggest) 
				biggest = *myiter;
			else if (*myiter < smallest)
				smallest = *myiter;
	}
	pair<double,double> range = pair<double, double>(smallest,biggest);
	return range;
}

bool  AltitudeMap::save_obj(char path[]){
		
	Progressbar(); //AltitudeMap::Progressbar aufrufen
	
	fstream  file;
	file.open(path,ios::out | ios::trunc); // datei oeffnen
	if (!(file)) { // auf ios::failbit pruefen
		return false;
	}
	dialog->Update(30);
	
	file << " ### Max Reimanns terrain generator .obj file #### " << endl;
	//string buffer; 
	for (int x = xsize; x > 0;--x){
		for (int y = ysize; y > 0;--y) {

		file << "v " << -static_cast<double>(x) / 100.0 << " " << -static_cast<double>(ALT(x,y)) <<  " " << static_cast<double>(y) / 100.0<< endl; // obj ist in der form x,-z,y , vertices werden ab 1 gezaehlt
		}
	}
	dialog->Update(60);
	//file << buffer;
	for (int x = 1; x < ( xsize * ysize - ysize) ;++x){
		if ((x % xsize) != 0){ //die kante nicht als face machen
		file << "f " << x  << " " <<  x + xsize << " " << x + xsize + 1 << " " <<  x+1 << endl; //x, x+xsize, x+size+1,x+1 formel fuer faces
		}
	}
	dialog->Update(99);
	file.close();
	
	dialog->Destroy();


	return true;
}




