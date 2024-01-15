/*
 * Agent.c
 *
 *  Created on: Oct 11, 2023
 *      Author: blobo
 */

/*	INCLUDE FILES */

#include "VxMAES.h"
#include <stdio.h>
#include <stdlib.h>


/*
 *  AID Function: It returns the Agent ID of the Agent given.
 *  	Inputs: Pointer to an Agent.
 *  	Output: The Agent's ID.
 */
Agent_AID AIDX(MAESAgent* MAESagent) {
	return MAESagent->agent.aid; 
};

/**
 * Agent Initialization: The function sets the default values for each parameter.
 *	 	 Inputs: Pointer to an Agent, String that contains the name you want 
 * 		         to give to the Agent, priority and the size of the stack.
 *   	Outputs: None
 */
void MAESAgentX(MAESAgent* MAESagent,char* name, STATUS pri, MAESStackSize sizeStack)
{
	MAESagent->agent.aid = NULL;
	MAESagent->agent.queue_id = NULL;
	MAESagent->agent.agent_name = name;
	MAESagent->agent.priority = pri;
	MAESagent->resources.stackSize = sizeStack;
	MAESagent->agent.AP = NULL;
	MAESagent->agent.org = NULL;
	MAESagent->agent.affiliation = NON_MEMBER;
	MAESagent->agent.role = VISITOR;
}

/**
 * Agent Constructor: This function assigns the class pointers to its corresponding function.
 * 		 Inputs: Pointer to the Agent.
 * 		Outputs: None.
 */
void ConstructorAgente(MAESAgent* agente) {
	agente->Iniciador = &MAESAgentX;
	agente->AID = &AIDX;
}

