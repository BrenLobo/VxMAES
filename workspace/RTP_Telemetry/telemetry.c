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


/*Enums and structs*/
typedef enum meas_type
{
	CURRENT,
	VOLTAGE,
	TEMPERATURE
}meas_type;

typedef struct logger_info
{
	meas_type type;
	MAESTickType_t rate;
}logger_info;


//Defining the app's variables.

MAESAgent logger_current,logger_voltage,logger_temperature,measurement;
Agent_Platform Platform;
sysVars env;
CyclicBehaviour CurrentBehaviour,VoltageBehaviour,TemperatureBehaviour, genBehaviour;
Agent_Msg msg_current,msg_voltage,msg_temperature, msg_gen;
logger_info log_current, log_voltage, log_temperature, *info, *infox;

//Functions related to the Logger Action Behaviour//

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



//Functions related to the Generator Behaviour//

void genAction(CyclicBehaviour* Behaviour, MAESArgument taskParam) {
	Agent_info informacion = Platform.get_Agent_description(Platform.get_running_agent(&Platform));
	printf("\n Running Agent: %s",informacion.agent_name);	
	
	char response[50]="";
	double min, max, value;
	Behaviour->msg->receive(Behaviour->msg,WAIT_FOREVER);
	srand((unsigned int)time(NULL));
	int i = (int)Behaviour->msg->get_msg_content(Behaviour->msg);
	switch (i)
	{
	case CURRENT:
		min = 0.1; //mA
		max = 1000; //mA
		value = min + rand() / (RAND_MAX / (max - min + 1) + 1);
		snprintf(response, 50, "\r\nCurrent mesasurment: %f\r", value);
		break;

	case VOLTAGE:
		min = 0.5; //V
		max = 3.3; //V
		value = min + rand() / (RAND_MAX / (max - min + 1) + 1);
		snprintf(response, 50, "\r\nVoltage mesasurment: %f\r", value);
		break;

	case TEMPERATURE:
		min = 30; //C
		max = 100; //C
		value = min + rand() / (RAND_MAX / (max - min + 1) + 1);
		snprintf(response, 50, "\r\nTemperature mesasurment: %f\r", value);
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

//Main
int main() {
	printf("------Telemetry------ \n");
	int startTick= tickGet();
	//Rates and types for each logger.
	log_current.rate = 500;
	log_voltage.rate = 1500;
	log_temperature.rate = 1000;
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

	//Initializing the Agents and the Platform.
	logger_current.Iniciador(&logger_current, "Current Logger", 105, 100);
	logger_voltage.Iniciador(&logger_voltage, "Voltage Logger", 104, 100);
	logger_temperature.Iniciador(&logger_temperature, "Temperature Logger", 103, 100);
	measurement.Iniciador(&measurement, "Measure", 102, 10);
	
	TASK_ID rtpInfo=taskIdSelf();
	Platform.Agent_Platform(&Platform, "telemetry_platform",rtpInfo);

	//Registering the Agents and their respective behaviour into the Platform
	Platform.agent_initConParam(&Platform, &logger_temperature, &Temperaturelogger, (MAESArgument)&log_temperature);
	Platform.agent_initConParam(&Platform, &logger_voltage, &Voltagelogger, (MAESArgument)&log_voltage);
	Platform.agent_initConParam(&Platform,&logger_current, &Currentlogger, (MAESArgument)&log_current);
	Platform.agent_init(&Platform, &measurement, &gen);
	printf("Initiating APP\n\n");	

	//Platform Init
	Platform.boot(&Platform);
	

	// Start the scheduler so the created tasks start executing.
	while(1){
		int actual_tick=tickGet();
		taskDelay(200);
		
		if ((actual_tick-startTick)>=(4*MinuteInTicks)){
			printf("************ VxMAES app execution stops ******************");
			break;
		}
	}
	return 0;
}

