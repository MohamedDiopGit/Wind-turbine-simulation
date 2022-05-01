#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/* Variables globales */
#define LINE_LENGTH 100  // Longueur max. d'une ligne du fichier

/* Délaration des prototypes */
int speed_calculation(int display_info,int speed_state,float V_i[]);
int simulation_windturbine();
void average_production(int start_period,int end_period);
void minimum_production(int start_period,int end_period);
void maximum_production(int start_period,int end_period);
void full_production(int start_period,int end_period);
int length_simulation();
void select_menu();

/* Déclaration des fonctions */

// CALCUL L'ETAT DE VITESSE AU JOUR SUIVANT

int speed_calculation(int display_info,int speed_state, float V_speed[]){  //V_speed : vecteur des probabilités correspondant à l'état de vitesse 0,1 ou 2
	// Distribution des probabilités
	int p1 = (int) 100*V_speed[0];
	int p2 = (int) 100*V_speed[1];
	int p3 = (int) 100*V_speed[2];

	// Distribution des intervalles
	int interval_limit1 = p1 - 0;
	int interval_limit2 = p2 + interval_limit1;
	int interval_limit3 = interval_limit2 + p3;   // égal 100

	// Génération d'un nombre alétoire etre 0 et 100
	int randomValue = (int)(rand() / (double)RAND_MAX * (100 - 1));
	
	// Affichage des informations des calculs
	if(display_info == 1){
		printf("Speed transition possibility = [%.2f %.2f %.2f] \n",V_speed[0],V_speed[1],V_speed[2]);
		printf("Interval's distribution :[%d %d %d] \n",interval_limit1,interval_limit2,interval_limit3);
		printf("randomValue = %d\n \n",randomValue);
	}
	
	// Evaluation de l'état de vitesse
	if(randomValue <= interval_limit1){
		return 0;
	}
	else if(randomValue > interval_limit1 && randomValue <= interval_limit2){
		return 1;
	}
	else if(randomValue >interval_limit2 && randomValue <= interval_limit3){
		return 2;
	}
}

// SIMULATION DU PARC EOLIEN

int simulation_windturbine(){
	printf("\n----- Windturbine's Simulation -----\n\n");
	
	// Set and initialisation of data
	
	printf("Choose your length's simulation (in days): ");
	int length_simulation;
	scanf("%d",&length_simulation);
	
	printf("Initial speed state (0,1,2) : ");
	int speed_state;   
	scanf("%d",&speed_state);
	if(speed_state > 2){
		printf("Invalid speed state, please restart.\n");
		return 0;
	}
	
	printf("Display site's production each day ? (1=yes/0=no): ");
	int display_info;
	scanf("%d",&display_info);
	
	printf("Matrix V initialisation ...");
	float V[3][3] = {{0.2,0.73,0.07},{0.11,0.74,0.15},{0.04,0.61,0.35}};
	printf("Finished\n");
	
	srand (time (NULL));   // Initialisation du SRAND pour le rand()
	
	printf("Vector production initialisation ...");
	float prod[3] = {0.0,0.4,0.65};
	printf("Finished\n");

	// Estimation de la production d'une éolienne
	float total_production = 0;
	FILE *fichier;
	fichier = fopen("Production eolienne.txt","w");
	if(fichier != NULL){
		
		fprintf(fichier,"Day | Speed_State | Daily_production | Total_Production\n");
		for(int day=1; day<=length_simulation; day++){
			float production_day = 20*prod[speed_state];    // Production du parc
			total_production += production_day;
			fprintf(fichier,"%d %d %.2f %.2f\n",day,speed_state,production_day,total_production);
			
			if(display_info == 1){
				printf("Day = %d \n",day);
				printf("Speed state : %d\n",speed_state);
				printf("Daily production = %.2f MW\n",production_day);
				printf("Total production = %.2f MW\n",total_production);
			}
		 	
		 	
			//Nouveau calcul de l'état de vitesse
			if(day == length_simulation){
				printf("End of simulation - File Created\n");
			}
			else
				speed_state = speed_calculation(display_info,speed_state,V[speed_state]);  
		}
		
		fclose(fichier);
	}
	if(display_info == 1){
		printf("Total production on %d days is estimated at %.2f MW for the site.\n \n",length_simulation,total_production);
	}
}

// OPTION DU MENU :

