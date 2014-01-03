//#include "simulation.h"
#include "gl_object.h"
#include "Voronoi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



Voronoi::Voronoi(int _fValue, int _blockSize,
                 int* _pointsList, int _seed, bool _negative, float _clip)
				 :fValue(_fValue),
				 blockSize(_blockSize),
				 seed(_seed),
				 negative(_negative),
				 clip(_clip)
{

		xsize = Alti.getxsize();
		ysize = Alti.getysize();
		init_random();
		f = 0.0; //progressbar progress length

		Progressbar();
		num_to_blockSize = (blockSize-xsize % blockSize)*(blockSize-ysize%blockSize); //quadrat des betrages bis zur naechsten blockSize'sten stelle
		big_xsize = xsize +(blockSize-xsize % blockSize);
		big_ysize = ysize + (blockSize-ysize % blockSize);
        pixelColorNumbers = new int [big_xsize*big_ysize+1];

		pixelColor_numberate();
		

        totalBlocks = (big_xsize / blockSize) * (big_xsize / blockSize);

		for(int i= 0; i< 9;++i){ pixelDist.push_back(0.0);} //pixelDist initalisieren
		for(int i= 0; i < fValue;++i){ shortest.push_back(0.0);}

		if(_pointsList == NULL)
		{
			//vector< pair<int,int> > pointsList
			//pointsList = 
            generatePoints(totalBlocks);
            xyPoints(pointsList);
		}
		else{
			for(int i = 0;i < sizeof(_pointsList)/sizeof(int);i+=2){
				pointsList.push_back( pair<int,int>(_pointsList[i],_pointsList[i+1])) ;
			}
		}

}

void Voronoi::main(double Inp_map[],int distType,float coefficients[],int elements){
	pvec neighbors(9);
	pair<int,int> XY;
	pair_i xy_block;
	inp_map = Inp_map;
	
	for (int pixels=0;pixels < big_xsize*big_ysize;pixels++){
            XY = getLocation(pixels);
            xy_block = getBlock(XY); 
            neighborCheck(xy_block, neighbors);
			pixelColorNumbers[pixels] = findNearestPoints(XY.first,XY.second, neighbors, distType,coefficients,elements);
			
			if (pixels % big_xsize == 0){
				f+= 100.0/(big_xsize*2.0);
				dialog->Update(int(f));
			}
	}
    draw();

}
void Voronoi::init_random(void){
	unsigned long init[4]={static_cast<long>(random(100,300)),static_cast<long>( random(100,300)),static_cast<long>(random(100,300)),static_cast<long>(random(100,300))}, length=4;
    init_by_array(init, length);
}

int inline Voronoi::random(int a, int b){
	return int(a + (genrand_real1() * (abs(a) + abs(b)))); // gibt eine zahl im intervall a,b, muss genreal, nich gen_int sein um 0,1 intervall zu bekommen
}


void Voronoi::range(int range,int arr[]){//range muss = elements von arr sein
		for (int i=0; i<range;i++){
			arr[i] = i;
		}
}

void Voronoi::pixelColor_numberate(){
		for (int i=0; i<big_xsize*big_xsize;i++){
			pixelColorNumbers[i] = i;
		}
}
        
int Voronoi::findNearestPoints(int x,int y,pvec& neighbor,int distType,float coefficients[],int elements){
	int elem = neighbor.size();
	int blockID;
	float distance;
	pixelDist.clear();
	shortest.clear();


	for (int i= 0; i < elem;i++){
        blockID = getBlockID(neighbor[i]);
		distance =  lenght(x, y, pointsList[blockID].first,pointsList[blockID].second, distType);
		pixelDist.push_back(distance);
	}
	for(int amount= 0; amount < fValue;amount++){
        minNum = min_element(pixelDist.begin(),pixelDist.end()) ; //min_element returns forward_iterator, dereferenced by *
		//idxNum = search(pixelDist.begin(),pixelDist.end(),minNum,minNum); // same here
        shortest.push_back(*minNum);
		pixelDist.erase(minNum);
	}
	float sum = 0.0;
	float coeff;
	for(size_t i = 0; i < shortest.size();++i){
		if(signed(i)<elements){
			coeff = coefficients[i];
		}
		else{ coeff = 1.0;}

		sum += shortest[i]*coeff;
	}
	return(sum);
}
            
