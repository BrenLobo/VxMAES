/*
 * telemetry.c
 *
 *  Created on: Dec 11, 2023
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
typedef enum meas_type
{
	CURRENT,
	VOLTAGE,
	TEMPERATURE
}meas_type;

// let boot parameter
typedef struct logger_info
{
	meas_type type;
	MAESTickType_t rate;
}logger_info;


/**********************************************/
/* 		  Defining the app's variables.       */
/**********************************************/


MAESAgent logger_current,logger_voltage,logger_temperature,measurement;
Agent_Platform Platform;
sysVars env;
CyclicBehaviour CurrentBehaviour,VoltageBehaviour,TemperatureBehaviour, genBehaviour;
Agent_Msg msg_current,msg_voltage,msg_temperature, msg_gen;
logger_info log_current, log_voltage, log_temperature, *info, *infox;



/******************************************************/
/*   Function related to the Logger Action Behavior   */
/******************************************************/

// logger action
void loggerAction(CyclicBehaviour* Behaviour,MAESArgument taskParam) {
	info= (logger_info*)taskParam;
	Behaviour->msg->set_msg_content(Behaviour->msg, (char*)info->type);
	Behaviour->msg->sendX(Behaviour->msg, measurement.AID(&measurement), WAIT_FOREVER);
	Behaviour->msg->receive(Behaviour->msg,WAIT_FOREVER);

	/*Logging*/
	printf("%s\n", Behaviour->msg->get_msg_content(Behaviour->msg));
	Platform.agent_wait(&Platform,info->rate);
};

//Current Logger Wrapper:
void Currentlogger(MAESArgument taskParam) {
	CurrentBehaviour.msg = &msg_current;
	CurrentBehaviour.msg->Agent_Msg(CurrentBehaviour.msg);
	CurrentBehaviour.action = &loggerAction;
	CurrentBehaviour.execute(&CurrentBehaviour, taskParam);
};

//Voltage Logger Wrapper:
void Voltagelogger(MAESArgument taskParam) {
	VoltageBehaviour.msg = &msg_voltage;
	VoltageBehaviour.msg->Agent_Msg(VoltageBehaviour.msg);
	VoltageBehaviour.action = &loggerAction;
	VoltageBehaviour.execute(&VoltageBehaviour, taskParam);
};

//Temperature Logger Wrapper:
void Temperaturelogger(MAESArgument taskParam) {
	TemperatureBehaviour.msg = &msg_temperature;
	TemperatureBehaviour.msg->Agent_Msg(TemperatureBehaviour.msg);
	TemperatureBehaviour.action = &loggerAction;
	TemperatureBehaviour.execute(&TemperatureBehaviour, taskParam);
};



/******************************************************/
/*     Function related to the Generator Behavior     */
/******************************************************/

//action
void genAction(CyclicBehaviour* Behaviour, MAESArgument taskParam) {
	Agent_info informacion = Platform.get_Agent_description(Platform.get_running_agent(&Platform));
	printf("\n Agente en ejecucion: %s",informacion.agent_name);	
	
	char response[50]="";
	int min, max, value;
	Behaviour->msg->receive(Behaviour->msg,WAIT_FOREVER);
	srand((unsigned int)time(NULL));
	
	int i = (int)Behaviour->msg->get_msg_content(Behaviour->msg);
	switch (i)
	{
	case CURRENT:
		min = 1; //mA
		max = 1000; //mA
		value = (int)(min + rand() / (RAND_MAX / (max - min + 1) + 1));
		snprintf(response, 50, "\r\nCurrent measurement: %d\r", value);
		break;

	case VOLTAGE:
		min = 0; //V
		max = 4; //V
		value = (int)(min + rand() / (RAND_MAX / (max - min + 1) + 1));
		snprintf(response, 50, "\r\nVoltage measurement: %d\r", value);
		break;

	case TEMPERATURE:
		min = 30; //C
		max = 100; //C
		value = (int)(min + rand() / (RAND_MAX / (max - min + 1) + 1));
		snprintf(response, 50, "\r\nTemperature measurement: %f\r", value);
		break;

	default:
		snprintf(response, 50, "\r\nNot understood");
		printf(response);
		break;
	}

	Behaviour->msg->set_msg_content(Behaviour->msg,response);
	Behaviour->msg->sendX(Behaviour->msg, Behaviour->msg->get_sender(Behaviour->msg), WAIT_FOREVER);
};

//Generator Wrapper
void gen(MAESArgument taskParam) {
	genBehaviour.msg = &msg_gen;
	genBehaviour.msg->Agent_Msg(genBehaviour.msg);
	genBehaviour.action = &genAction;
	genBehaviour.execute(&genBehaviour, taskParam);
};

/**********************************************/
/* 						Main				  */
/**********************************************/
int main() {
	
	printf("------Telemetry------ \n");
	

	//ticks counter
	int startTick= tickGet();
	
	// parameters definition: rates, types
	log_current.rate = 500;
	log_voltage.rate = 1000;
	log_temperature.rate = 1500;
	log_current.type = CURRENT;
	log_voltage.type = VOLTAGE;
	log_temperature.type = TEMPERATURE;

	//Constructors for each initialized class
	ConstructorAgente(&logger_current);
	ConstructorAgente(&logger_voltage);
	ConstructorAgente(&logger_temperature);
	ConstructorAgente(&measurement);
	ConstructorSysVars(&env);
	ConstructorAgent_Platform(&Platform, &env);
	ConstructorAgent_Msg(&msg_current, &env);
	ConstructorAgent_Msg(&msg_voltage, &env);
	ConstructorAgent_Msg(&msg_temperature, &env);
	ConstructorAgent_Msg(&msg_gen, &env);
	ConstructorCyclicBehaviour(&CurrentBehaviour);
	ConstructorCyclicBehaviour(&VoltageBehaviour);
	ConstructorCyclicBehaviour(&TemperatureBehaviour);
	ConstructorCyclicBehaviour(&genBehaviour);

	//Defining the RPT task (change name in each RTP)
	TASK_ID rtpInfo=taskIdSelf();
	
	//Initializing the Agents and the Platform.
	logger_current.Iniciador(&logger_current, "Current Logger", 201, 100);
	logger_voltage.Iniciador(&logger_voltage, "Voltage Logger", 202, 100);
	logger_temperature.Iniciador(&logger_temperature, "Temperature Logger", 203, 100);
	measurement.Iniciador(&measurement, "Measure", 200, 10);	
	Platform.Agent_Platform(&Platform, "telemetry_platform",rtpInfo);//add the variable of the RTP

	//Registering the Agents and their respective behaviour into the Platform
	Platform.agent_init(&Platform, &measurement, &gen);
	Platform.agent_initConParam(&Platform, &logger_temperature, &Temperaturelogger, (MAESArgument)&log_temperature);
	Platform.agent_initConParam(&Platform, &logger_voltage, &Voltagelogger, (MAESArgument)&log_voltage);
	Platform.agent_initConParam(&Platform,&logger_current, &Currentlogger, (MAESArgument)&log_current);
	printf("CMAES booted successfully \n");
	printf("Initiating APP\n\n");
	
	//Platform Init
	Platform.boot(&Platform);
	

	// Start the scheduler so the created tasks start executing.
	while(1){
		int actual_tick=tickGet();
		
		if ((actual_tick-startTick)>=(MinuteInTicks)){
			printf("************ VxMAES app execution stops ******************");
			break;
		}
	}
	return 0;
}

