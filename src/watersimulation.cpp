#include "watersimulation.h"

#include <valarray>
#include "globals.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define GRAVITY 9.81f

#define AREA 1.0f //Square area of pipe
//
//#define KC 0.001f //sediment capacity constant 
//#define KD 0.05f //deposition constant 
//#define KS 0.03f //dissolving constant 
//#define KE 0.02f //evaporation Constant
//#define KR 0.001f // rain sclae constant


#define VEX(x,y) (vert_array[(x)*ysize+(y)]) //liefert ein struct zurueck

using namespace std;

WaterSimulation::WaterSimulation(const struct WaterConstants *constants, int _method)
{
	ysize = Alti.getYSize();
	xsize = Alti.getXSize();
	vert_array = new WaterVertex[xsize * ysize];

	errorstream.open("error_sim_water.txt", ios::out);

	method = _method;

	KC = constants->KC;
	KD = constants->KD;
	KS = constants->KS;
	KE = constants->KE;
	KR = constants->KR;
	waterAmount = constants->Water_Amount;
	RADIUS = constants->Radius;
	PIPE_LENGTH = constants->Pipe_length_x;
	DT = constants->DT;
	iterations = constants->Iterations;
	RainOn = constants->Rain_on;
	WatersourceOn = constants->Watersource_on;

	pipeLength = 1.0;
	pipeArea = AREA;

	//init values
	for (int x = 0; x < xsize; x++)
	{
		for (int y = 0; y < ysize; y++)
		{

			VEX(x, y).z = Alti.getAltitude(x, y);
			VEX(x, y).Waterdistribution = 0.0;
			VEX(x, y).d = 0.0; // water is 0 everywhere
			VEX(x, y).d1 = 0.0;
			VEX(x, y).d2 = 0.0;

			VEX(x, y).fluxLeft = 0.0;
			VEX(x, y).fluxRight = 0.0;
			VEX(x, y).fluxTop = 0.0;
			VEX(x, y).fluxBottom = 0.0;
			VEX(x, y).Volume = 0.0;
			VEX(x, y).Vx = 0.0;
			VEX(x, y).Vy = 0.0;
			VEX(x, y).sediment_amount = 0.0;
			VEX(x, y).sediment_amount1 = 0.0;
			VEX(x, y).w = 0.0;
		}
	}


	if (method == 1)
	{
		if (constants->Watersource_on == true && constants->Rain_on == false)
		{
			waterflowDistribution(waterAmount);
		}
		else if (constants->Watersource_on == false && constants->Rain_on == true)
		{
			rainDistribution();
		}
		else if (constants->Watersource_on == true && constants->Rain_on == true)
		{
			waterflowDistribution(waterAmount);
			rainDistribution();
		}

		simLoop();
	}

	else if (method == 2)
	{
		strictSolver();
	}
	else if (method == 3)
	{
		fastSolver();
	}
	else
	{
		hydraulicErosion();
	}

	normalizeHeight();

	for (int x = 0; x < xsize; x++){
		for (int y = 0; y < ysize; y++)
		{
			Alti.writeAltitude(x, y, VEX(x, y).z);
			//Alti.sediment_map[(x)* ysize+(y)] = VEX(x,y).sediment_amount;
		}
	}
}

WaterSimulation::~WaterSimulation()
{
	delete[] vert_array;
	errorstream.close();
}

void WaterSimulation::simLoop()
{
	showProgressbar();
	double progress = 0.0;

	for (int t = 0; t < int(iterations); t++){

		progress += 100.0 / (double(iterations));
		dialog->Update((int)floor(progress));

		if (RainOn)
			rainDistribution();
		if (WatersourceOn)
			waterflowDistribution(waterAmount);

		increaseWater();
		fluxSim();
		surfaceAndVelocity();
		erosionDeposition();
		sedimentTransport();
		evaporation();

		if (frame->water_preview_on)
			visualizeWater2D();
		if (frame->sediment_preview_on)
			visualizeSediment2D();

		frame->manualRender();
	}

	dialog->Destroy();
}





void WaterSimulation::rainDistribution(void)
{
	seconds = time(NULL);
	srand(seconds);

	for (int x = 0; x < xsize; x++){
		for (int y = 0; y < ysize; y++){
			if (VEX(x, y).z > 0.0)
				VEX(x, y).Waterdistribution = 100 * KR * (rand() % 2) / double(iterations);
			else
				VEX(x, y).Waterdistribution = 0.0;
		}
	}
}


