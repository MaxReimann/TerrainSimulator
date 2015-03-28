#include "imports.h"
#include <valarray>
#include "main.h"
#include "globals.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//#define DT 0.01f //Timestep
#define GRAVITY 9.81f

//#define WATER_AMOUNT 5000.0f 
//#define RADIUS 100.0f //Radius of water source
//#define PIPE_LENGTH 1.0f

#define AREA 1.0f //Square area of pipe
//
//#define KC 0.001f //sediment capacity constant 
//#define KD 0.05f //deposition constant 
//#define KS 0.03f //dissolving constant 
//#define KE 0.02f //evaporation Constant
//#define KR 0.001f // rain sclae constant


#define VEX(x,y) (vert_array[(x)*ysize+(y)]) //liefert ein struct zurueck

using namespace std;

Water_Simulation::Water_Simulation(const struct WaterConstants *constants,int _method)
{
	ysize = Alti.getysize();
	xsize = Alti.getxsize();
	vert_array = new WaterVertex [xsize * ysize];

	errorstream.open("error_sim_water.txt",ios::out);
	


	method = _method;
 
	KC = constants->KC;
	KD = constants->KD;
	KS = constants->KS;
	KE = constants->KE;
	KR = constants->KR;
	WATER_AMOUNT = constants->Water_Amount;
	RADIUS  = constants->Radius;
	PIPE_LENGTH = constants->Pipe_length_x;
	DT = constants->DT;
	Iterations = constants->Iterations;
	Rain_on = constants->Rain_on;
	Water_source_on = constants->Watersource_on;
	
	l = 1.0;
	A = AREA;

	//Anfagswerte festlegen
	for (int x = 0; x < xsize; x++)
	{
		for (int y = 0;y < ysize; y++)
		{

			VEX(x,y).z = Alti.getaltitude(x,y);// (Alti.getaltitude(x,y) == 0.0 ? (Alti.getaltitude(x,y) + 0.001) :  Alti.getaltitude(x,y));
			VEX(x,y).Waterdistribution = 0.0; 
			VEX(x,y).d = 0.0; // wasser ist am anfang ueberall 0
			VEX(x,y).d1 = 0.0;
			VEX(x,y).d2 = 0.0; 
			
			VEX(x,y).fluxLeft = 0.0;
			VEX(x,y).fluxRight = 0.0;
			VEX(x,y).fluxTop = 0.0;
			VEX(x,y).fluxBottom = 0.0;
			VEX(x,y).Volume = 0.0;
			VEX(x,y).Vx = 0.0;
			VEX(x,y).Vy = 0.0;
			VEX(x,y).sediment_amount = 0.0;
			VEX(x,y).sediment_amount1 = 0.0;
			VEX(x,y).w = 0.0;
		}
	}


	if (method == 1)
	{
		if (constants->Watersource_on == true && constants->Rain_on == false)
		{
			Distribution(WATER_AMOUNT);
		}
		else if (constants->Watersource_on == false && constants->Rain_on == true)
		{
		Rain_Distribution();
		}
		else if (constants->Watersource_on == true && constants->Rain_on == true )
		{
			Distribution(WATER_AMOUNT);
			Rain_Distribution();
		}

		Sim_Loop();
	}

	else if (method == 2)
	{
		Water_Sim2();
	}
	else if (method == 3)
	{
		Water_Sim3();
	}
	else
	{
		Water_Sim4();
	}

	normalize_height();

	for (int x = 0; x < xsize; x++){
		for (int y = 0; y < ysize; y++)
		{
			/*if ( abs(Alti.getaltitude(x,y) - VEX(x,y).z) > 0.01)
			{
				errorstream <<  VEX(x,y).z;  
			}*/

			Alti.Alt(x,y, VEX(x,y).z ) ; //muss leider wieder in map zureuckgeschrieben werden und spater wieder in vertex geschrieben
			//Alti.sediment_map[(x)* ysize+(y)] = VEX(x,y).sediment_amount;
		}
	}
	//Alti.normalize2();

}

Water_Simulation::~Water_Simulation()
{
	delete [] vert_array;
	errorstream.close();
	/*for (int x = 0; x < xsize; x++){
		for (int y = 0; y < ysize; y++){
			delete vert_array[(x)*ysize+(y)];
		}
	}*/
	
}
void Water_Simulation::Rain_Distribution(void)
{
 seconds = time (NULL);
 srand(seconds);
 for (int x = 0; x < xsize; x++){
	for (int y = 0; y < ysize; y++){
		if (VEX(x,y).z > 0.0){
			VEX(x,y).Waterdistribution =   100 *  KR * (rand() % 2) /*zufallszahlen */  / double(Iterations) ;
		}
		else {
			VEX(x,y).Waterdistribution = 0.0;
		}
	}
  }
}




