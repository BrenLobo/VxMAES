/*
 * VerificationSystem.c
 *
 *  Created on: Nov 1, 2023
 *      Author: blobo
 */
#include <vxWorks.h>
#include "VxMAES.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>  

/**********************************************/
/*			    Enums and structs			  */
/**********************************************/
//Type of parameter
typedef enum actual_type
{
	WATER,
	OIL,
	TIRE
}actual;

// let boot parameter
typedef struct letboot
{
	bool pass;
	actual type;
	float DataReady;
}bootable;

/**********************************************/
/* 		  Defining the app's variables.       */
/**********************************************/
MAESAgent water_temp, oil_pressure, tire_pressure, inspector;
Agent_Platform AP;
sysVars env;
CyclicBehaviour ins_behaviour, oil_behaviour, tire_behaviour,water_behaviour;
Agent_Msg water_msg, oil_msg, tire_msg, ins_msg;
bootable water_pass, oil_pass, tire_pass,*data;
char content[80];
MAESArgument nose;//revisar
float min, max, value;

/**********************************************/
/*   Function related to each measure agent   */
/**********************************************/
// setup
void meaSetup(CyclicBehaviour * Behaviour, MAESArgument taskParam) {
	Behaviour->msg->Agent_Msg(Behaviour->msg);
	Behaviour->msg->add_receiver(Behaviour->msg, inspector.AID(&inspector));
}

// action
void Meas_Action(CyclicBehaviour * Behaviour, MAESArgument taskParam) {
	Agent_info informacion = AP.get_Agent_description(AP.get_running_agent(&AP));
	printf("\n Agente en ejecucion de medicion: %s \n",informacion.agent_name);
	data = (bootable*)taskParam;
	int num =(int)data->type;
	srand((unsigned int)time(NULL));
	switch (num) {
	case WATER: ///temp agua
		min = 85.0;//centi
		max = 100.0;//centi
		
		value = (float)(rand() % 71 + 50);
		if (value<min || value>max) {
			snprintf(content, 80, "\r\n Check water temperature!!! Normal state 85-100 C, now is in %f\n", value);
			water_pass.pass = false;
		}
		else
		{
			snprintf(content, 80, "\r\n Water measurement: %f\r", value); 
			water_pass.pass = true;
		}
		water_pass.DataReady = value;
		break;
	
	case OIL:  //presion aceite
		min = 40.0;//psi
		max = 55.0;//psi
		value = (float)(rand() % 51 + 30); // N-M+1 +  M  Nmax
		
		if (value<min || value>max) {
			if (value < min){}
			snprintf(content, 80, "\r\n Check oil pressure!!! Normal state 40-55 psi, now is in %f\n", value);
			oil_pass.pass = false;
		}
		else{
			snprintf(content, 80, "\r\n Oil measurement: %f\r", value);
			oil_pass.pass = true;
		
		}
		oil_pass.DataReady = value;
		break;

	case TIRE: //Tire presion llantas aire freno
		min = 60.0;//psi
		max = 125.0;//psi
		value = (float)(rand() % 111 + 40);
		if (value<min || value>max) {
			snprintf(content, 80, "\r\n Check tire pressure !!! Normal state 60-125 psi, now is in %f\n", value);
			tire_pass.pass = false;
		}
		else {
			snprintf(content, 80, "\r\n Tire measurement: %f\r", value);
			tire_pass.pass = true;
		}
		tire_pass.DataReady = value;
		break;

	default:
		snprintf(content, 80, "\r\n Not understood");
		printf(content);
		break;
	
	}

	Behaviour->msg->set_msg_content(Behaviour->msg, content);
	Behaviour->msg->set_msg_type(Behaviour->msg, INFORM);
	Behaviour->msg->sendAll(Behaviour->msg);	
}

/**********************************************/
/* Action function related to inspector agent */
/**********************************************/
// setup
void Verification_Setup(CyclicBehaviour* Behaviour, MAESArgument taskParam){
	Behaviour->msg->Agent_Msg(Behaviour->msg);
	Behaviour->msg->add_receiver(Behaviour->msg, water_temp.AID(&water_temp));
	Behaviour->msg->add_receiver(Behaviour->msg, oil_pressure.AID(&oil_pressure));
	Behaviour->msg->add_receiver(Behaviour->msg, tire_pressure.AID(&tire_pressure));
}