void WaterSimulation::waterflowDistribution(double Water_Amount)
{
	double temp_z = 0;
	double R = RADIUS;

	//write source of water flow on map
	int fX = Alti.FlagX;
	int fY = Alti.FlagY;
	int fX_smallest = ((fX - R / 2) > 0) ? (fX - R / 2) : 0;
	int fY_smallest = ((fY - R / 2) > 0) ? (fY - R / 2) : 0;
	int fX_biggest = ((fX + R / 2) < xsize) ? (fX + R / 2) : (xsize);
	int fY_biggest = ((fY + R / 2) < ysize) ? (fY + R / 2) : (ysize);

	for (int x = fX_smallest; x < fX_biggest; x++){
		for (int y = fY_smallest; y < fY_biggest; y++){
			VEX(x, y).Waterdistribution = Water_Amount / (double(R) + sqrt(double(R - x)*double(R - x) + double(R - y)*double(R - y)));
		}
	}
}


void WaterSimulation::increaseWater()
{
	for (int x = 0; x < xsize; x++){
		for (int y = 0; y < ysize; y++){
			VEX(x, y).d1 = VEX(x, y).d + VEX(x, y).Waterdistribution;
		}
	}
}

void WaterSimulation::fluxSim()
{
	double scalingK;
	double scale;
	double V_out;

	for (int x = 1; x < xsize - 1; x++){
		for (int y = 1; y < ysize - 1; y++){
			diffHeightL = VEX(x, y).z + VEX(x, y).d1 - VEX(x - 1, y).z - VEX(x - 1, y).d1;
			diffHeightR = VEX(x, y).z + VEX(x, y).d1 - VEX(x + 1, y).z - VEX(x + 1, y).d1;
			diffHeightT = VEX(x, y).z + VEX(x, y).d1 - VEX(x, y + 1).z - VEX(x, y + 1).d1;
			diffHeightB = VEX(x, y).z + VEX(x, y).d1 - VEX(x, y - 1).z - VEX(x, y - 1).d1;

			VEX(x, y).fluxLeft = max(0.0, VEX(x, y).fluxLeft + (pipeArea * (GRAVITY * abs(diffHeightL)) / pipeLength)); //outflow flux must be 0 minimum
			VEX(x, y).fluxRight = max(0.0, VEX(x, y).fluxRight + (pipeArea * (GRAVITY * abs(diffHeightR)) / pipeLength));
			VEX(x, y).fluxTop = max(0.0, VEX(x, y).fluxTop + (pipeArea * (GRAVITY * abs(diffHeightT)) / pipeLength));
			VEX(x, y).fluxBottom = max(0.0, VEX(x, y).fluxBottom + (pipeArea * (GRAVITY * abs(diffHeightB)) / pipeLength));

			//double test_values1[]= {VEX(x,y).d,VEX(x,y).Waterdistribution,VEX(x,y).fluxLeft,VEX(x,y).fluxRight,VEX(x,y).fluxTop,VEX(x,y).fluxBottom};
			V_out = (VEX(x, y).fluxLeft + VEX(x, y).fluxRight + VEX(x, y).fluxTop + VEX(x, y).fluxBottom);
			//double test_values[]= {VEX(x,y).d,VEX(x,y).d1,VEX(x,y).d2,VEX(x,y).Waterdistribution,VEX(x,y).fluxLeft,VEX(x,y).fluxRight,VEX(x,y).fluxTop,VEX(x,y).fluxBottom,VEX(x,y).Vx,VEX(x,y).Vy};
			/*if (VEX(x,y).d1 != 0.0 && V_out >= pow(10.0,-10)){ //division durch 0 ausschliessen

				if (V_out <= l * l * VEX(x,y).d1){
				scale = 1.0; //scaing_K wird 1
				}
				else if (V_out > l * l * VEX(x,y).d1){
				scale = (VEX(x,y).d1 * l*l)/(V_out);
				}
				}
				else{ scale = 0.0; }*/
			scale = (VEX(x, y).d1 * pipeLength*pipeLength) / (V_out);
			scalingK = min(1.0, scale);
			//Scaling_K = 1.0;

			if (x != 1){
				VEX(x, y).fluxLeft = VEX(x, y).fluxLeft * scalingK;
			}
			else {
				VEX(x, y).fluxLeft = VEX(x, y).fluxLeft * scalingK;
				VEX(x - 1, y).fluxLeft = 0.0;  //an der grenze outflow auf 0 setzen
			}

			if (x < xsize - 2){
				VEX(x, y).fluxRight = VEX(x, y).fluxRight * scalingK;
			}
			else {
				VEX(x, y).fluxRight = VEX(x, y).fluxRight * scalingK;
				VEX(x + 1, y).fluxRight = 0.0;
			}

			if (y != 1){
				VEX(x, y).fluxTop = VEX(x, y).fluxTop * scalingK;
			}
			else {
				VEX(x, y).fluxTop = VEX(x, y).fluxTop * scalingK;
				VEX(x, y - 1).fluxTop = 0.0;
			}

			if (y < ysize - 2){
				VEX(x, y).fluxBottom = VEX(x, y).fluxBottom * scalingK;
			}
			else {
				VEX(x, y).fluxBottom = VEX(x, y).fluxBottom * scalingK;
				VEX(x, y + 1).fluxBottom = 0.0;
			}

			//double test_values[]= {VEX(x,y).d,VEX(x,y).Waterdistribution,VEX(x,y).fluxLeft,VEX(x,y).fluxRight,VEX(x,y).fluxTop,VEX(x,y).fluxBottom};
			//Water_Vertex d = vert_array[(x)*ysize+(y)];
			/*if ( (VEX(x-1,y).fluxRight + VEX(x,y-1).fluxTop + VEX(x+1,y).fluxLeft+VEX(x,y+1).fluxBottom - (VEX(x,y).fluxLeft + VEX(x,y).fluxRight + VEX(x,y).fluxTop + VEX(x,y).fluxBottom)) < 0.000001){
				double test_values1[]= {VEX(x,y).d,VEX(x,y).d1,VEX(x,y).d2,VEX(x,y).Waterdistribution,VEX(x,y).fluxLeft,VEX(x,y).fluxRight,VEX(x,y).fluxTop,VEX(x,y).fluxBottom,VEX(x,y).Vx,VEX(x,y).Vy};
				int f = 2;
				}*/


		}
	}
}