void Water_Simulation::Distribution(double Water_Amount){
	//valarray<double> temp_array(Alti.map,Alti.getxsize() * ysize);
	
	
	//hoechsten punkt im gelaende finden 
	int temp_x, temp_y;
	double temp_z = 0;

	//Alti.FlagX = temp_x;
	//Alti.FlagY = temp_y; //den ort des ausflusses anzeigen
	//die quelle des wasserflusses auf die karte schreiben
	temp_x = Alti.FlagX;
	temp_y = Alti.FlagY;
	int temp_x_smallest = ((temp_x  - RADIUS/2) > 0 ) ? (temp_x  - RADIUS/2) : 0;
	int temp_y_smallest = ((temp_y  - RADIUS/2) > 0 ) ? (temp_y  - RADIUS/2) : 0;
	int temp_x_biggest = ((temp_x  + RADIUS/2) < xsize ) ? (temp_x  + RADIUS/2) : (xsize);
	int temp_y_biggest = ((temp_y  + RADIUS/2) < ysize ) ? (temp_y  + RADIUS/2) : (ysize);
	
	for(int x = temp_x_smallest; x < temp_x_biggest;x++){
		for(int y = temp_y_smallest; y < temp_y_biggest;y++){
			VEX(x,y).Waterdistribution = (Water_Amount) / (double(RADIUS)+sqrt(double(RADIUS-x)*double(RADIUS-x) + double(RADIUS-y)*double(RADIUS-y)));
			//double test = VEX(x,y).Waterdistribution;
		}
	}

	
		
}

void Water_Simulation::Increase_Water()
{
	for (int x = 0; x < xsize; x++){
		for (int y = 0; y < ysize; y++){
			VEX(x,y).d1 = VEX(x,y).d +  VEX(x,y).Waterdistribution;
		}
	}
}