// action
void Verification_SysAction(CyclicBehaviour* Behaviour, MAESArgument taskParam) {
	printf("\n........Inicializando arranque sistema........\n");
	while (true) {
		if ((AP.get_state(&AP, water_temp.AID(&water_temp)) == SUSPENDED) && (AP.get_state(&AP, oil_pressure.AID(&oil_pressure)) == SUSPENDED) && (AP.get_state(&AP, tire_pressure.AID(&tire_pressure)) == SUSPENDED)){
			if ((water_pass.pass == true) && (oil_pass.pass == true) && (tire_pass.pass == true)) {
				printf("\n  Verificacion del sistema completado   \n");
				printf("\n........Inicializando arranque sistema2........\n");
				break;
			} else {
				printf("\n-------------Adjust Systems ---------------\n");		
				break;
			}
		} 
		else{
//		if ((AP.get_state(&AP, water_temp.AID(&water_temp)) != SUSPENDED) || (AP.get_state(&AP, oil_pressure.AID(&oil_pressure)) != SUSPENDED) || (AP.get_state(&AP, tire_pressure.AID(&tire_pressure)) != SUSPENDED)){
			Behaviour->msg->receive(Behaviour->msg, WAIT_FOREVER);
			if (Behaviour->msg->get_msg_type(Behaviour->msg) == INFORM){			
				char* mensaje = Behaviour->msg->get_msg_content(Behaviour->msg);								
				Agent_info informacion = AP.get_Agent_description(AP.get_running_agent(&AP));
				printf("\n Agente en ejecucion inspector: %s \n",informacion.agent_name);
				Behaviour->msg->suspend(Behaviour->msg, Behaviour->msg->get_sender(Behaviour->msg));
				printf("%s\n", mensaje);
//				AGENT_MODE estado =AP.get_state(&AP, water_temp.AID(&water_temp));
//				AGENT_MODE estado1 =AP.get_state(&AP, oil_pressure.AID(&oil_pressure));
//				AGENT_MODE estado2 =AP.get_state(&AP, tire_pressure.AID(&tire_pressure));
//				printf("agua: %d,aceite: %d,tire: %d .\n",estado,estado1,estado2);
			}
		}
		
	}
	water_pass.pass= false;//, oil_pass.pass, tire_pass.pass =false;
	oil_pass.pass = false;
	tire_pass.pass = false;
	AP.agent_wait(&AP, 500);
	printf("\n-------------Begin Verification---------------\n");
	Behaviour->msg->resume(Behaviour->msg, water_temp.AID(&water_temp));
	Behaviour->msg->resume(Behaviour->msg, oil_pressure.AID(&oil_pressure));
	Behaviour->msg->resume(Behaviour->msg, tire_pressure.AID(&tire_pressure));
	printf("Cycle Ended\n");
}

/**********************************************/
/*          Wrappers for each agent		      */
/**********************************************/

void WaterFunction(MAESArgument taskParam) {
	water_behaviour.msg = &water_msg;
	water_behaviour.setup = &meaSetup;
	water_behaviour.action = &Meas_Action;
	water_behaviour.execute(&water_behaviour, taskParam);
};

void OilFunction(MAESArgument taskParam) {
	oil_behaviour.msg = &oil_msg;
	oil_behaviour.setup = &meaSetup;
	oil_behaviour.action = &Meas_Action;
	oil_behaviour.execute(&oil_behaviour, taskParam);
};

void TireFunction(MAESArgument taskParam) {
	tire_behaviour.msg = &tire_msg;
	tire_behaviour.setup = &meaSetup;
	tire_behaviour.action = &Meas_Action;
	tire_behaviour.execute(&tire_behaviour, taskParam);
};

void InspectorFunction(MAESArgument taskParam) {
	ins_behaviour.msg = &ins_msg;
	ins_behaviour.setup = &Verification_Setup;
	ins_behaviour.action = &Verification_SysAction;
	ins_behaviour.execute(&ins_behaviour, taskParam);
};

void stopExecution() {
    printf("Stopping program execution after 1 minute.\n");
    exit(0); // Exiting the program
}

/**********************************************/
/* 						Main				  */
/**********************************************/
int main() {
	printf("------Boot Verification System ------ \n");
	
	time_t startTimeInSeconds = time(NULL);
	printf("%d",startTimeInSeconds);
	water_pass.pass = false;
	oil_pass.pass = false;
	tire_pass.pass = false;
	water_pass.type = WATER;
	oil_pass.type = OIL;
	tire_pass.type = TIRE;

	//Constructors for each initialized class
	ConstructorAgente(&water_temp);
	ConstructorAgente(&oil_pressure);
	ConstructorAgente(&tire_pressure);
	ConstructorAgente(&inspector);
	ConstructorSysVars(&env);
	ConstructorAgent_Platform(&AP, &env);
	ConstructorAgent_Msg(&water_msg, &env);
	ConstructorAgent_Msg(&oil_msg, &env);
	ConstructorAgent_Msg(&tire_msg, &env);
	ConstructorAgent_Msg(&ins_msg, &env);
	ConstructorCyclicBehaviour(&water_behaviour);
	ConstructorCyclicBehaviour(&oil_behaviour);
	ConstructorCyclicBehaviour(&tire_behaviour);
	ConstructorCyclicBehaviour(&ins_behaviour);

	//Initializing the Agents and the Platform.
	inspector.Iniciador(&inspector, "Inspector", 102, 100);
	water_temp.Iniciador(&water_temp, "Water Temperature", 103, 100);
	oil_pressure.Iniciador(&oil_pressure, "Oil pressure", 104, 100);
	tire_pressure.Iniciador(&tire_pressure, "Tire pressure", 105, 100);
	
	//Defining the RPT task
	TASK_ID rtpInfo=taskIdSelf();
	AP.Agent_Platform(&AP, "App Platform",rtpInfo);

	//Registering the Agents and their respective behaviour into the Platform
	AP.agent_initConParam(&AP, &water_temp, &WaterFunction,(MAESArgument)&water_pass);
	AP.agent_initConParam(&AP, &oil_pressure, &OilFunction,(MAESArgument)&oil_pass);
	AP.agent_initConParam(&AP, &tire_pressure, &TireFunction,(MAESArgument)&tire_pass);
	AP.agent_init(&AP, &inspector, &InspectorFunction);
	printf("VxMAES booted successfully \n");
	printf("Initiating APP\n\n");
	
	AP.boot(&AP);

	time_t currentTime;
	time_t elapsedTime;

	 // Calculate the start time

	while (1) {
		currentTime = time(NULL); 
		elapsedTime = currentTime - startTimeInSeconds;
		
		if (elapsedTime>20) {
//			printf("Program has run for 1 minute. Stopping the program.\n");
			break; // Exit the loop after 1 minute
		}

//		printf("Program running. Time elapsed: %d seconds.\n", elapsedTime);
		
		// Add a delay using the system tick rate
	}

//	for(;;);
//	taskDelay(40000);
	// Start the scheduler so the created tasks start executing.

	
	return 0;
}