void WaterSimulation::surfaceAndVelocity()
{
	for (int x = 1; x < xsize - 1; x++){
		for (int y = 1; y < ysize - 1; y++){

			inflowSum = VEX(x - 1, y).fluxRight + VEX(x, y - 1).fluxTop + VEX(x + 1, y).fluxLeft + VEX(x, y + 1).fluxBottom;
			outflowSum = VEX(x, y).fluxLeft + VEX(x, y).fluxRight + VEX(x, y).fluxTop + VEX(x, y).fluxBottom;

			VEX(x, y).Volume = max(0.0, (inflowSum - outflowSum)); /*darf nicht kleiner 0 sein*/;

			//update water height
			VEX(x, y).d2 = VEX(x, y).d1 + VEX(x, y).Volume / (pipeLength*pipeLength);

			//double test_values[]= {VEX(x,y).d,VEX(x,y).Waterdistribution,VEX(x,y).fluxLeft,VEX(x,y).fluxRight,VEX(x,y).fluxTop,VEX(x,y).fluxBottom};

			waterpassX = (VEX(x - 1, y).fluxRight - VEX(x, y).fluxLeft + VEX(x, y).fluxRight - VEX(x + 1, y).fluxLeft) / 2.0;
			waterpassY = (VEX(x, y - 1).fluxTop - VEX(x, y).fluxBottom + VEX(x, y - 1).fluxBottom - VEX(x, y).fluxTop) / 2.0;

			dAvg = (VEX(x, y).d1 + VEX(x, y).d2) / 2.0;

			//if (d_avg > pow(10.0,-10) && (Waterpass_x != 0.0 || Waterpass_y != 0.0)){
			VEX(x, y).Vx = waterpassX / (dAvg * pipeLength);//*d_avg;
			VEX(x, y).Vy = waterpassY / (dAvg * pipeLength);//*d_avg;

			//if (VEX(x,y).Vx > 60 || VEX(x,y).Vy > 60){
			//	int d =0;
			//}


			/*if (DT * VEX(x,y).Vx > l || VEX(x,y).Vy >= l){ //CFL condition for stability: DT * Vx <= lX, DT * Vy <= lY
					double test_values[]= {VEX(x,y).d,VEX(x,y).Waterdistribution,VEX(x,y).fluxLeft,VEX(x,y).fluxRight,VEX(x,y).fluxTop,VEX(x,y).fluxBottom,VEX(x,y).Vx,VEX(x,y).Vy};
					DT = l/max(VEX(x,y).Vx,VEX(x,y).Vy); 
					}*/

		}
	}
}






