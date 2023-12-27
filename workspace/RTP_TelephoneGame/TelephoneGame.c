/*
 * TelephoneGame.c
 *
 *  Created on: Oct 31, 2023
 *      Author: blobo
 */

#include <vxWorks.h>
#include "VxMAES.h"
#include <string.h>

/**********************************************/
/* 		  Defining the app's variables.       */
/**********************************************/

MAESAgent Person1, Person2, Person3;
Agent_Platform APTelephone;
sysVars env;
OneShotBehaviour BehaviourP1, BehaviourP2, BehaviourP3;
Agent_Msg msgperson1, msgperson2, msgperson3;
SEM_ID    mySemMId; 

  

/**********************************************/
/*    Setup functions related to each agent   */
/**********************************************/
void person1setup(OneShotBehaviour* Behaviour, MAESArgument taskParam) {
	Behaviour->msg->Agent_Msg(Behaviour->msg);
	char* info_persona1= "This";
	Behaviour->msg->add_receiver(Behaviour->msg, Person2.AID(&Person2));
	Behaviour->msg->set_msg_content(Behaviour->msg, info_persona1);
};

void person2setup(OneShotBehaviour* Behaviour, MAESArgument taskParam) {
	Behaviour->msg->Agent_Msg(Behaviour->msg);
	Behaviour->msg->add_receiver(Behaviour->msg, Person3.AID(&Person3));
	char* info_persona2= " is the ";
	Behaviour->msg->set_msg_content(Behaviour->msg, info_persona2);
};

void person3setup(OneShotBehaviour* Behaviour, MAESArgument taskParam) {
	Behaviour->msg->Agent_Msg(Behaviour->msg);
	Behaviour->msg->add_receiver(Behaviour->msg, Person1.AID(&Person1));
	char* info_persona3 = "message.";
	Behaviour->msg->set_msg_content(Behaviour->msg, info_persona3);
};

/**********************************************/
/*   Action function related to the Person 1  */
/**********************************************/
void persona1action(OneShotBehaviour* Behaviour, MAESArgument taskParam) {
	Agent_info informacion = APTelephone.get_Agent_description(APTelephone.get_running_agent(&APTelephone));
	printf("\n Agente en ejecucion: %s", informacion.agent_name);
	printf("\nEste es el mensaje de %s: %s\n", informacion.agent_name, Behaviour->msg->get_msg_content(Behaviour->msg));
	Behaviour->msg->sendAll(Behaviour->msg);

};

/************************************************/
/*Action function related to the Person 2 and 3 */
/************************************************/
void personaction(OneShotBehaviour* Behaviour, MAESArgument taskParam) {
	Agent_info informacion = APTelephone.get_Agent_description(APTelephone.get_running_agent(&APTelephone));
	printf("\n Agente en ejecucion: %s\n",informacion.agent_name);
	char* msg_content = Behaviour->msg->get_msg_content(Behaviour->msg);
	Behaviour->msg->receive(Behaviour->msg, WAIT_FOREVER);
	char* contenidoTel= Behaviour->msg->get_msg_content(Behaviour->msg);
	char contenido[30]="";
	strncat_s(contenido, 30, contenidoTel, 14);	
	strncat_s(contenido, 30, msg_content, 10);
	taskDelay(50);
	Behaviour->msg->set_msg_content(Behaviour->msg,(char*)contenido);	
	printf("\nEste es el mensaje de %s: %s\n",informacion.agent_name, Behaviour->msg->get_msg_content(Behaviour->msg));
	Behaviour->msg->sendAll(Behaviour->msg);

};


/**********************************************/
/*         Wrappers for each agent			  */
/**********************************************/
void phonePerson1(MAESArgument taskParam) {
	BehaviourP1.msg = &msgperson1;
	BehaviourP1.setup = &person1setup;
	BehaviourP1.action = &persona1action;
	BehaviourP1.execute(&BehaviourP1, taskParam);
};

void phonePerson2(MAESArgument taskParam) {
	BehaviourP2.msg = &msgperson2;
	BehaviourP2.setup = &person2setup;
	BehaviourP2.action = &personaction;
	BehaviourP2.execute(&BehaviourP2, taskParam);
 };

void phonePerson3(MAESArgument taskParam) {
	BehaviourP3.msg = &msgperson3;
	BehaviourP3.setup = &person3setup;
	BehaviourP3.action = &personaction;
	BehaviourP3.execute(&BehaviourP3, taskParam);
};


/**********************************************/
/* 						Main				  */
/**********************************************/
int main() {
	printf("------Telephone Game APP ------ \n");
	int startTick= tickGet();
	//Constructors for each initialized class
	ConstructorAgente(&Person1);
	ConstructorAgente(&Person2);
	ConstructorAgente(&Person3);
	ConstructorSysVars(&env);
	ConstructorAgent_Platform(&APTelephone, &env);
	ConstructorAgent_Msg(&msgperson1, &env);
	ConstructorAgent_Msg(&msgperson2, &env);
	ConstructorAgent_Msg(&msgperson3, &env); 
	ConstructorOneShotBehaviour(&BehaviourP1);
	ConstructorOneShotBehaviour(&BehaviourP2);
	ConstructorOneShotBehaviour(&BehaviourP3);
	
	//Defining the RPT task (change name in each RTP)
	TASK_ID rtpInfo=taskIdSelf();
	
	//Initializing the Agents and the Platform.
	Person1.Iniciador(&Person1, "Persona 1", 105, 200);
	Person2.Iniciador(&Person2, "Persona 2", 104, 200);
	Person3.Iniciador(&Person3, "Persona 3", 103, 200);
	APTelephone.Agent_Platform(&APTelephone, "AP Telephone",rtpInfo);

	//Registering the Agents and their respective behaviour into the Platform
	APTelephone.agent_init(&APTelephone, &Person1, &phonePerson1);
	APTelephone.agent_init(&APTelephone, &Person2, &phonePerson2);
	APTelephone.agent_init(&APTelephone, &Person3, &phonePerson3);
	printf("VxMAES booted successfully \n");
	printf("Initiating APP\n\n");
	
	//Platform Init
	APTelephone.boot(&APTelephone);
	
	while(1){
		int actual_tick=tickGet();
		
		if ((actual_tick-startTick)>=(ONE_MINUTE_IN_TICKS)){
			printf("Brenda aca");
			break;
		}
	}
	
	return 0;

}