float Voronoi::lenght(int x1, int y1, int x2,int y2,int method){
        int dx = x2 - x1;
        int dy = y2 - y1;
		float distance;
		if ( method == 1){//"Linear"
			distance = sqrt(float(dx*dx+dy*dy)); }
		else if(method == 2){//"SqLen"
			distance = (dx*dx+dy*dy);}
		else if( method == 3){//"Manhattan"
			distance = (dx+dy);}
		else if (method == 4){//"Chebyshev"
			if (dx == dy || dx > dy){
				distance = dx;}
			else
				{ distance = dy;}
		}
		else if ( method == 5){//"Quadratic"
			distance = (dx*dx+dy*dy+dx*dy);}
		else if (method == 6){//Meine Methode
			distance = cos(double(dx*dx)) + sin(double(dy*dy));
		}
        
		return(distance);
}

void Voronoi::generatePoints(int totalBlocks){
	for (int number = 0; number < totalBlocks; number++){
		if (seed == 0){
                seed = random(0, 1000000000);
		}
		init_genrand(seed);
        pointsList.push_back(make_pair(random(0, blockSize - 1), random(0, blockSize - 1)));
        seed +=1;
	}
}

pair<int,int> Voronoi::getLocation(int idNumber){
        int xloc = idNumber % big_xsize;
        int yloc = idNumber / big_ysize;
        return make_pair(xloc, yloc);
	}


pair<int,int> Voronoi::getLocation_Alti(int idNumber){//fur die richtige (kleinere) map
        int xloc = idNumber % xsize;
        int yloc = idNumber / ysize;
        return make_pair(xloc, yloc);
	}

pair<int,int> Voronoi::getBlock(pair<int,int> XY){
	XY.first /= blockSize;
	XY.second /= blockSize;
	return XY;
}

pair<int,int> Voronoi::getBlockStartXY(int ID){
        int x = (ID % (big_xsize / blockSize)) * blockSize;
        int y = (ID / (big_ysize / blockSize)) * blockSize;
        return make_pair(x, y);
}

pair<int,int> Voronoi::getBlockEndXY(int ID){
        int x = (ID % (big_xsize / blockSize)) * blockSize + blockSize;
        int y = (ID / (big_ysize / blockSize)) * blockSize + blockSize;
        return make_pair(x,y);
		}

int  Voronoi::getBlockID(pair<int,int> blocks){
        int blockID = blocks.second * (big_xsize / blockSize) + blocks.first;
        return blockID;
}

void	Voronoi::neighborCheck(pair<int,int> XYblock,pvec& neighbors){

		pair_i tmp[] = {make_pair(XYblock.first - 1, XYblock.second + 1), make_pair(XYblock.first, XYblock.second + 1), make_pair(XYblock.first + 1, XYblock.second + 1),
                       make_pair(XYblock.first - 1, XYblock.second), make_pair(XYblock.first, XYblock.second), make_pair(XYblock.first + 1, XYblock.second),
					   make_pair(XYblock.first - 1, XYblock.second - 1), make_pair(XYblock.first, XYblock.second - 1), make_pair(XYblock.first + 1, XYblock.second - 1)};
        neighbors.clear();
		for(int i=0;i<9;i++){
			neighbors.push_back(tmp[i]);
		}
		int maxX,maxY;
		pair<int,int> neig;
		pair<int,int> XY;
		for(int number = 0; number < int(neighbors.size());number++){
            XY = neighbors[number];
            maxX = (big_xsize / blockSize) - 1;
            maxY = (big_ysize / blockSize) - 1;
			if (XY.first < 0){
				XY.first = maxX;}
			if (XY.second < 0){
				XY.second = maxY;}
			if (XY.first > maxX){
				XY.first = 0;}
			if (XY.second > maxY){
				XY.second = 0;}
			
            neighbors[number] = XY;
		}

        //return (neighbors);
}