void  WaterSimulation::erosionDeposition()
{
	double C; //sediment transport capacity 
	double alpha; // local tilt angle
	double velo;

	for (int x = 1; x < xsize - 1; x++){
		for (int y = 1; y < ysize - 1; y++){

			alpha = max(0.01, max(abs(Alti.angleSlopeX(x, y)), abs(Alti.angleSlopeY(x, y))));
			velo = sqrt(VEX(x, y).Vx * VEX(x, y).Vx + VEX(x, y).Vy * VEX(x, y).Vy)/*betrag des velocity vektor*/;
			C = KC /*sediment capacity*/ * sin(alpha) * velo;//* 


			if (C > VEX(x, y).sediment_amount){
				//test_before = VEX(x,y).z;
				VEX(x, y).z = VEX(x, y).z - KS *(C - VEX(x, y).sediment_amount); // z = z - Ks * (C - st)
				//test_delta = abs(VEX(x,y).z - test_before);
				VEX(x, y).sediment_amount1 = VEX(x, y).sediment_amount + KS * (C - VEX(x, y).sediment_amount); // s1 = st + Ks (C - st)
				//test = VEX(x,y).sediment_amount + KS * (C - VEX(x,y).sediment_amount);
			}
			else {
				//test_before = VEX(x,y).z;
				VEX(x, y).z = VEX(x, y).z + KD * (VEX(x, y).sediment_amount - C); // z = z + Ks * (st - C)
				//test_delta = abs(VEX(x,y).z - test_before);
				VEX(x, y).sediment_amount1 = VEX(x, y).sediment_amount - KD * (VEX(x, y).sediment_amount - C); // s1 = st - Ks (st - C)
				//test = VEX(x,y).sediment_amount + KS * (C - VEX(x,y).sediment_amount);
			}
		}
	}
}

void WaterSimulation::sedimentTransport()
{
	double avg_sediment = 0;

	for (int x = 1; x < xsize - 1; x++){
		for (int y = 1; y < ysize - 1; y++){
			//Water_Vertex test2 = VEX(x,y);
			if (((x - int(abs(floor(VEX(x, y).Vx)))) <= 0) || ((y - int(abs(floor(VEX(x, y).Vy)))) <= 0)) //check if values in grid
			{
				for (int i = -1; i <= 1; i++){
					for (int j = -1; j <= 1; j++)
					{
						if (i != 0 || j != 0)
							avg_sediment += VEX(x + i, y + i).sediment_amount1;
					}
				}
				avg_sediment /= 8.0;
				VEX(x, y).sediment_amount = avg_sediment;
			}
			else
				VEX(x, y).sediment_amount = VEX(x - int(abs(floor(VEX(x, y).Vx))), y - int(abs(floor(VEX(x, y).Vy)))).sediment_amount1;
		}
	}
}


void WaterSimulation::evaporation()
{
	for (int x = 0; x < xsize; x++){
		for (int y = 0; y < ysize; y++){
			VEX(x, y).d = VEX(x, y).d2 * (1.0 - KE);
		}
	}
}





void WaterSimulation::showProgressbar(void){
	dialog = new wxProgressDialog(wxT("Progress of Operation"),
		wxT("Progress of Operation"), 100, NULL, wxPD_AUTO_HIDE);
}


void WaterSimulation::normalizeHeight(){
	for (int x = 0; x < xsize; x++){
		for (int y = 0; y < ysize; y++){
			if (VEX(x, y).z < 0.0)
				VEX(x, y).z = 0.0;
			else if (VEX(x, y).z > 1.0)
				VEX(x, y).z = 1.0;
		}
	}
}

void WaterSimulation::visualizeWater2D(void){
	tempImg = global_preview_bitmap.ConvertToImage();

	for (int x = 0; x < xsize; x++){
		for (int y = 0; y < ysize; y++){
			if (VEX(x, y).w > 0.001 || VEX(x, y).d > 0.001){
				if (method == 1)
					normalized = int(1.0 / 2.0 * (1.0 - VEX(x, y).d / (sqrt(VEX(x, y).d2 * VEX(x, y).d + 0.1)) * 255)); // 1/2 * (1-d/(sqrt(d² + a)) * 255 
				else
					normalized = int(1.0 / 2.0 * (1.0 - VEX(x, y).w / (sqrt(VEX(x, y).w * VEX(x, y).w + 0.1)) * 255)); // 1/2 * (1-d/(sqrt(d² + a)) * 255 

				tempImg.SetRGB(x, (ysize - 1) - y, tempImg.GetRed(x, y), tempImg.GetGreen(x, y), (unsigned char)normalized);
			}
		}
	}

	global_preview_bitmap = wxBitmap(tempImg, -1);
}

