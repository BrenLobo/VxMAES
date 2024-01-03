/*
 * sender_receiver.c
 *
 *  Created on: Oct 26, 2023
 *      Author: blobo
 */

#include <vxWorks.h>
#include "VxMAES.h"

/**********************************************/
/* 		  Defining the app's variables.       */
/**********************************************/
MAESAgent sender, receiver;
Agent_Platform AP;
sysVars env;
CyclicBehaviour readingBehaviour, writingBehaviour;
Agent_Msg msg_writing, msg_reading;

/**********************************************/
/* Functions related to the writing Behaviour */
/**********************************************/
 // setup
void writingsetup(CyclicBehaviour* Behaviour, MAESArgument taskParam) {
	Behaviour->msg->Agent_Msg(Behaviour->msg);
	Behaviour->msg->add_receiver(Behaviour->msg,receiver.AID(&receiver));
};
// action
void writingaction(CyclicBehaviour* Behaviour, MAESArgument taskParam) {
	printf("***Sending Message***\n");
	char* info_persona1= "This is the message.\n";
	Behaviour->msg->set_msg_content(Behaviour->msg, info_persona1);
	Behaviour->msg->sendAll(Behaviour->msg);
	taskDelay(100);
};

// Write Wrapper:
void writing(MAESArgument taskParam) {
	writingBehaviour.msg = &msg_writing;
	writingBehaviour.setup = &writingsetup;
	writingBehaviour.action = &writingaction;
	writingBehaviour.execute(&writingBehaviour,taskParam);
};

/**********************************************/
/* Functions related to the reading Behaviour */
/**********************************************/
// action
void readingaction(CyclicBehaviour* Behaviour, MAESArgument taskParam) {
	Behaviour->msg->Agent_Msg(Behaviour->msg);
	Behaviour->msg->receive(Behaviour->msg,WAIT_FOREVER);
	printf("***Receiving Message***\n");
	printf("Message content: %s\n",Behaviour->msg->get_msg_content(Behaviour->msg));
};

//Read Wrapper:
void reading(MAESArgument taskParam) {
	readingBehaviour.msg = &msg_reading;
	readingBehaviour.action = &readingaction;
	readingBehaviour.execute(&readingBehaviour,taskParam);
};

/**********************************************/
/* 						Main				  */
/**********************************************/
int main() {
	printf("------Sender Receiver APP------ \n");

	//ticks counter
	int startTick= tickGet();
	
	//Constructors for each initialized class
	ConstructorAgente(&sender);
	ConstructorAgente(&receiver);
	ConstructorSysVars(&env);
	ConstructorAgent_Platform(&AP, &env);
	ConstructorAgent_Msg(&msg_writing, &env);
	ConstructorAgent_Msg(&msg_reading, &env);
	ConstructorCyclicBehaviour(&writingBehaviour);
	ConstructorCyclicBehaviour(&readingBehaviour);
	
	//Defining the RPT task (change name in each RTP)
	TASK_ID rtpInfo=taskIdSelf();
	
	//Initializing the Agents and the Platform.
	receiver.Iniciador(&receiver, "Agent Receiver", 103, 100);
	sender.Iniciador(&sender, "Agent Sender", 102, 100);	
	AP.Agent_Platform(&AP, "sender_receiver_platform",rtpInfo); //add the variable of the RTP
	
	//Registering the Agents and their respective behaviour into the Platform
	AP.agent_init(&AP,&sender, &writing);
	AP.agent_init(&AP,&receiver, &reading);
	printf("	Initiating APP\n\n");
	
	//Platform Init
	AP.boot(&AP);
	
	while(1){
		int actual_tick=tickGet();
		
		if ((actual_tick-startTick)>=(ONE_MINUTE_IN_TICKS)){
			printf("Brenda aca");
			break;
		}
	}
	
	return 0;
    return 0;


};

