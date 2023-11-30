/*
 * Rock_Paper_Scissors.c
 *
 *  Created on: Oct 31, 2023
 *      Author: blobo
 */
#include <vxWorks.h>
#include "VxMAES.h"
#include <stdlib.h>
#include <string.h>

/**********************************************/
/* 		  Defining the app's variables.       */
/**********************************************/

MAESAgent AgentA, AgentB, Referee;
Agent_Platform Platform; 
sysVars env;
CyclicBehaviour playBehaviourA, playBehaviourB;
OneShotBehaviour watchoverBehaviour;
Agent_Msg msg_playA,msg_playB, msg_watchover;


/** 
 * Random Number Generator Function: This function generates a random number between 0 and 2.
 * 			 Inputs: None.
 * 			Outputs: Random number between 0 and 2.
 */
int getRandom() {
	return rand()%2;
};

/**
 * Msg to int Function: This function translates a specific msg to a certain numeric value.
 * 		 	 Inputs: A message that represents the player's choice.
 *  		Outputs: A number that represents the player's choice.
 */
int choices(char* msg) {
	if (strcmp((const char*)msg, "ROCK"))
	{
		return 0;
	}
	else if (strcmp((const char*)msg, "PAPER"))
	{
		return 1;
	}
	else
	{
		return 2; //scissors
	}
};

/**********************************************/
/*  Functions related to the play Behaviour   */
/**********************************************/

void playSetup(CyclicBehaviour* Behaviour, MAESArgument taskParam) {
	Behaviour->msg->Agent_Msg(Behaviour->msg);
	Behaviour->msg->add_receiver(Behaviour->msg, Referee.AID(&Referee));
};

void playAction(CyclicBehaviour* Behaviour, MAESArgument taskParam) {
	Agent_info informacion = Platform.get_Agent_description(Platform.get_running_agent(&Platform));
	printf(informacion.agent_name);
	printf(": Rock, Paper, Scissors... \n");
//	Platform.agent_wait(&Platform, 10);
	int num = getRandom();
	char* bet= "";
	switch (num){
	case 0:
		bet = "ROCK";
		break;

	case 1:
		bet = "PAPER";
		break;

	case 2:
		bet = "SCISSORS";
		break;

	default:
		break;
	}
	printf("bet: %s\n",bet);
	Behaviour->msg->set_msg_content(Behaviour->msg, bet);
	Behaviour->msg->set_msg_type(Behaviour->msg, INFORM);
	Behaviour->msg->sendAll(Behaviour->msg);
};

/**
 * Wrappers: Since two different agents are going to use the play Behaviour, a wrapper function must be created for each one of the agents. Doing this, two different instances of the play Behaviour will be created and
 *          there will not be an error regarding overwriting values from the other agent. 
 */
//PlayA Wrapper: 
void playA(MAESArgument taskParam) {
	playBehaviourA.msg = &msg_playA;
	playBehaviourA.setup = &playSetup;
	playBehaviourA.action = &playAction;
	playBehaviourA.execute(&playBehaviourA, taskParam);
	
};

//PlayB Wrapper:
void playB(MAESArgument taskParam) {
	playBehaviourB.msg = &msg_playB;
	playBehaviourB.setup = &playSetup;
	playBehaviourB.action = &playAction;
	playBehaviourB.execute(&playBehaviourB, taskParam);
	
};

//Functions related to the Watchover Behaviour

void watchoverSetup(OneShotBehaviour* Behaviour, MAESArgument taskParam) {
	Behaviour->msg->Agent_Msg(Behaviour->msg);
	Behaviour->msg->add_receiver(Behaviour->msg,AgentA.AID(&AgentA));
	Behaviour->msg->add_receiver(Behaviour->msg,AgentB.AID(&AgentB));
};