void WaterSimulation::visualizeSediment2D(void){
	tempImg = g_sedimentBitmap.ConvertToImage();
	for (int x = 0; x < xsize; x++){
		for (int y = 0; y < ysize; y++){
			if (VEX(x, y).sediment_amount > 0.01){
				normalized = int(1.0 / 2.0 * (1.0 - VEX(x, y).sediment_amount / 
					(sqrt(VEX(x, y).sediment_amount * VEX(x, y).sediment_amount + 0.1)) * 255)); // 1/2 * (1-d/(sqrt(d² + a)) * 255 

				tempImg.SetRGB(x, (ysize - 1) - y, (unsigned char)normalized, tempImg.GetGreen(x, y), tempImg.GetBlue(x, y));
			}
		}
	}
	g_sedimentBitmap = wxBitmap(tempImg, -1);
}


//#----------------------------------------------------------------------

void WaterSimulation::strictSolver()
{
	double avg_a;
	double a;
	double delta_w;
	double d_tot;
	double m_max;
	double delta_m;
	double water_amount;
	int iterations = 100; 
	double progress = 0;
	//double dissolve;
	showProgressbar();

	for (int iter = 0; iter < iterations; iter++)
	{
		for (int x = 1; x < xsize - 1; x++){
			for (int y = 1; y < ysize - 1; y++){
				VEX(x, y).w += KR;//rain constant
				VEX(x, y).z -= KS * VEX(x, y).w;
				VEX(x, y).sediment_amount += KS * VEX(x, y).w;//soliubilty constant

				avg_a = averageHeight(x, y);
				a = VEX(x, y).w + VEX(x, y).z + VEX(x, y).sediment_amount;
				d_tot = dTotal(x, y);

				if (d_tot == 0)//no water differences
				{
					continue;
				}

				water_amount = min(VEX(x, y).w, a - avg_a);
				VEX(x, y).w -= water_amount;
				for (int i = -1; i <= 1; i++){
					for (int j = -1; j <= 1; j++)
					{
						if (i != 0 || j != 0)
						{
							delta_w = water_amount * (a - (VEX(x + i, y + j).z + VEX(x + i, y + j).w + VEX(x + i, y + j).sediment_amount)) / d_tot;
							VEX(x + i, y + j).w += delta_w; //d1 = delta water map
							//dissolve =2.0 * ( VEX(x+i,y+i).w + VEX(x+i,y+i).z)  * water_amount
							VEX(x + i, y + j).sediment_amount += VEX(x, y).sediment_amount * delta_w / VEX(x, y).w;
							//VEX(x+i,y+j).sediment_amount += VEX(x+i,y+j).w * VEX(x,y).sediment_amount / d_tot + dissolve;
							//VEX(x+i,y+j).z += VEX(x+i,y+j).sediment_amount;
						}
					}
				}

				VEX(x, y).w -= min(VEX(x, y).w, a - avg_a);
				VEX(x, y).w *= (1 - KE); //evaporation

				m_max = KC * VEX(x, y).w; //maximum sediment amount (=capacity)
				delta_m = max(0, VEX(x, y).sediment_amount - m_max); // amount exceeding maximum amount is deposited
				VEX(x, y).sediment_amount -= delta_m;
				VEX(x, y).z += delta_m;
			}
		}


		if (iter % 5 == 0)
		{
			if (frame->water_preview_on)   { visualizeWater2D(); }
			if (frame->sediment_preview_on) { visualizeSediment2D(); }
			frame->manualRender();
		}

		progress += 100.0 / double(iterations); //(die endzahl muss 100 sein
		dialog->Update(int(progress));
	}

	dialog->Destroy();
}

double inline WaterSimulation::averageHeight(int x, int y)
{
	double total_height = 0;
	int cells = 1; //1 = die x,y zelle
	double height = 0;
	for (int i = -1; i <= 1; i++){
		for (int j = -1; j <= 1; j++)
		{
			if (i != 0 || j != 0)
			{
				height = VEX(x + i, y + i).z + VEX(x + i, y + i).w + VEX(x + i, y + i).sediment_amount;
				if (height > 0)
				{
					total_height += height;
					cells++;
				}
			}
		}
	}

	return total_height / double(cells);
}