void select_menu(){
	// INITIALISATIO VARIABLES
	int perf_metric=99;  //Initialisation condition affichage menu
	int start_period,end_period;   // Variables pour la sélection d'un intervalle de mesures
	int length_sim = length_simulation();  // Durée de la simulation en jours
	int N_selection = 4;  //Nombre d'option du menu
	
	// CONDITION D'ARRET AFFICHAGE MENU
	while(perf_metric !=0){
	
		// AFFICHAGE ET SELECTION DU MENU
		printf("\n---- Performance metric menu ----\n");
		
		printf("0: Quit\n1: Average prod.\n2: Minimum prod.\n3: Maximum prod.\n4: Full production\n");
		printf("The length of simulation is %d days\n",length_sim);
		printf("Select your performance metric or quit: ");
		scanf("%d",&perf_metric);
		while(perf_metric > N_selection){
			printf("The metric choice must be inferior to %d. Please retry: ",N_selection);
			scanf("%d",&perf_metric);
		}

		
		// Choix entre intervalle de la simulation ou de toute la simulation
		int time_select; //CHOIX ENTRE INTERVALLE OU SIMULATION ENTIERE
		if(perf_metric != 0){
			printf("Select an interval time or entire simulation? (1=interval/0=entire): ");
			scanf("%d",&time_select);
		}
		
		// CHOIX INTERVALLE : Selection de la période de mesure
		if(perf_metric !=0 && time_select == 1){
			printf("Select your interval time in days (start end): ");
			scanf("%d %d",&start_period,&end_period);
			
			// Evite les erreurs de frappe
			if(start_period > end_period){
				printf("Did you mean start = %d and end = %d ?(1=yes/0=no): ",end_period,start_period);
				int change_select = 0;
				scanf("%d",&change_select);
				if(change_select == 1){
					int stock_value = end_period;
					end_period = start_period;
					start_period = stock_value;
				}
			}
			printf("\n");
		}	
		
		// CHOIX DE TOUTE LA SIMULATION	
		else if(perf_metric !=0 && time_select == 0){
			start_period = 1;
			end_period = length_sim;
		}
		
		//ARRET DU MENU SI MAUVAIS CHOIX SUR L'INTERVALLE
		if(end_period > length_sim || start_period > end_period){
			printf("Wrong selection: Respect the length of simulation. Please retry.\n");
			perf_metric = 0;
		}
		else if(start_period < 0 || end_period < 0){
			printf("Wrong selection: start/end must be positive integers. Please retry.\n");
			perf_metric = 0;
		}
		
		// CHOIX DU MENU
		if(perf_metric == 1){
			average_production(start_period,end_period);
		}
		else if(perf_metric == 2){
			minimum_production(start_period,end_period);
		}
		else if(perf_metric == 3){
			maximum_production(start_period,end_period);
		}
		else if(perf_metric == 4){
			full_production(start_period,end_period);
		}
	}
}

// Durée d'une simulation

int length_simulation(){
	FILE *fichier;
	fichier = fopen("Production eolienne.txt","r");
	int day = 0;
	if(fichier != NULL){
		char chain[LINE_LENGTH]; // l'arrêt de lecture d'une ligne (longueur max.)
		//Affichage en-tête des données
		fgets(chain,LINE_LENGTH,fichier);
		
		//Affichage et récupération des données
		while(fgetc(fichier) != EOF){    // Condition Fin du fichier (end of file)
			//Organisation des données
			fseek(fichier,-1,SEEK_CUR);  //Retour au caractère précèdent due à fgetc
			//printf("%ld ",ftell(fichier));  // Position du curseur
			
			//Récupération des données : [DAY SPEED_STATE DAILY_PROD TOTAL_PROD]
			fscanf(fichier,"%d",&day);
		
			//Utilisation de la fonction : saut ligne par ligne
			fgets(chain,LINE_LENGTH,fichier);
		}
		fclose(fichier);
	}
	return day;
}
void average_production(int start_period,int end_period){
	FILE *fichier;
	fichier = fopen("Production eolienne.txt","r");
	
	if(fichier != NULL){
		char chain[LINE_LENGTH]; // l'arrêt de lecture d'une ligne (longueur max.)
		int day;
		int speed_state;
		float daily_prod;
		float total_prod;
		float average_prod = 0;
		printf("Data recovery ...\n \n");
		
		//Affichage en-tête des données
		fgets(chain,LINE_LENGTH,fichier);
		printf("%s", chain);
		
		//Affichage et récupération des données
		while(fgetc(fichier) != EOF){    // Condition Fin du fichier (end of file)
			fseek(fichier,-1,SEEK_CUR);  //Retour au caractère précèdent due à fgetc
			//printf("%ld ",ftell(fichier));  // Position du curseur
			
			//Récupération des données : [DAY SPEED_STATE DAILY_PROD TOTAL_PROD]
			fscanf(fichier,"%d %d %f %f",&day,&speed_state,&daily_prod,&total_prod);
			printf("%d %d %.2f %.2f\n",day,speed_state,daily_prod,total_prod);	
			
			//Traitement des données adéquat : Production moyenne sur la durée voulue
			if(day >= start_period && day <= end_period){ //&& speed_state != 0){
				average_prod += daily_prod;
			}
			
			//Utilisation de la fonction saut ligne par ligne
			fgets(chain,LINE_LENGTH,fichier);
		}
		average_prod /= (end_period - start_period + 1); //Moyennage
		printf("\nAll data has been recovered.\n");
		printf("The average production between day no. %d and day no. %d is estimated at %.2f MW\n",start_period,end_period,average_prod);
		fclose(fichier);	
	}	
}

