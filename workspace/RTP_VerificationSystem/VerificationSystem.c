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
	MAESTickType_t rate;
//	float DataReady;
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
	int min, max, value;
	Agent_info informacion = AP.get_Agent_description(AP.get_running_agent(&AP));
	printf("\n Running measurement agent: %s \n",informacion.agent_name);
	data = (bootable*)taskParam;
	char content[80];
	int num =(int)data->type;
	srand((unsigned int)time(NULL));
	switch (num) {
	case WATER: ///temp water
		min = 85;//centi
		max = 100;//centi
		
		value = (int)(rand() % 71 + 50);
		if (value<min || value>max) {
			snprintf(content, 80, "\r\n Check water temperature!!! Normal state 85-100 C, now is in %d\n", value);
			water_pass.pass = false;
		}
		else
		{
			snprintf(content, 80, "\r\n Water measurement: %d\r", value); 
			water_pass.pass = true;
		}
		break;
	
	case OIL:  //oil pressure
		min = 40;//psi
		max = 55;//psi
		value = (int)(rand() % 51 + 30); // N-M+1 +  M  Nmax
		
		if (value<min || value>max) {
			snprintf(content, 80, "\r\n Check oil pressure!!! Normal state 40-55 psi, now is in %d\n", value);
			oil_pass.pass = false;
		}
		else{
			snprintf(content, 80, "\r\n Oil measurement: %d\r", value);
			oil_pass.pass = true;
		
		}
		break;

	case TIRE: //Tire pressure
		min = 60;//psi
		max = 125;//psi
		value = (int)(rand() % 111 + 40);
		if (value<min || value>max) {
			snprintf(content, 80, "\r\n Check tire pressure !!! Normal state 60-125 psi, now is in %d\n", value);
			tire_pass.pass = false;
		}
		else {
			snprintf(content, 80, "\r\n Tire measurement: %d\r", value);
			tire_pass.pass = true;
		}
		break;

	default:
		snprintf(content, 80, "\r\n Not understood");
		printf(content);
		break;
	
	}

	Behaviour->msg->set_msg_content(Behaviour->msg, content);
	Behaviour->msg->set_msg_type(Behaviour->msg, INFORM);
	AP.agent_wait(&AP,data->rate);
	Behaviour->msg->sendX(Behaviour->msg,inspector.AID(&inspector), WAIT_FOREVER);	
	printf("The message had been send by: %s \n",informacion.agent_name);
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
	printf("\n........Initializing system boot........\n");
	while (true) {	
		if ((AP.get_state(&AP, water_temp.AID(&water_temp)) == SUSPENDED) && (AP.get_state(&AP, oil_pressure.AID(&oil_pressure)) == SUSPENDED) && (AP.get_state(&AP, tire_pressure.AID(&tire_pressure)) == SUSPENDED)){
			if ((water_pass.pass == true) && (oil_pass.pass == true) && (tire_pass.pass == true)) {
				printf("\n  system verification completed   \n");
				printf("\n........Initializing system boot 2........\n");	
				break;
			} else {
				printf("\n-------------Adjust Systems ---------------\n");
				break;
			}
		} 
		else{
			Behaviour->msg->receive(Behaviour->msg, WAIT_FOREVER);		
			if (Behaviour->msg->get_msg_type(Behaviour->msg) == INFORM){				
				Agent_info informacion = AP.get_Agent_description(AP.get_running_agent(&AP));
				printf("\n Inspection agent in execution: %s \n",informacion.agent_name);
				char* mensaje = Behaviour->msg->get_msg_content(Behaviour->msg);
				Behaviour->msg->suspend(Behaviour->msg, Behaviour->msg->get_sender(Behaviour->msg));	
				printf("%s\n", mensaje);	
			}			
		}
				
	}
	water_pass.pass= false;//, oil_pass.pass, tire_pass.pass =false;
	oil_pass.pass = false;
	tire_pass.pass = false;
	
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



/**********************************************/
/* 						Main				  */
/**********************************************/
int main() {
	
	printf("------Boot Verification System ------ \n");
	

	//ticks counter
	int startTick= tickGet();
	
	// parameters definition: rates, types, pass
	tire_pass.rate=500;
	water_pass.rate=1000;
	oil_pass.rate=1500;
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
	inspector.Iniciador(&inspector, "Inspector", 200, 100);
	water_temp.Iniciador(&water_temp, "Water Temperature", 205, 100);
	oil_pressure.Iniciador(&oil_pressure, "Oil pressure", 203, 100);
	tire_pressure.Iniciador(&tire_pressure, "Tire pressure", 204, 100);
	
	TASK_ID rtpInfo=taskIdSelf();
	AP.Agent_Platform(&AP, "App Platform",rtpInfo);

	//Registering the Agents and their respective behaviour into the Platform
	AP.agent_init(&AP, &inspector, &InspectorFunction);
	AP.agent_initConParam(&AP, &oil_pressure, &OilFunction,(MAESArgument)&oil_pass);
	AP.agent_initConParam(&AP, &tire_pressure, &TireFunction,(MAESArgument)&tire_pass);
	AP.agent_initConParam(&AP, &water_temp, &WaterFunction,(MAESArgument)&water_pass);
	
	printf("VxMAES booted successfully \n");
	printf("Initiating APP\n\n");
	
	AP.boot(&AP);
	
	
	while(1){
		int actual_tick=tickGet();
		
		if ((actual_tick-startTick)>=(1*MinuteInTicks)){
			printf("************ VxMAES app execution stops ******************");
			break;
		}
	}
	
	// Start the scheduler so the created tasks start executing.

	
	return 0;
}