double inline WaterSimulation::dTotal(int x, int y)
{
	double dTot = 0;
	double a = VEX(x, y).z + VEX(x, y).w + VEX(x, y).sediment_amount;
	double dCell;
	for (int i = -1; i <= 1; i++){
		for (int j = -1; j <= 1; j++)
		{
			if (i != 0 || j != 0)
			{
				dCell = (a - VEX(x + i, y + j).z + VEX(x + i, y + j).w + VEX(x + i, y + j).sediment_amount);
				if (dCell > 0)
				{
					dTot += dCell;
				}
			}
		}
	}

	return dTot;
}



void WaterSimulation::fastSolver()
{
	int rain_freq = 1;
	showProgressbar();
	int iterations = 100; //test
	double progress = 0.0;

	for (int iter = 0; iter < iterations; iter++) {

		// rain
		if (iter%rain_freq == 0) {
			for (int x = 0; x < xsize; x++){
				for (int y = 0; y < ysize; y++){
					VEX(x, y).w += KR;
				}
			}
		}
		double h, h1, h2, h3, h4, d1, d2, d3, d4;
		double total_height = 0;
		double totalHeightDiff = 0;
		double avgHeight, waterAmount, waterInv, sediment_amount, sediment_inv, dissolve;
		// water and sediment transport
		for (int x = 1; x < xsize - 1; x++){
			for (int y = 1; y < ysize - 1; y++){

				// calculate total heights and height differences
				h = VEX(x, y).z + VEX(x, y).w;

				h1 = VEX(x, y + 1).z + VEX(x, y + 1).w + VEX(x, y + 1).sediment_amount;
				h2 = VEX(x - 1, y).z + VEX(x - 1, y).w + VEX(x - 1, y).sediment_amount;
				h3 = VEX(x + 1, y).z + VEX(x + 1, y).w + VEX(x + 1, y).sediment_amount;
				h4 = VEX(x, y - 1).z + VEX(x, y - 1).w + VEX(x, y - 1).sediment_amount;

				d1 = h - h1;
				d2 = h - h2;
				d3 = h - h3;
				d4 = h - h4;

				// calculate amount of water and sediment to transport
				total_height = 0;
				totalHeightDiff = 0;
				int cells = 1;

				if (d1 > 0) {
					totalHeightDiff += d1;
					total_height += h1;
					cells++;
				}
				if (d2 > 0) {
					totalHeightDiff += d2;
					total_height += h2;
					cells++;
				}
				if (d3 > 0) {
					totalHeightDiff += d3;
					total_height += h3;
					cells++;
				}
				if (d4 > 0) {
					totalHeightDiff += d4;
					total_height += h4;
					cells++;
				}

				if (cells == 1) {
					continue;
				}

				avgHeight = total_height / cells;

				waterAmount = min(VEX(x, y).w, h - avgHeight);
				VEX(x, y).d1 -= waterAmount;
				waterInv = waterAmount / totalHeightDiff;

				sediment_amount = VEX(x, y).sediment_amount;
				VEX(x, y).sediment_amount1 -= sediment_amount;
				sediment_inv = sediment_amount / totalHeightDiff;

				// transport water and sediment and dissolve more material
				if (d1 > 0) {
					VEX(x, y + 1).d1 += d1*waterInv;
					dissolve = d1*waterAmount;
					VEX(x, y + 1).sediment_amount1 += d1*sediment_inv + dissolve;
					VEX(x, y + 1).z -= dissolve;
				}
				if (d2 > 0) {
					VEX(x - 1, y).d1 += d1*waterInv;
					dissolve = d1*waterAmount;
					VEX(x - 1, y).sediment_amount1 += d1*sediment_inv + dissolve;
					VEX(x - 1, y).z -= dissolve;
				}
				if (d3 > 0) {
					VEX(x + 1, y).d1 += d1*waterInv;
					dissolve = d1*waterAmount;
					VEX(x + 1, y).sediment_amount1 += d1*sediment_inv + dissolve;
					VEX(x + 1, y).z -= dissolve;
				}
				if (d4 > 0) {
					VEX(x, y - 1).d1 += d1*waterInv;
					dissolve = d1*waterAmount;
					VEX(x, y - 1).sediment_amount1 += d1*sediment_inv + dissolve;
					VEX(x, y - 1).z -= dissolve;
				}
			}
		}

		// apply changes to water map
		for (int x = 1; x < xsize - 1; x++){
			for (int y = 1; y < ysize - 1; y++){
				VEX(x, y).w += VEX(x, y).d1;

				// apply changes to sediment map
				VEX(x, y).sediment_amount += VEX(x, y).sediment_amount1;

				// apply changes to height map
				//channel.channelAddNoClip(height_map_diff);

				// water vaporization
				//VEX(x,y).w *= (1-KE);

				// sedimentation
				VEX(x, y).sediment_amount1 = VEX(x, y).sediment_amount - VEX(x, y).w;
				VEX(x, y).sediment_amount -= VEX(x, y).sediment_amount1;
				VEX(x, y).z += VEX(x, y).sediment_amount1;

				// clear diff maps
				VEX(x, y).sediment_amount1 = 0;
				VEX(x, y).d1 = 0;
			}
		}

		if (iter % 5 == 0)
		{
			if (frame->water_preview_on)   { visualizeWater2D(); }
			if (frame->sediment_preview_on) { visualizeSediment2D(); }
			frame->manualRender();
		}

		progress += 100.0 / double(iterations); //(die endzahl muss 100 sein
		dialog->Update(int(progress));
	}
	dialog->Destroy();



}

