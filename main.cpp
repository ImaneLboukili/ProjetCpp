//==============================
//    INCLUDES
//==============================

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>

using namespace std;

#include "Environnement.h"
#include "Case.h"
#include "Cell.h"
#include "CellA.h"
#include "CellB.h"

#define couleur(param) printf("\033[%sm",param)

void Rdiagram(int Tmin, int Tmax, float Amin, float Amax, int Pt, float Pa, float Pmut, float D);
void diagram(int Tmin, int Tmax, int Amin, int Amax, int Pt, int Pa, float Pmut);
void simple_run(int t, float A, int T, float D, float Pmut);


//diagramme de classes : destructeur case et env , attributs env cA et cB
//faire discret pour pmut et pas pumt, faire joli simul pour tout
//faire varier diffusion
//enlever affichage temps
//mettre meme conditions limites partout et virer les autres
//convlu latex

//==============================
//    MAIN
//==============================

int main(int argc, char const *argv[])
{   
	float Pmut = 0;
	float D = 0.1;
	/*simple_run(10000, 45 , 1200, D,0.001);
	system("Rscript Chroniques.R Chroniques.txt Chroniques.jpeg");*/
	
	
	Rdiagram(1,1201,0,50,200,5,Pmut,D);
	system("Rscript Plot_heatmap_p0.R Rdiagram.txt out.pdf");
	//system("Rscript Plot_heatmap_p.R Rdiagram.txt out.pdf");
    return 0;
}

//==============================
//    FUNCTIONS
//==============================
/**
 * Display a simulation with values of Ainit, T and D passed in parameters, that lasts the time t
 * We can see the time evolution of the grid, with the genotypes A in blue, and B in red.
 */
void simple_run(int t, float A, int T, float D,float Pmut){
	Environnement env = Environnement(A,T,D,Pmut);
	env.run(t);
}

/**
 * Computes the final state of the simulations for each Ainit and T and write them in a file 
 * Parameters are the extremal values of the diagram axis, and the step for each one.
 * Mutation probability and diffusion constant are also asked
 */
void Rdiagram(int Tmin, int Tmax, float Amin, float Amax, int Pt, float Pa,float Pmut, float D){
	ofstream file("Rdiagram.txt", ios::out | ios::trunc);
	if(file){  
		int cpt = 0;
		file << "Ainit " << "T " << "val" << endl;
		int nb_it = (Tmax-Tmin+Pt)/Pt*(Amax-Amin+Pa)/Pa;
		for(int T=Tmin; T<=Tmax; T+=Pt){
			for (float A=Amin; A<=Amax; A+=Pa){
				cpt++;
				cout << " Simulation " << cpt << " / " << nb_it << ", Ainit = " << A << " T = " << T;				
				Environnement env = Environnement(A,T,D,Pmut);
				float result = env.run_diagram(2000);
				file << A << " " << T << " " << result << endl;
				if (result==0){
					couleur("30");
					printf("			Extinction \n");
				}
				else if (result==1){
					couleur("31");
					printf("			Selection \n");
				}
				else{
					couleur("32");
					printf("			Cohabitation \n");
				}
				couleur("0");
			}
		}
		file.close();		
	}	
	else{
		cerr << "File opening error !" << endl;
	}
}
	
/**
 * Computes several runs with differents values of Ainit and T to return a state diagram
 * in those 2 dimensions.
 * The output is an array wich axis (displayed in white) are the values taken by Ainit and T for the different runs.
 * This array is filled with the number of different genotypes (0,1 or 2) living after a simulation time (1000) 
 */
void diagram(int Tmin, int Tmax, int Amin, int Amax, int Pt, int Pa,float Pmut){
	int** diagram;
	//creation of the array, with an additionnal dimension to store the diagram axis
	diagram  = new int* [(Tmax-Tmin)/Pt+2];
	for(int i=0; i<(Tmax-Tmin)/Pt+2;i++){
		diagram[i] = new int[(Amax-Amin)/Pa+2];
	}
	
	//Filling of the axis with parameters values
	for(int i=0; i<(Tmax-Tmin)/Pt+1; i++){
		diagram[i][0]=Tmax-i*Pt;
		for(int j=0; j<(Amax-Amin)/Pa+1; j++){
			diagram[(Tmax-Tmin)/Pt+1][j+1]=Amin+j*Pa;
		}
	}
	
	int nb_it = (Tmax-Tmin+Pt)/Pt*(Amax-Amin+Pa)/Pa;
	//Points of the diagram
	int cpt = 0;
	for(int T=Tmin; T<=Tmax; T+=Pt){
		for (int A=Amin; A<=Amax; A+=Pa){
			cpt++;
			cout << " Simulation " << cpt << " sur " << nb_it << ", Ainit = " << A << " et T = " << T;
			Environnement env = Environnement(A,T,0.1,Pmut);
			diagram[(Tmax-T)/Pt][(A-Amin)/Pa+1]=env.run_diagram(1000);
			if (diagram[(Tmax-T)/Pt][(A-Amin)/Pa+1]==0){
				cout << ":		Extinction" << endl;
			}
			else if (diagram[(Tmax-T)/Pt][(A-Amin)/Pa+1]==1){
				cout << ":		Selection" << endl;
			}
			else{
				cout << ":		Cohabitation" << endl;
			}
			
			//cout << "*************************************************************Changement de paramètres" << endl;
		}
	}
	//display
	for (int i=0; i<(Tmax-Tmin)/Pt+2; i++){
		for(int j=0; j<(Amax-Amin)/Pa+2; j++){
			if(j!=0 && i!=(Tmax-Tmin)/Pt+1){
				//changes the color of the value with the final state, for a better visualization
				if(diagram[i][j]==0){
					couleur("30");
					printf("0  ");
				}
				if(diagram[i][j]==1){
					couleur("31");
					printf("1  ");
				}
				if(diagram[i][j]==2){
					couleur("32");
					printf("2  ");
				}
				couleur("0");
			}
			//alignement
			else{
				cout << diagram[i][j] << " ";
				if(diagram[i][j]<10 && j==0){
					cout << " ";
				}
				if(diagram[i][j]<100 && j==0){
					cout << " ";
				}
				if(diagram[i][j]<1000 && j==0){
					cout << " ";
				}
				if(diagram[i][j]<10 && i==(Tmax-Tmin)/Pt+1){
					cout << " ";
				}
			}
		}
		cout << endl;
	}
	delete[] diagram;	
}