void minimum_production(int start_period,int end_period){
	FILE *fichier;
	fichier = fopen("Production eolienne.txt","r");
	
	if(fichier != NULL){
		char chain[LINE_LENGTH]; // l'arrêt de lecture d'une ligne (longueur max.)
		int day;
		int speed_state;
		float daily_prod;
		float total_prod;
		float min_prod = 0;
		printf("Data recovery ...\n \n");
		
		//Affichage en-tête des données
		fgets(chain,LINE_LENGTH,fichier);
		printf("%s", chain);
		
		//Affichage et récupération des données
		while(fgetc(fichier) != EOF){    // Condition Fin du fichier (end of file)
			fseek(fichier,-1,SEEK_CUR);  //Retour au caractère précèdent due à fgetc
			//printf("%ld ",ftell(fichier));  // Position du curseur
			
			//Récupération des données : [DAY SPEED_STATE DAILY_PROD TOTAL_PROD]
			fscanf(fichier,"%d %d %f %f",&day,&speed_state,&daily_prod,&total_prod);
			printf("%d %d %.2f %.2f\n",day,speed_state,daily_prod,total_prod);	
			if(day == start_period){  //Initialisation au premier jour sur la période
				min_prod = daily_prod;
			}
			//Traitement des données adéquat : Production moyenne sur la durée voulue
			if(day >= start_period && day <= end_period && min_prod >= daily_prod){ //&& speed_state != 0){
				min_prod = daily_prod;
			}
			
			//Utilisation de la fonction saut ligne par ligne
			fgets(chain,LINE_LENGTH,fichier);
		}
		printf("\nAll data has been recovered.\n");
		printf("The minimum production between day no. %d and day no. %d is estimated at %.2f MW\n",start_period,end_period,min_prod);
		fclose(fichier);	
	}
}

void maximum_production(int start_period,int end_period){
	FILE *fichier;
	fichier = fopen("Production eolienne.txt","r");
	
	if(fichier != NULL){
		char chain[LINE_LENGTH]; // l'arrêt de lecture d'une ligne (longueur max.)
		int day;
		int speed_state;
		float daily_prod;
		float total_prod;
		float max_prod = 0;
		printf("Data recovery ...\n \n");
		
		//Affichage en-tête des données
		fgets(chain,LINE_LENGTH,fichier);
		printf("%s", chain);
		
		//Affichage et récupération des données
		while(fgetc(fichier) != EOF){    // Condition Fin du fichier (end of file)
			fseek(fichier,-1,SEEK_CUR);  //Retour au caractère précèdent due à fgetc
			//printf("%ld ",ftell(fichier));  // Position du curseur
			
			//Récupération des données : [DAY SPEED_STATE DAILY_PROD TOTAL_PROD]
			fscanf(fichier,"%d %d %f %f",&day,&speed_state,&daily_prod,&total_prod);
			printf("%d %d %.2f %.2f\n",day,speed_state,daily_prod,total_prod);	
			
			//Traitement des données adéquat : Production maximale sur la durée voulue
			if(day >= start_period && day <= end_period && max_prod <= daily_prod){ //&& speed_state != 0){
				max_prod = daily_prod;
			}
			
			//Utilisation de la fonction saut ligne par ligne
			fgets(chain,LINE_LENGTH,fichier);
		}
		printf("\nAll data has been recovered.\n");
		printf("The maximum production between day no. %d and day no. %d is estimated at %.2f MW\n",start_period,end_period,max_prod);
		fclose(fichier);	
	}
}
void full_production(int start_period,int end_period){
	FILE *fichier;
	fichier = fopen("Production eolienne.txt","r");
	
	if(fichier != NULL){
		char chain[LINE_LENGTH]; // l'arrêt de lecture d'une ligne (longueur max.)
		int day;
		int speed_state;
		float daily_prod;
		float total_prod;
		float full_prod = 0;
		printf("Data recovery ...\n \n");
		
		//Affichage en-tête des données
		fgets(chain,LINE_LENGTH,fichier);
		printf("%s", chain);
		
		//Affichage et récupération des données
		while(fgetc(fichier) != EOF){    // Condition Fin du fichier (end of file)
			fseek(fichier,-1,SEEK_CUR);  //Retour au caractère précèdent due à fgetc
			//printf("%ld ",ftell(fichier));  // Position du curseur
			
			//Récupération des données : [DAY SPEED_STATE DAILY_PROD TOTAL_PROD]
			fscanf(fichier,"%d %d %f %f",&day,&speed_state,&daily_prod,&total_prod);
			printf("%d %d %.2f %.2f\n",day,speed_state,daily_prod,total_prod);	
			
			//Traitement des données adéquat : Production maximale sur la durée voulue
			if(day >= start_period && day <= end_period){ //&& speed_state != 0){
				full_prod += daily_prod;
			}
			
			//Utilisation de la fonction saut ligne par ligne
			fgets(chain,LINE_LENGTH,fichier);
		}
		printf("\nAll data has been recovered.\n");
		printf("The full production between day no. %d and day no. %d is estimated at %.2f MW\n",start_period,end_period,full_prod);
		fclose(fichier);	
	}
}
void main(){
	printf("Launch a wind turbine simulation ?(1=yes/0=no): ");
	int select_simul;
	scanf("%d",&select_simul);
	
	if(select_simul == 1){
		simulation_windturbine();
	}
	else
		printf("No simulation selected\n");
	select_menu();
}