//-----------------------------------------------------------------------------------------


void  WaterSimulation::hydraulicErosion()
{
	int xsize = Alti.getXSize();
	int ysize = Alti.getYSize();
	int x;
	int y;
	int xNeighbours;
	int yNeighbours;
	int xShift;
	int yShift;
	int xIndex;
	int yIndex;
	int Nx;
	int Ny;
	double d;
	double heightAtIndex;
	double heightAtPoint;
	int nNeighbours;
	double maxSediment;
	double sedimentAtCell;
	double heightAtCell;
	int iterations = iterations;

	// Start iterations...
	double waterAmount;
	double sedimentAmount;
	double erodedSediment;
	double dtotal = 0.0;
	double dMax = 0.0;
	double waterAtIndex;
	double waterAtPoint;
	double hCumulative;

	showProgressbar();

	if (WatersourceOn)
	{
		waterflowDistribution(waterAmount);
	}

	for (int iter = 0; iter < iterations; iter++) {
		if (WatersourceOn)
		{
			for (y = 0; y < ysize; y++) {
				for (x = 0; x < xsize; x++) {
					waterAmount = VEX(x, y).w + VEX(x, y).Waterdistribution;
					if (waterAmount > 1.0) {
						waterAmount = 1.0;
					}
					VEX(x, y).w = waterAmount;
				}
			}
		}
		if (RainOn)
		{
			for (y = 0; y < ysize; y++) {
				for (x = 0; x < xsize; x++) {
					waterAmount = VEX(x, y).w + KR;
					if (waterAmount > 1.0) {
						waterAmount = 1.0;
					}
					VEX(x, y).w = waterAmount;
				}
			}
		}

		// dissolve material as sediment
		for (y = 0; y < ysize; y++) {
			for (x = 0; x < xsize; x++) {
				sedimentAmount = VEX(x, y).sediment_amount;
				maxSediment = VEX(x, y).w * KS;
				if (sedimentAmount < maxSediment) {
					erodedSediment = VEX(x, y).w * KS;
					if (sedimentAmount + erodedSediment > maxSediment) {
						erodedSediment = maxSediment - sedimentAmount;
					}
					heightAtIndex = VEX(x, y).z;
					if (erodedSediment > heightAtIndex) {
						erodedSediment = heightAtIndex;
					}
					VEX(x, y).sediment_amount = sedimentAmount + erodedSediment;
					VEX(x, y).z = heightAtIndex - erodedSediment;
				}
			}
		}
		// calculate height differences and move water
		for (y = 0; y < ysize; y++) {
			// y...
			if (y == 0) {
				yNeighbours = 2;
				yShift = 0;
				yIndex = 0;
			}
			else if (y == ysize - 1) {
				yNeighbours = 2;
				yShift = -1;
				yIndex = 1;
			}
			else {
				yNeighbours = 3;
				yShift = -1;
				yIndex = 1;
			}
			for (x = 0; x < xsize; x++) {
				// x...
				if (x == 0) {
					xNeighbours = 2;
					xShift = 0;
					xIndex = 0;
				}
				else if (x == xsize - 1) {
					xNeighbours = 2;
					xShift = -1;
					xIndex = 1;
				}
				else {
					xNeighbours = 3;
					xShift = -1;
					xIndex = 1;
				}

				dtotal = 0.0f;
				dMax = 0.0f;
				heightAtIndex = VEX(x, y).z; // Get height at index
				waterAtIndex = VEX(x, y).w; // Get water at index
				waterAtPoint;
				hCumulative = heightAtIndex;
				nNeighbours = 0;


				for (Ny = 0; Ny < yNeighbours; Ny++) {
					for (Nx = 0; Nx < xNeighbours; Nx++) {
						//If cell not index cell
						if (Nx != xIndex || Ny != yIndex) {
							heightAtPoint = VEX(x + Nx + xShift, y + Ny + yShift).z; // Get height at point
							waterAtPoint = VEX(x + Nx + xShift, y + Ny + yShift).w; // Get water at point
							d = (heightAtIndex + waterAtIndex) - (heightAtPoint + waterAtPoint);
							if (d > 0) {
								//sum of positive d's
								dtotal += d;
								hCumulative += heightAtPoint + waterAtPoint;
								// number of outflow neighbours
								nNeighbours++;
								if (d > dMax) {
									d = dMax;
								}
							}
						}
					}
				}
				//calculate water outflow from index cell
				double hAverage = hCumulative / (nNeighbours + 1);
				double dWater = (heightAtIndex + waterAtIndex) - hAverage;
				double transportedWater = min(waterAtIndex, dWater);

				VEX(x, y).d1 -= transportedWater;
				
				//calculate sediment outflow from index cell
				double transportedSediment = VEX(x, y).sediment_amount * (transportedWater / waterAtIndex);
				VEX(x, y).sediment_amount1 -= transportedSediment;

				// neighbours
				for (Ny = 0; Ny < yNeighbours; Ny++) {
					for (Nx = 0; Nx < xNeighbours; Nx++) {
						if (Nx != xIndex || Ny != yIndex) {
							heightAtPoint = VEX(x + Nx + xShift, y + Ny + yShift).z; 
							waterAtPoint = VEX(x + Nx + xShift, y + Ny + yShift).w; 
							d = (heightAtIndex + waterAtIndex) - (heightAtPoint + waterAtPoint);
							if (d > 0) {
								//shift water to neighbouring cell
								VEX(x + Nx + xShift, y + Ny + yShift).d1 += transportedWater * (d / dtotal);
								//shift sediment to neighbouring cell
								VEX(x + Nx + xShift, y + Ny + yShift).sediment_amount1 += transportedSediment * (d / dtotal);
							}
						}
					}
				}
			}
		}
		//apply water difference to map
		double waterAtCell;
		for (y = 0; y < ysize; y++) {
			for (x = 0; x < xsize; x++) {
				waterAtCell = VEX(x, y).w + VEX(x, y).d1;
				//calculate evaporation
				double evaporatedWater = waterAtCell * KE;
				waterAtCell -= evaporatedWater;
				if (waterAtCell < 0.0) {
					waterAtCell = 0.0;
				}
				VEX(x, y).w = waterAtCell;
				VEX(x, y).d1 = 0.0;
			}
		}
		//apply sediment difference to map
		for (y = 0; y < ysize; y++) {
			for (x = 0; x < xsize; x++) {
				sedimentAtCell = VEX(x, y).sediment_amount + VEX(x, y).sediment_amount1;
				if (sedimentAtCell > 1.0) {
					sedimentAtCell = 1.0;
				}
				else if (sedimentAtCell < 0.0) {
					sedimentAtCell = 0.0;
				}
				VEX(x, y).sediment_amount = sedimentAtCell;
				VEX(x, y).sediment_amount1 = 0.0;
			}
		}
		// deposit sediment
		for (y = 0; y < ysize; y++) {
			for (x = 0; x < xsize; x++) {
				maxSediment = VEX(x, y).w * KS;
				sedimentAtCell = VEX(x, y).sediment_amount;
				if (sedimentAtCell > maxSediment) {
					double depositedSediment = sedimentAtCell - maxSediment;
					VEX(x, y).sediment_amount = maxSediment;
					heightAtCell = VEX(x, y).z;
					VEX(x, y).z = heightAtCell + depositedSediment;
				}
			}
		}
		// Update progress...
		double percentComplete = (double)iter / (double)iterations;
		dialog->Update(int(percentComplete * 100));

		if (iter % 5 == 0)
		{
			if (frame->water_preview_on)   { visualizeWater2D(); }
			if (frame->sediment_preview_on) { visualizeSediment2D(); }
			frame->manualRender();
		}
	}
	dialog->Destroy();
}