void watchoverAction(OneShotBehaviour* Behaviour, MAESArgument taskParam) {
	printf("wacho ejecutando\n");
	char* msgA;
	char* msgB;
	int choiceA;
	int choiceB;
	int winner[3][3] = {
		{0, 2, 1},
		{1, 0, 2},
		{2, 1, 0}
	};
	while (true)
	{
		Behaviour->msg->receive(Behaviour->msg,WAIT_FOREVER);
		printf("Se recibe mensaje\n");
		if (Behaviour->msg->get_msg_type(Behaviour->msg) == INFORM)
		{
			Agent_info PlayerInfo = Platform.get_Agent_description(Behaviour->msg->get_sender(Behaviour->msg));
			printf(PlayerInfo.agent_name);
			printf(": ");
			printf(Behaviour->msg->get_msg_content(Behaviour->msg));
			printf("\n");
			if (Behaviour->msg->get_sender(Behaviour->msg) == AgentA.AID(&AgentA))
			{
				msgA = Behaviour->msg->get_msg_content(Behaviour->msg);
				choiceA = choices(msgA);
			}
			else if (Behaviour->msg->get_sender(Behaviour->msg) == AgentB.AID(&AgentB))
			{
				msgB = Behaviour->msg->get_msg_content(Behaviour->msg);
				choiceB = choices(msgB);
			}
			Behaviour->msg->suspend(Behaviour->msg, Behaviour->msg->get_sender(Behaviour->msg));
		}
		else {
			printf("no entro\n");
		}
		if (Platform.get_state(&Platform,AgentA.AID(&AgentA)) == SUSPENDED && Platform.get_state(&Platform,AgentB.AID(&AgentB)) == SUSPENDED) {
			break;
		}
	}
	Agent_info RefereeInfo = Platform.get_Agent_description(Platform.get_running_agent(&Platform));
	switch (winner[choiceA][choiceB])
	{
	case 0:
		printf("\n");
		printf(RefereeInfo.agent_name);
		printf(": DRAW!\n");
		break;

	case 1:
		printf("\n");
		printf(RefereeInfo.agent_name);
		printf(": PLAYER A WINS!\n");
		break;

	case 2:
		printf("\n");
		printf(RefereeInfo.agent_name);
		printf(": PLAYER B WINS!\n");
		break;

	default:
		break;
	}
	Platform.agent_wait(&Platform,2000);
	Behaviour->msg->resume(Behaviour->msg,AgentA.AID(&AgentA));
	Behaviour->msg->resume(Behaviour->msg,AgentB.AID(&AgentB));
	printf("\n-------------PLAYING AGAIN---------------\n");
};

//Watchover Wrapper:
void watchover(MAESArgument taskParam) {
	watchoverBehaviour.msg = &msg_watchover;
	watchoverBehaviour.setup = &watchoverSetup;
	watchoverBehaviour.action = &watchoverAction;
	for (;;) {
		watchoverBehaviour.execute(&watchoverBehaviour, taskParam);
	}
};

//Main
int main() {
	printf("------Rock Paper Scissors APP------ \n");

	//Constructors for each initialized class
	ConstructorAgente(&AgentA);
	ConstructorAgente(&AgentB);
	ConstructorAgente(&Referee);
	ConstructorSysVars(&env);
	ConstructorAgent_Platform(&Platform, &env);
	ConstructorAgent_Msg(&msg_playA, &env);
	ConstructorAgent_Msg(&msg_playB, &env);
	ConstructorAgent_Msg(&msg_watchover, &env);
	ConstructorCyclicBehaviour(&playBehaviourA);
	ConstructorCyclicBehaviour(&playBehaviourB);
	ConstructorOneShotBehaviour(&watchoverBehaviour);

	//Defining the RPT task
	TASK_ID rtpInfo=taskIdSelf();
	
	//Initializing the Agents and the Platform.
	AgentA.Iniciador(&AgentA, "Player A", 104, 300);
	AgentB.Iniciador(&AgentB, "Player B", 105, 300);
	Referee.Iniciador(&Referee, "Referee", 102, 300);
	Platform.Agent_Platform(&Platform, "RPS_platform",rtpInfo);

	//Registering the Agents and their respective behaviour into the Platform
	Platform.agent_init(&Platform,&AgentA, &playA);
	Platform.agent_init(&Platform, &AgentB, &playB);
	Platform.agent_init(&Platform, &Referee, &watchover);
	
	printf("CMAES booted successfully \n");
	printf("Initiating APP\n\n");
	Platform.boot(&Platform);
	

	return 0;
}