void Voronoi::xyPoints(pvec& numList){
	pair_i XY;
	//int xB, yB;
	pair_i blocks;
	for(int number = 0; number < int(numList.size()); number++){
            XY = numList[number];
			blocks = getBlockStartXY(number);
			XY.first += blocks.first;
			XY.second += blocks.second;
            numList[number] = XY;
	}
}
            
void Voronoi::showIt(char* color="color", bool pointsOn=false,bool border=false){
        //img = Image.new("RGB", (self.width, self.height))
        //draw = ImageDraw.ImageDraw(img)
		pair_i start,end;
		for(int number = 0; number < int(pointsList.size()); number++){
            start = getBlockStartXY(number);
            end = getBlockEndXY(number);
            /*color1 = random.randint(0, 255)
            color2 = random.randint(0, 255)
            color3 = random.randint(0, 255)
            colorSet = (color1, color2, color3)
            if color == "mono":
                colorSet = (color1, color1, color1)
            elif color == "none":
                colorSet = (255, 255, 255)
            if border == True:
                draw.rectangle([start, end], fill=colorSet, outline="black")
            else:
                draw.rectangle([start, end], fill=colorSet)
        if pointsOn == True:
            for x, y in self.pointsList:
                draw.point((x, y), fill=(0,0,0))
        img.show()
        img.save("test.png")*/
		}
}



template <class T, class return_type>
return_type Voronoi::maxima(T& liste,return_type type){
	return_type biggest = NULL;
	for(int i = 0; i < sizeof(liste)/sizeof(type);++i){
		biggest = max(liste[i],biggest);
	}
	return biggest;
}
/*
template<>
int Voronoi::maxima(int* liste){
	T biggest = NULL;
	for(int i = 0; i < sizeof(liste)/sizeof(liste[0]);i++){
		biggest = max(liste[i],biggest);
	}
	return biggest;
}*/

void Voronoi::draw(){
		int type=1;//nur dazu, um der template funktion den typ(also int) zu geben
        float  maxNumber = maxima(pixelColorNumbers,type) * (1.0-clip);
        float  colorSpread = 255.0 / maxNumber;
		pair<int,int> XY;
		//int x,y;
		double color;
	
	for(int pixel = 0; pixel < big_xsize*big_ysize; ++pixel){
            XY = getLocation(pixel);
            color = double(colorSpread * pixelColorNumbers[pixel] / 255.0);
			if (negative != false ){
				color = 1.0 - color;}
			if (XY.first < xsize && XY.second < ysize){
				if (inp_map == NULL)
				{
					Alti.Alt(XY.first,XY.second,color);
				}
				else 
				{
					inp_map[XY.first*ysize+XY.second] = color;
				}
			}
			if (pixel%big_xsize==0){
				f+=100.0/big_xsize/2.0;
				dialog->Update(int(f));
			}
	}
	dialog->Destroy();
}



void Voronoi::Progressbar(void){
	dialog = new wxProgressDialog(wxT("Voronoi"),
		wxT("Voronoi"),100,NULL, wxPD_AUTO_HIDE );
}


void AltitudeMap::Voronoi_Tex(int fValue, int blockSize,/* int _seed=0, */
							  bool negative, float clip,int distance,float coefficients[],int elements)
{

	Voronoi Vo = Voronoi(fValue,blockSize,NULL,0,negative,clip);
	Vo.main(NULL,distance,coefficients,elements);//sqlen ist schneller als sqrt operation
	normalize();

}

void AltitudeMap::Inp_Voronoi_Tex(int fValue, int blockSize,/* int _seed=0, */
							  bool negative, float clip,int distance,double inp_map[],float coefficients[],int elements)
{
	Voronoi Vo = Voronoi(fValue,blockSize,NULL,0,negative,clip);
	Vo.main(inp_map,distance,coefficients,elements);//sqlen ist schneller als sqrt operation
}

Voronoi::~Voronoi(void){
	delete [] pixelColorNumbers;
}