void Water_Simulation::Flux_Sim() 
{
	double Scaling_K;
	double scale;
	double V_out;


	for (int x = 1; x < xsize-1; x++){
		for (int y = 1; y < ysize-1; y++){
			dif_height_l = VEX(x,y).z + VEX(x,y).d1 - VEX(x-1,y).z - VEX(x-1,y).d1;
			dif_height_r = VEX(x,y).z + VEX(x,y).d1 - VEX(x+1,y).z - VEX(x+1,y).d1;
			dif_height_t = VEX(x,y).z + VEX(x,y).d1 - VEX(x,y+1).z - VEX(x,y+1).d1;
			dif_height_b = VEX(x,y).z + VEX(x,y).d1 - VEX(x,y-1).z - VEX(x,y-1).d1;

			
			VEX(x,y).fluxLeft = max(0.0,VEX(x,y).fluxLeft + ( A * (GRAVITY * abs(dif_height_l))/l)); //outflow flux muss immmer mindestens 0 sein
			VEX(x,y).fluxRight = max(0.0,VEX(x,y).fluxRight + ( A * (GRAVITY * abs(dif_height_r))/l));
			VEX(x,y).fluxTop = max(0.0,VEX(x,y).fluxTop + ( A * (GRAVITY * abs(dif_height_t))/l));
			VEX(x,y).fluxBottom = max(0.0,VEX(x,y).fluxBottom + ( A * (GRAVITY * abs(dif_height_b))/l));

			
			//double test_values1[]= {VEX(x,y).d,VEX(x,y).Waterdistribution,VEX(x,y).fluxLeft,VEX(x,y).fluxRight,VEX(x,y).fluxTop,VEX(x,y).fluxBottom};
			V_out = (VEX(x,y).fluxLeft + VEX(x,y).fluxRight + VEX(x,y).fluxTop + VEX(x,y).fluxBottom);
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
			scale = (VEX(x,y).d1 * l*l)/(V_out);
			Scaling_K = min(1.0,scale);
			//Scaling_K = 1.0;
			
			if (x != 1){
			VEX(x,y).fluxLeft = VEX(x,y).fluxLeft * Scaling_K;}
			else {
				VEX(x,y).fluxLeft = VEX(x,y).fluxLeft * Scaling_K;
				VEX(x-1,y).fluxLeft = 0.0;  //an der grenze outflow auf 0 setzen
			}

			if ( x < xsize - 2 ){
				VEX(x,y).fluxRight = VEX(x,y).fluxRight * Scaling_K;}
			else {
				VEX(x,y).fluxRight = VEX(x,y).fluxRight * Scaling_K;
				VEX(x+1,y).fluxRight = 0.0;
			}

			if (y != 1){
				VEX(x,y).fluxTop = VEX(x,y).fluxTop * Scaling_K; }
			else {
				VEX(x,y).fluxTop = VEX(x,y).fluxTop * Scaling_K; 
				VEX(x,y-1).fluxTop = 0.0;
			}

			if (y < ysize - 2){
				VEX(x,y).fluxBottom = VEX(x,y).fluxBottom * Scaling_K;}
			else {
				VEX(x,y).fluxBottom = VEX(x,y).fluxBottom * Scaling_K;
				VEX(x,y+1).fluxBottom = 0.0;
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

void Water_Simulation::Surface_and_Velocity()
{
	for (int x = 1; x < xsize-1; x++){
		for (int y = 1; y < ysize-1; y++){
			
			inflow_sum = VEX(x-1,y).fluxRight + VEX(x,y-1).fluxTop + VEX(x+1,y).fluxLeft+VEX(x,y+1).fluxBottom;
			outflow_sum = VEX(x,y).fluxLeft + VEX(x,y).fluxRight + VEX(x,y).fluxTop + VEX(x,y).fluxBottom;
			
			VEX(x,y).Volume =  max(0.0,  (inflow_sum - outflow_sum)) ; /*darf nicht kleiner 0 sein*/;
			
			//update water height
			VEX(x,y).d2 = VEX(x,y).d1 + VEX(x,y).Volume/(l*l);

			//double test_values[]= {VEX(x,y).d,VEX(x,y).Waterdistribution,VEX(x,y).fluxLeft,VEX(x,y).fluxRight,VEX(x,y).fluxTop,VEX(x,y).fluxBottom};

			Waterpass_x = (VEX(x-1,y).fluxRight - VEX(x,y).fluxLeft + VEX(x,y).fluxRight - VEX(x+1,y).fluxLeft)/ 2.0;
			Waterpass_y = (VEX(x,y-1).fluxTop - VEX(x,y).fluxBottom + VEX(x,y-1).fluxBottom - VEX(x,y).fluxTop)/ 2.0;

			d_avg = (VEX(x,y).d1 + VEX(x,y).d2)/2.0;

			//if (d_avg > pow(10.0,-10) && (Waterpass_x != 0.0 || Waterpass_y != 0.0)){
			VEX(x,y).Vx = Waterpass_x/(d_avg * l);//*d_avg;
			VEX(x,y).Vy = Waterpass_y/(d_avg * l);//*d_avg;
			
			//if (VEX(x,y).Vx > 60 || VEX(x,y).Vy > 60){
			//	int d =0;
			//}


		/*if (DT * VEX(x,y).Vx > l || VEX(x,y).Vy >= l){ //CFL condition zur stabiltaet: DT * Vx <= lX, DT * Vy <= lY
				double test_values[]= {VEX(x,y).d,VEX(x,y).Waterdistribution,VEX(x,y).fluxLeft,VEX(x,y).fluxRight,VEX(x,y).fluxTop,VEX(x,y).fluxBottom,VEX(x,y).Vx,VEX(x,y).Vy};
				DT = l/max(VEX(x,y).Vx,VEX(x,y).Vy); //wenn CFL nicht erfuellt , DT runterskalieren um numerische stabilitaet zu behalten
			}*/

		}
	}
}


	



void  Water_Simulation::Erosion_Deposition()
{
	double C; //sediment transport capacity 
	double alpha; // local tilt angle
	double velo;

	/*double test;
	double test_before;
	double test_delta;*/


	for (int x = 1; x < xsize-1; x++){
		for (int y = 1; y < ysize-1; y++){

			alpha = max(0.01,max(abs(Alti.angle_slope_x(x,y)),abs(Alti.angle_slope_y(x,y))));
			velo = sqrt( VEX(x,y).Vx * VEX(x,y).Vx + VEX(x,y).Vy * VEX(x,y).Vy )/*betrag des velocity vektor*/;
			C = KC /*sediment capacity*/ * sin(alpha) * velo ;//* 


			if(C > VEX(x,y).sediment_amount){
				//test_before = VEX(x,y).z;

				VEX(x,y).z = VEX(x,y).z - KS *( C - VEX(x,y).sediment_amount); // z = z - Ks * (C - st)

				//test_delta = abs(VEX(x,y).z - test_before);

				VEX(x,y).sediment_amount1 = VEX(x,y).sediment_amount + KS * (C - VEX(x,y).sediment_amount); // s1 = st + Ks (C - st)
				//test = VEX(x,y).sediment_amount + KS * (C - VEX(x,y).sediment_amount);
			}
			else {
				//test_before = VEX(x,y).z;

				VEX(x,y).z = VEX(x,y).z + KD * ( VEX(x,y).sediment_amount - C); // z = z + Ks * (st - C)
				
				//test_delta = abs(VEX(x,y).z - test_before);

				VEX(x,y).sediment_amount1 = VEX(x,y).sediment_amount - KD * (VEX(x,y).sediment_amount - C); // s1 = st - Ks (st - C)

				//test = VEX(x,y).sediment_amount + KS * (C - VEX(x,y).sediment_amount);
			}
		}
	}
}

void Water_Simulation::Sediment_Transport()
{
	double avg_sediment = 0;
	
	for (int x = 1; x < xsize-1; x++){
		for (int y = 1; y < ysize-1; y++){
			//Water_Vertex test2 = VEX(x,y);
			if(( (x - int( abs(floor(VEX(x,y).Vx ) )) ) <= 0) || ((y - int( abs(floor(VEX(x,y).Vy)) )) <= 0 ) ) //wenn werte nicht mehr im grid
			{
				for (int i = -1; i <= 1; i++){
					for (int j= -1; j <= 1; j++)
					{
						if (i != 0 || j != 0)
						{
							avg_sediment += VEX(x+i,y+i).sediment_amount1;
						}
					}
				}
				
				avg_sediment /= 8.0 ;  //die durchschnittliche menge sediment der 8 nachbarn

				VEX(x,y).sediment_amount = avg_sediment;
			}
			
			else {
				VEX(x,y).sediment_amount = VEX(x - int( abs(floor( VEX(x,y).Vx )) ),y - int( abs(floor( VEX(x,y).Vy  )) )).sediment_amount1;
				//double t = VEX(x,y).sediment_amount;
				//int sd = 0;
			}
		}
	}
}


void Water_Simulation::Evaporation()
{
	for (int x = 0; x < xsize; x++){
		for (int y = 0; y < ysize; y++){
			VEX(x,y).d = VEX(x,y).d2 * (1.0 - KE);
		}
	}
}

	
void Water_Simulation::Sim_Loop()
{
	Progressbar();
	double progress = 0.0;

	for (int t = 0; t < int(Iterations); t++){
		
		progress += 100.0/(double(Iterations)); //(die endzahl muss 100 sein
		dialog->Update((int)floor(progress)); 

		if (Rain_on){
			Rain_Distribution();
		}
		/*if (Water_source_on)
		{
			Distribution(WATER_AMOUNT);
		}*/


			
		Increase_Water();
		Flux_Sim();
		Surface_and_Velocity();
		Erosion_Deposition();
		Sediment_Transport();
		Evaporation();

		if ( frame->water_preview_on)   { visualize_Water2D(); }
		if ( frame->sediment_preview_on){ visualize_Sediment2D(); }
		frame->ManualRender();

		
	}


	 dialog->Destroy();
}



void Water_Simulation::Progressbar(void){
	dialog = new wxProgressDialog(wxT("Fortschritt der Operation"),
		wxT("Fortschritt der Operation"),100,NULL, wxPD_AUTO_HIDE );
}


void Water_Simulation::normalize_height(){
   for (int x = 0; x < xsize; x++){
		for (int y = 0; y < ysize; y++){
			if (VEX(x,y).z < 0.0) {
				VEX(x,y).z = 0.0;
			}
			else if (VEX(x,y).z > 1.0) {
					VEX(x,y).z = 1.0;
			}
		}
   }
}

void Water_Simulation::visualize_Water2D(void){
	temp_img = global_preview_bitmap.ConvertToImage();
	   
	for (int x = 0; x < xsize; x++){
		for (int y = 0; y < ysize; y++){			
			//if (VEX(x,y).d > 0.00001){
			//normalized = int(1.0 / 2.0 * ( 1.0 - VEX(x,y).d / (sqrt( VEX(x,y).d * VEX(x,y).d + 0.1)) * 255)); // 1/2 * (1-d/(sqrt(d² + a)) * 255 
			if (VEX(x,y).w > 0.001 || VEX(x,y).d > 0.001){
				if (method == 1)
				{
					normalized = int(1.0 / 2.0 * ( 1.0 - VEX(x,y).d / (sqrt( VEX(x,y).d2 * VEX(x,y).d + 0.1)) * 255)); // 1/2 * (1-d/(sqrt(d² + a)) * 255 
				}
				else
				{
					normalized = int(1.0 / 2.0 * ( 1.0 - VEX(x,y).w / (sqrt( VEX(x,y).w * VEX(x,y).w + 0.1)) * 255)); // 1/2 * (1-d/(sqrt(d² + a)) * 255 
				}
				temp_img.SetRGB(x,(ysize-1)-y,temp_img.GetRed(x,y),temp_img.GetGreen(x,y),(unsigned char) normalized ); //den roten wert setzen

			}
		}
	   }
	   //global_preview_bitmap.Create();
		global_preview_bitmap = wxBitmap(temp_img,-1);
}

void Water_Simulation::visualize_Sediment2D(void){
	temp_img = global_sediment_bitmap.ConvertToImage();
	   for (int x = 0; x < xsize; x++){
		for (int y = 0; y < ysize; y++){
			
			if (VEX(x,y).sediment_amount > 0.01){
				normalized = int(1.0 / 2.0 * ( 1.0 - VEX(x,y).sediment_amount / (sqrt( VEX(x,y).sediment_amount * VEX(x,y).sediment_amount + 0.1)) * 255)); // 1/2 * (1-d/(sqrt(d² + a)) * 255 
				temp_img.SetRGB(x,(ysize-1)-y,(unsigned char) normalized,temp_img.GetGreen(x,y),temp_img.GetBlue(x,y)); //den roten wert setzen
			}
		}
	   }
	   //global_preview_bitmap.Create();
		global_sediment_bitmap = wxBitmap(temp_img,-1);
}




	
//#----------------------------------------------------------------------

void Water_Simulation::Water_Sim2()
{
	double avg_a;
	double a;
	double delta_w;
	double d_tot;
	double m_max;
	double delta_m;
	double water_amount;
	int iterations = 100; //test
	double progress = 0;
	//double dissolve;
	Progressbar();
	
	for (int iter = 0; iter < iterations; iter++)
	{
		for (int x = 1; x < xsize - 1; x++){
			for (int y = 1; y < ysize - 1; y++){
				VEX(x,y).w += KR;//rain constant
				VEX(x,y).z -= KS * VEX(x,y).w;
				VEX(x,y).sediment_amount += KS * VEX(x,y).w;//soliubilty constant
				
				
				avg_a = Average_a(x,y);
				a =  VEX(x,y).w + VEX(x,y).z + VEX(x,y).sediment_amount;	
				d_tot = d_total(x,y);
				
				if (d_tot == 0)//keine wasserunterschiede
				{
					continue;
				}
				
				
				water_amount = min( VEX(x,y).w ,a - avg_a);
				VEX(x,y).w -= water_amount;
				for (int i = -1; i <= 1; i++){
					for (int j= -1; j <= 1; j++)
					{
						if (i != 0 || j != 0)
						{

							delta_w =  water_amount * ( a -  ( VEX(x+i,y+j).z + VEX(x+i,y+j).w + VEX(x+i,y+j).sediment_amount) ) / d_tot;
							VEX(x+i,y+j).w += delta_w; //d1 = delta water map

							//dissolve =2.0 * ( VEX(x+i,y+i).w + VEX(x+i,y+i).z)  * water_amount;
				
							VEX(x+i,y+j).sediment_amount += VEX(x,y).sediment_amount * delta_w / VEX(x,y).w;
							//VEX(x+i,y+j).sediment_amount += VEX(x+i,y+j).w * VEX(x,y).sediment_amount / d_tot + dissolve;
							//VEX(x+i,y+j).z += VEX(x+i,y+j).sediment_amount;

						}
					}
				}

				VEX(x,y).w -= min(VEX(x,y).w, a - avg_a );

				VEX(x,y).w *= (1-KE); //evaporation


				m_max = KC * VEX(x,y).w; //maximum sediment amount (=capacity)

				delta_m = max(0, VEX(x,y).sediment_amount - m_max ); // amount exceeding maximum amount is deposited
				VEX(x,y).sediment_amount -= delta_m;
				VEX(x,y).z += delta_m;
			}
		}
				

		if (iter % 5 == 0)
		{
			if ( frame->water_preview_on)   { visualize_Water2D(); }
			if ( frame->sediment_preview_on) { visualize_Sediment2D(); }
			frame->ManualRender();
		}
		
		progress += 100.0/double(iterations); //(die endzahl muss 100 sein
		dialog->Update(int(progress) ); 
	}
	dialog->Destroy();

}

double inline Water_Simulation::Average_a(int x,int y)
{
	double total_height = 0;
	int cells = 1; //1 = die x,y zelle
	double height = 0;
	for (int i = -1; i <= 1; i++){
		for (int j= -1; j <= 1; j++)
			{
				if (i != 0 || j != 0)
				{
					height = VEX(x+i,y+i).z + VEX(x+i,y+i).w + VEX(x+i,y+i).sediment_amount;
					if (height > 0)
					{
						total_height += height;
						cells++;
					}
				}
		}
	}
	return total_height/double(cells); //averag height = summe der positiven zellen / anzahl der positiven zellen


	

}

double inline Water_Simulation::d_total(int x, int y)
{
	double d_tot = 0;
	double a = VEX(x,y).z + VEX(x,y).w + VEX(x,y).sediment_amount;
	double d_ij;
	for (int i = -1; i <= 1; i++){
		for (int j= -1; j <= 1; j++)
			{
				if (i != 0 || j != 0)
				{
					d_ij = (a- VEX(x+i,y+j).z + VEX(x+i,y+j).w + VEX(x+i,y+j).sediment_amount);
					if (d_ij > 0)
					{
						d_tot += d_ij;
					}
				}
		}
	}
	return d_tot;
}



void Water_Simulation::Water_Sim3()
{

	int rain_freq = 1;
	Progressbar();
	int iterations = 100; //test
	double progress = 0.0;



		for (int iter = 0; iter < iterations; iter++) {

			// rain
			if (iter%rain_freq == 0) {
				for (int x = 0; x < xsize; x++){
					for (int y = 0; y < ysize; y++){
						VEX(x,y).w += KR;
					}
				}
			}
			double h,h1,h2,h3,h4,d1,d2,d3,d4;
			double total_height = 0;
			double total_height_diff = 0;
			double avr_height,water_amount,water_inv,sediment_amount,sediment_inv,dissolve;
			// water and sediment transport
				for (int x = 1; x < xsize-1; x++){
					for (int y = 1; y < ysize-1; y++){

					// calculate total heights and height differences
					h = VEX(x,y).z + VEX(x,y).w;

					h1 = VEX(x,y+1).z +  VEX(x,y+1).w +  VEX(x,y+1).sediment_amount;
					h2 = VEX(x-1,y).z +  VEX(x-1,y).w +  VEX(x-1,y).sediment_amount;
					h3 = VEX(x+1,y).z +  VEX(x+1,y).w +  VEX(x+1,y).sediment_amount;
					h4 = VEX(x,y-1).z +  VEX(x,y-1).w +  VEX(x,y-1).sediment_amount;

					d1 = h - h1;
					d2 = h - h2;
					d3 = h - h3;
					d4 = h - h4;

					// calculate amount of water and sediment to transport
					total_height = 0;
					total_height_diff = 0;
					int cells = 1;

					if (d1 > 0) {
						total_height_diff+= d1;
						total_height+= h1;
						cells++;
					}
					if (d2 > 0) {
						total_height_diff+= d2;
						total_height+= h2;
						cells++;
					}
					if (d3 > 0) {
						total_height_diff+= d3;
						total_height+= h3;
						cells++;
					}
					if (d4 > 0) {
						total_height_diff+= d4;
						total_height+= h4;
						cells++;
					}

					if (cells == 1) {
						continue;
					}

					avr_height = total_height/cells;

					water_amount = min(VEX(x,y).w, h - avr_height);
					VEX(x,y).d1 -= water_amount;
					water_inv = water_amount/total_height_diff;

					sediment_amount = VEX(x,y).sediment_amount;
					VEX(x,y).sediment_amount1 -= sediment_amount;
					sediment_inv = sediment_amount/total_height_diff;

					// transport water and sediment and dissolve more material
					if (d1 > 0) {
						VEX(x,y+1).d1 +=  d1*water_inv;
						dissolve = d1*water_amount;
						VEX(x,y+1).sediment_amount1 += d1*sediment_inv + dissolve;
						VEX(x,y+1).z -= dissolve;
					}
					if (d2 > 0) {
						VEX(x-1,y).d1 +=  d1*water_inv;
						dissolve = d1*water_amount;
						VEX(x-1,y).sediment_amount1 += d1*sediment_inv + dissolve;
						VEX(x-1,y).z -= dissolve;		
					}
					if (d3 > 0) {
						VEX(x+1,y).d1 +=  d1*water_inv;
						dissolve = d1*water_amount;
						VEX(x+1,y).sediment_amount1 += d1*sediment_inv + dissolve;
						VEX(x+1,y).z -= dissolve;							}
					if (d4 > 0) {
						VEX(x,y-1).d1 +=  d1*water_inv;
						dissolve = d1*water_amount;
						VEX(x,y-1).sediment_amount1 += d1*sediment_inv + dissolve;
						VEX(x,y-1).z -= dissolve;
					}
				}
			}

			// apply changes to water map
			for (int x = 1; x < xsize-1; x++){
				for (int y = 1; y < ysize-1; y++){
					VEX(x,y).w += VEX(x,y).d1;

					// apply changes to sediment map
					VEX(x,y).sediment_amount += VEX(x,y).sediment_amount1;

					// apply changes to height map
					//channel.channelAddNoClip(height_map_diff);

					// water vaporization
					//VEX(x,y).w *= (1-KE);

					// sedimentation
					VEX(x,y).sediment_amount1 = VEX(x,y).sediment_amount - VEX(x,y).w;
					VEX(x,y).sediment_amount -= VEX(x,y).sediment_amount1 ;
					VEX(x,y).z += VEX(x,y).sediment_amount1;

					// clear diff maps
					VEX(x,y).sediment_amount1 = 0;
					VEX(x,y).d1 = 0;
				}
			}

		if (iter % 5 == 0)
		{
			if ( frame->water_preview_on)   { visualize_Water2D(); }
			if ( frame->sediment_preview_on) { visualize_Sediment2D(); }
			frame->ManualRender();
		}
		
		progress += 100.0/double(iterations); //(die endzahl muss 100 sein
		dialog->Update(int(progress) ); 
	}
	dialog->Destroy();



	}

//-----------------------------------------------------------------------------------------





void  Water_Simulation::Water_Sim4()
{
int xsize = Alti.getxsize();
int ysize = Alti.getysize();
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
int iterations = Iterations;

// Start iterations...
double waterAmount;
double sedimentAmount;
double erodedSediment;
double dtotal = 0.0;
double dMax = 0.0;
double waterAtIndex;
double waterAtPoint;
double hCumulative;

Progressbar();

if (Water_source_on)
{
	Distribution(WATER_AMOUNT);
}





for (int iter = 0; iter < iterations; iter++) {
  

	if (Water_source_on)
	{
		for (y = 0; y < ysize; y++) {
                for (x = 0; x < xsize; x++) {
					waterAmount = VEX(x,y).w +  VEX(x,y).Waterdistribution;
					if (waterAmount > 1.0) {
                                waterAmount = 1.0;
                        }
					VEX(x,y).w = waterAmount;
				}
		}
	}
	if (Rain_on)
	{
        for (y = 0; y < ysize; y++) {
                for (x = 0; x < xsize; x++) {
                        waterAmount = VEX(x,y).w + KR;
                        if (waterAmount > 1.0) {
                                waterAmount = 1.0;
                        }
                        VEX(x,y).w = waterAmount;
                }
        }
	}
        // Material als Sediment auflösen
        for (y = 0; y < ysize; y++) {
                for (x = 0; x < xsize; x++) {
                        sedimentAmount = VEX(x,y).sediment_amount;
                        maxSediment = VEX(x,y).w * KS;
                        if (sedimentAmount < maxSediment) {
                                erodedSediment = VEX(x,y).w * KS;
                                if (sedimentAmount + erodedSediment > maxSediment) {
                                        erodedSediment = maxSediment - sedimentAmount;
                                }
                                heightAtIndex = VEX(x,y).z;
                                if (erodedSediment > heightAtIndex) {
                                        erodedSediment = heightAtIndex;
                                }
                                VEX(x,y).sediment_amount = sedimentAmount + erodedSediment;
                                VEX(x,y).z = heightAtIndex - erodedSediment;
                        }
                }
        }
        // Höhenunterschiede berechnen und Wasser bewegen
        for (y = 0; y < ysize; y++) {
                // y...
                if (y == 0) {
                        yNeighbours = 2;
                        yShift = 0;
                        yIndex = 0;
                } else if (y == ysize - 1) {
                        yNeighbours = 2;
                        yShift = -1;
                        yIndex = 1;
                } else {
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
                        } else if (x == xsize - 1) {
                                xNeighbours = 2;
                                xShift = -1;
                                xIndex = 1;
                        } else {
                                xNeighbours = 3;
                                xShift = -1;
                                xIndex = 1;
                        }
                        // Höhenunterschied kalkulieren
                        dtotal = 0.0f;
						dMax = 0.0f;
                        heightAtIndex = VEX(x,y).z; // Get height at index
                        waterAtIndex = VEX(x,y).w; // Get water at index
                        waterAtPoint;
                        hCumulative = heightAtIndex;
                        nNeighbours = 0;
                        
						
						for (Ny = 0; Ny < yNeighbours; Ny++) {
                                for (Nx = 0; Nx < xNeighbours; Nx++) {
									/*Wenn zelle nicht die Indexzelle ist*/
									if (Nx != xIndex || Ny != yIndex) {
                                        heightAtPoint = VEX(x + Nx + xShift, y + Ny + yShift).z; // Get height at point
                                        waterAtPoint = VEX(x + Nx + xShift, y + Ny + yShift).w; // Get water at point
                                        d = (heightAtIndex + waterAtIndex) - (heightAtPoint + waterAtPoint);
                                        if (d > 0) {
											//summe der positiven d's
                                            dtotal += d;
                                            hCumulative += heightAtPoint + waterAtPoint;
											// anzahl der "abfluss"nachbarn
                                            nNeighbours++;
                                            if (d > dMax) {
                                                    d = dMax;
                                            }
										}
                                    }
                                }
						}
                        //Wasser,dass von Indexzelle abfließt berechnen
                        double hAverage = hCumulative / (nNeighbours + 1);
                        double dWater = (heightAtIndex + waterAtIndex) - hAverage;
                        double transportedWater = min(waterAtIndex, dWater);
						//In die Difference Map schreiben
                        VEX(x,y).d1 -= transportedWater;
                        //Sediment,dass von Indexzelle abfließt berechnen
                        double transportedSediment = VEX(x,y).sediment_amount * (transportedWater / waterAtIndex);
                        VEX(x, y).sediment_amount1 -= transportedSediment;
                        
						// Nachbarn
                        for (Ny = 0; Ny < yNeighbours; Ny++) {
                                for (Nx = 0; Nx < xNeighbours; Nx++) {
									if (Nx != xIndex || Ny != yIndex) {											
										heightAtPoint = VEX(x + Nx + xShift, y + Ny + yShift).z; // Get height at point
										waterAtPoint = VEX(x + Nx + xShift, y + Ny + yShift).w; // Get water at point
										d = (heightAtIndex + waterAtIndex) - (heightAtPoint + waterAtPoint);
										//Nur tiefere Zellen betrachten
										if (d > 0) {
											//Wasser zur Nachbarzelle bewegen
											VEX(x + Nx + xShift, y + Ny + yShift).d1 +=  transportedWater * (d / dtotal);
											//Sediment zur Nachbarzelle bewegen
											VEX(x + Nx + xShift, y + Ny + yShift).sediment_amount1 += transportedSediment * (d / dtotal);
										}
                                       }
                                }
                        }
                }
        }
        //Wasser difference Map auf die Wasserkarte anwenden
        double waterAtCell;
        for (y = 0; y < ysize; y++) {
                for (x = 0; x < xsize; x++) {
                        waterAtCell = VEX(x, y).w + VEX(x, y).d1;
                        // Verdunstung berechnen
                        double evaporatedWater = waterAtCell * KE;
                        waterAtCell -= evaporatedWater;
                        if (waterAtCell < 0.0) {
                                waterAtCell = 0.0;
                        }
                        VEX(x, y).w = waterAtCell;
                        VEX(x, y).d1 = 0.0;
                }
        }
        // Sediment difference Map auf die Sedimentkarte anwenden
        for (y = 0; y < ysize; y++) {
                for (x = 0; x < xsize; x++) {
                        sedimentAtCell = VEX(x,y).sediment_amount + VEX(x,y).sediment_amount1;
                        if (sedimentAtCell > 1.0) {
                                sedimentAtCell = 1.0;
                        } else if (sedimentAtCell < 0.0) {
                                sedimentAtCell = 0.0;
                        }
                        VEX(x,y).sediment_amount = sedimentAtCell;
                        VEX(x,y).sediment_amount1 = 0.0;
                }
        }
        // Sediment ablegen
        for (y = 0; y < ysize; y++) {
                for (x = 0; x < xsize; x++) {
                        maxSediment = VEX(x, y).w * KS;
                        sedimentAtCell = VEX(x,y).sediment_amount;
                        if (sedimentAtCell > maxSediment) {
                                double depositedSediment = sedimentAtCell - maxSediment;
                                VEX(x,y).sediment_amount = maxSediment;
                                heightAtCell = VEX(x,y).z;
                                VEX(x,y).z = heightAtCell + depositedSediment;
                        }
                }
        }
        // Update progress...
        double percentComplete = (double) iter / (double) iterations;
		dialog->Update(int(percentComplete * 100) ); 

		if (iter % 5 == 0)
		{
			if ( frame->water_preview_on)   { visualize_Water2D(); }
			if ( frame->sediment_preview_on) { visualize_Sediment2D(); }
			frame->ManualRender();
		}
}
dialog->Destroy();
    }
