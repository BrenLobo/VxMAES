/*
 * Agent_Platform.c
 *
 *  Created on: Oct 22, 2023
 *      Author: blobo
 */

#include "VxMAES.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/**
 * AMS Task Function: This function initiates the AMS Task, which manages the states of the agents. 
 * 		 Inputs: Parameters passed into the AMS Task Function.
 * 		Outputs: None.
 */
void AMS_taskFunction(MAESArgument taskParam) {
	AMSparameter* amsParameters = (AMSparameter*)taskParam;
	USER_DEF_COND* cond = amsParameters->cond;
	Agent_Platform* services = amsParameters->services;
	sysVars* ptrenvi = amsParameters->ptr_env;	
	Agent_Msg msg;
	ConstructorAgent_Msg(&msg, ptrenvi);
	msg.Agent_Msg(&msg);
	MAESUBaseType_t error_msg = 0;
	for (;;)
	{
		msg.receive(&msg, WAIT_FOREVER);
		if (msg.get_msg_type(&msg) == REQUEST)
		{
			if (strcmp((const char*)msg.get_msg_content(&msg), "KILL") == 0)
			{
				if (cond->kill_cond())
				{
					error_msg = services->kill_agent(services, msg.get_target_agent(&msg));
					if (error_msg == NO_ERRORS)
					{
						msg.set_msg_type(&msg, CONFIRM);
					}
					else
					{
						msg.set_msg_type(&msg, REFUSE);
					}
				}
				else
				{
					msg.set_msg_type(&msg, REFUSE);
				}
				msg.sendX(&msg, msg.get_sender(&msg), NO_WAIT);
			} //KILL Case

			else if (strcmp((const char*)msg.get_msg_content(&msg),"REGISTER") == 0)
			{
				if (cond->register_cond())
				{
					error_msg = services->register_agent(services, msg.get_target_agent(&msg));
					if (error_msg == NO_ERRORS)
					{
						msg.set_msg_type(&msg, CONFIRM);
					}
					else
					{
						msg.set_msg_type(&msg, REFUSE);
					}
				}
				else
				{
					msg.set_msg_type(&msg, REFUSE);
				}
				msg.sendX(&msg, msg.get_sender(&msg), NO_WAIT);
			} //REGISTER Case

			else if (strcmp((const char*)msg.get_msg_content(&msg), "DEREGISTER") == 0)
			{
				if (cond->deregister_cond())
				{
					error_msg = services->deregister_agent(services, msg.get_target_agent(&msg));
					if (error_msg == NO_ERRORS)
					{
						msg.set_msg_type(&msg, CONFIRM);
					}
					else
					{
						msg.set_msg_type(&msg, REFUSE);
					}
				}
				else
				{
					msg.set_msg_type(&msg, REFUSE);
				}
				msg.sendX(&msg, msg.get_sender(&msg), NO_WAIT);
			} //DEREGISTER Case

			else if (strcmp((const char*)msg.get_msg_content(&msg), "SUSPEND") == 0)
			{
				if (cond->suspend_cond())
				{
					error_msg = services->suspend_agent(services, msg.get_target_agent(&msg));
					if (error_msg == NO_ERRORS)
					{
						msg.set_msg_type(&msg, CONFIRM);
					}
					else
					{
						msg.set_msg_type(&msg, REFUSE);
					}
				}
				else
				{
					msg.set_msg_type(&msg, REFUSE);
				}
				msg.sendX(&msg, msg.get_sender(&msg), NO_WAIT);
			} //SUSPEND Case

			else if (strcmp((const char*)msg.get_msg_content(&msg), "RESUME") == 0)
			{
				if (cond->resume_cond())
				{
					error_msg = services->resume_agent(services, msg.get_target_agent(&msg));
					if (error_msg == NO_ERRORS)
					{
						msg.set_msg_type(&msg, CONFIRM);
					}
					else
					{
						msg.set_msg_type(&msg, REFUSE);
					}
				}
				else
				{
					msg.set_msg_type(&msg, REFUSE);
				}
				msg.sendX(&msg, msg.get_sender(&msg), NO_WAIT);
			} //RESUME Case

			else if (strcmp((const char*)msg.get_msg_content(&msg), "RESTART") == 0)
			{
				if (cond->restart_cond())
				{
					services->restart_agent(services, msg.get_target_agent(&msg));
				}
				else
				{
					msg.set_msg_type(&msg, REFUSE);
				}
				msg.sendX(&msg, msg.get_sender(&msg), NO_WAIT);
			} //RESTART Case

			else
			{
				msg.set_msg_type(&msg, NOT_UNDERSTOOD);
				msg.sendX(&msg, msg.get_sender(&msg), NO_WAIT);
			}
		} //end if
		else
		{
			msg.set_msg_type(&msg, NOT_UNDERSTOOD);
			msg.sendX(&msg, msg.get_sender(&msg), NO_WAIT);
		}
	} // end while
};

/**
 * Agent Platform Function: This function sets some of the initial values of the parameters needed in the 
 * 							agent platform. 
 * 		  Inputs: The Platform instance itself and the platform given name.
 *		 Outputs: None.
 */
void Agent_PlatformFunction(Agent_Platform* platform, char* name, Agent_AID rtpInfo) {
	static AMSparameter parameters;
	platform->parameter = &parameters;
	ConstructorUSER_DEF_COND(&platform->cond);
	platform->agentAMS.agent.agent_name = name;
	platform->description.AP_name = name;
	platform->description.RTP_info= rtpInfo;
	platform->description.subscribers = 0;
	platform->ptr_cond = &platform->cond;
	platform->agentAMS.agent.priority = MAESmaxPriority;
	for (MAESUBaseType_t i = 0; i < AGENT_LIST_SIZE; i++)
	{
		platform->Agent_Handle[i] = (Agent_AID)NULL;
	}
};

/**
 * Agent Platform with Conditions Function: This function sets some of the initial values of the parameters
 * 			    needed in the agent platform. In this function, some conditions are established by the user. 
 *		 Inputs: The Platform instance itself, the platform given name and the conditions given by the user.
 *		Outputs: None.
 */
void Agent_PlatformWithCondFunction(Agent_Platform* platform, char* name, USER_DEF_COND* user_cond,Agent_AID rtpInfo) {
	platform->agentAMS.agent.agent_name = name;
	platform->ptr_cond = user_cond;
	platform->description.RTP_info= rtpInfo;
	platform->description.subscribers = 0;
	platform->description.AP_name = name;
	platform->agentAMS.agent.priority = MAESmaxPriority;
	for (MAESUBaseType_t i = 0; i < AGENT_LIST_SIZE; i++)
	{
		platform->Agent_Handle[i] = (Agent_AID)NULL;
	}
};

/*
 * Boot Function: This function boots the agent platform. Therefore, it register each agent into the 
 * 				   platform itself. 
 * 		     Inputs: The Platform instance itself.
 *			Outputs: Bool variable indicating if the boot was successful.
 */
bool bootFunction(Agent_Platform* platform) {
	if (taskIdSelf() == platform->description.RTP_info)
	{
		ConstructorAgente(&platform->agentAMS);		
		platform->agentAMS.Iniciador(&platform->agentAMS, "AMSAgent", MAESmaxPriority, 1024); 
		platform->agentAMS.agent.queue_id = msgQCreate(1,MAXmsgLength,MSG_Q_FIFO );
		// Task
		platform->parameter->cond=platform->ptr_cond;
		platform->parameter->services = platform;
		platform->parameter->ptr_env = &env;
		platform->agentAMS.resources.stackSize = MAESminStacksize;
		platform->agentAMS.agent.aid=taskCreate(platform->agentAMS.agent.agent_name,MAESmaxPriority,VX_FP_TASK, platform->agentAMS.resources.stackSize,(MAESTaskFunction_t)AMS_taskFunction, (MAESArgument)platform->parameter,0,0,0,0,0,0,0,0,0);
		platform->description.AMS_AID = platform->agentAMS.agent.aid;
		env.set_TaskEnv(&env,platform->agentAMS.agent.aid, &platform->agentAMS);
		if (platform->agentAMS.agent.aid != NULL)
		{
			sysVar* element;
			MAESUBaseType_t i = 0;
			while (i < AGENT_LIST_SIZE) {
				element = env.getEnv(&env);

				if ((element[i].first == NULL) || (platform->agentAMS.agent.aid == NULL))
				{
					break;
				}
				platform->register_agent(platform,element[i].first);
				i++;
			}
			taskActivate(platform->agentAMS.agent.aid);
			return NO_ERRORS;
		}
		else
		{
			/* System_abort */
			return INVALID;
		}
	}
	else
	{
		return INVALID;
	}
};

/**
 * Agent Initiate Function: This function creates a thread for an agent. 
 * 			 Inputs: The Platform instance itself, the agent and the agent's behavior.
 * 			Outputs: None.
*/
void agent_initFunction(Agent_Platform* platform, MAESAgent* agent, MAESTaskFunction_t behaviour) {
	if (taskIdSelf() == platform->description.RTP_info)
	{
		// Mailbox
		agent->agent.queue_id = msgQCreate(1, MAXmsgLength,MSG_Q_FIFO);
		// Task
		agent->resources.function = behaviour;
		agent->resources.taskParameters = NULL;
		agent->agent.aid= taskCreate(agent->agent.agent_name,agent->agent.priority,VX_FP_TASK,agent->resources.stackSize,agent->resources.function, 0, 0,0,0,0,0,0,0,0,0);//revisar
		env.set_TaskEnv(&env,agent->agent.aid, agent);
	}
};

/**
 * Agent Initiate with Parameters Function: This function creates a thread for an agent. Also, it includes 
 * 											input parameters 
 * 			 Inputs: The Platform instance itself, the agent, the agent's behavior and its input parameters.
 * 			Outputs: None.
 */
void agent_initConParamFunction(Agent_Platform* platform, MAESAgent* agent, MAESTaskFunction_t behaviour, MAESArgument taskParam) {
	if (taskIdSelf() == platform->description.RTP_info)
	{
		// Mailbox
		agent->agent.queue_id = msgQCreate(1, MAXmsgLength,MSG_Q_FIFO);
		// Task
		agent->resources.function = behaviour;
		agent->resources.taskParameters = taskParam;
		agent->agent.aid= taskCreate(agent->agent.agent_name,agent->agent.priority,VX_FP_TASK,agent->resources.stackSize, behaviour,taskParam, 0,0,0,0,0,0,0,0,0);
		env.set_TaskEnv(&env,agent->agent.aid, agent);
	}
};

/**
 * Agent Search Function: This function searches for an agent in the platform. 
 * 			 Inputs: The Platform instance itself and the agent AID.
 * 			Outputs: A bool value that indicates if the agent was found in the platform.
 */
bool agent_searchFunction(Agent_Platform* platform, Agent_AID aid) {
	for (MAESUBaseType_t i = 0; i < platform->description.subscribers; i++)
	{
		if (platform->Agent_Handle[i] == aid)
		{
			return true;
		}
	}
	return false;
};

/**
 * Agent Wait Function: This function creates a Vxworks delay. 
 * 		   	 Inputs: The Platform instance itself and the amount of ticks of delay.
 * 			Outputs: None.
 */
void agent_waitFunction(Agent_Platform* platform, MAESTickType_t ticks) {
	taskDelay(ticks);
};

/**
 * Agent Yield Function: This function stops the execution of the program. 
 * 		   	 Inputs: The Platform instance itself.
 * 			Outputs: None.
 */
void agent_yieldFunction(Agent_Platform* platform) {
	taskDelay(0); 
};

/** 
 * Get Running Agent Function: This function indicates the agent that is currently executing its task. 
 * 			 Inputs: The Platform instance itself.
 * 			Outputs: None.
 */
Agent_AID get_running_agentFunction(Agent_Platform* platform) {
	return taskIdSelf();
};
/**
 * Get State Funtion: This function gets the state of the task that is running in the platform.
 * 			 Input: The platform instance itself and the agent AID.
 * 			Output: The agent MODE / state. 
 */

AGENT_MODE get_stateFunction(Agent_Platform* platform, Agent_AID aid) {
	if (platform->agent_search(platform,aid))
	{	
		if(taskIsReady(aid)){
			return ACTIVE;
		} else if(taskIsPended(aid)){
			return WAITING;
		} else if (taskIsSuspended(aid)){
			return SUSPENDED;
		} else if (taskIsDelayed(aid,0)){
			return DELAYED;
		}else {
			return NO_MODE;
		}
	}
	else
	{
		return NO_MODE;
	}
};

/* *
 * Get Agent Description Function: This function indicates the description of an specific agent. 
 * 			 Inputs: The Platform instance itself and the agents AID.
 * 			Outputs: The description of the agent.
 */ 
Agent_info get_Agent_descriptionFunction(Agent_AID aid) {
	MAESAgent* a = (MAESAgent*)env.get_taskEnv(&env, aid);
	return a->agent;
};

/**
 * Get Agent Platform Description Function: This function indicates the platform's description. 
 * 		  	 Inputs: The Platform instance itself.
 * 			Outputs: The description of the platform.
 */

AP_Description get_AP_descriptionFunction(Agent_Platform* platform) {
	return platform->description;
};


/*
 *Register Agent Function: This function registers an agent into the platform. 
 *		  Inputs: The Platform instance itself and the agent's AID.
 * 		 Outputs: An error code indicating if registering the agent was successful.
 */
ERROR_CODE register_agentFunction(Agent_Platform* platform, Agent_AID aid) {
	if (aid == NULL)
	{
		return HANDLE_NULL;	
	}
	else if (taskIdSelf() == platform->description.RTP_info||taskIdSelf() == platform->description.AMS_AID)
	{
		if (!platform->agent_search(platform,aid))
		{
			if (platform->description.subscribers < AGENT_LIST_SIZE)
			{
				MAESAgent* a;
				a = env.get_taskEnv(&env, aid);
				a->agent.AP = platform->agentAMS.agent.aid;
				platform->Agent_Handle[platform->description.subscribers] = aid;
				platform->description.subscribers++;
				taskPrioritySet(aid, a->agent.priority);
				taskActivate(aid);
				return NO_ERRORS;
			}
			else
			{
				printf("\nList FUll\n");
				return LIST_FULL;
			}
		}
		else
		{
			printf("\nDuplicated\n");
			return DUPLICATED;
		}
	}
	else
	{
		printf("\nInvalid\n");
		return INVALID;
	}
};

/**
 * Deregister Agent Function: This function deregisters an agent into the platform. 
 * 		   	 Inputs: The Platform instance itself and the agent's AID.
 * 			Outputs: An error code indicating if deregistering the agent was successful.
 */
ERROR_CODE deregister_agentFunction(Agent_Platform* platform, Agent_AID aid) {
	if (taskIdSelf() == platform->description.AMS_AID)
	{
		MAESUBaseType_t i = 0;
		while (i < AGENT_LIST_SIZE)
		{
			if (platform->Agent_Handle[i] == aid)
			{
				MAESAgent* a;
				platform->suspend_agent(platform,aid);
				a = (MAESAgent*)env.get_taskEnv(&env, aid);
				a->agent.AP = NULL;

				while (i < AGENT_LIST_SIZE - 1)
				{
					platform->Agent_Handle[i] = platform->Agent_Handle[i + 1];
					i++;
				}
				platform->Agent_Handle[AGENT_LIST_SIZE - 1] = NULL;
				platform->description.subscribers--;
				break;
			}
			i++;
		}
		if (i == AGENT_LIST_SIZE)
		{
			return NOT_FOUND;
		}
	}
	return NO_ERRORS;
};

/**
 * Kill Agent Function: This function kills an agent into the platform. 
 * 		  	 Inputs: The Platform instance itself and the agent's AID.
 * 			Outputs: An error code indicating if killing the agent was successful.
 */
ERROR_CODE kill_agentFunction(Agent_Platform* platform, Agent_AID aid) {
	if (taskIdSelf() == platform->description.AMS_AID)
	{
		ERROR_CODE error;
		error = platform->deregister_agent(platform,aid);

		if (error == NO_ERRORS)
		{
			MAESAgent* a;
			Queue_ID m;

			a = (MAESAgent*)env.get_taskEnv(&env, aid);;
			a->agent.aid = NULL;
			m = a->agent.queue_id;
			msgQDelete(m);
			taskDelete(aid);
			
			env.erase_TaskEnv(&env,aid);
			platform->description.subscribers--;
		}
		return error;
	}
	else
	{
		return INVALID;
	}
};

/**
 *Suspend Agent Function: This function suspend an agent into the platform. 
 *		  	 Inputs: The Platform instance itself and the agent's AID.
 *			Outputs: An error code indicating if suspending the agent was successful.
 */
ERROR_CODE suspend_agentFunction(Agent_Platform* platform, Agent_AID aid) {
	if (taskIdSelf() == platform->description.AMS_AID)
	{
		if (platform->agent_search(platform,aid))
		{
			taskSuspend(aid);
			return (NO_ERRORS); 
		} else {
			return NOT_FOUND;
		}
	} else	{
		return INVALID;
	}
};

/**
 * Resume Agent Function: This function resumes an agent into the platform. 
 *			 Inputs: The Platform instance itself and the agent's AID.
 * 			Outputs: An error code indicating if resuming the agent was successful.
 */
ERROR_CODE resume_agentFunction(Agent_Platform* platform, Agent_AID aid) {
	if (taskIdSelf() == platform->description.AMS_AID)
	{
		if (platform->agent_search(platform,aid))
		{
			MAESAgent* a;
			a = (MAESAgent*)env.get_taskEnv(&env, aid);
			taskResume(aid);
			taskPrioritySet(aid, a->agent.priority);
			return NO_ERRORS;
		}
		else
		{
			return NOT_FOUND;
		}
	}
	else
	{
		return INVALID;
	}
};

/**
 *Restart Agent Function: This function restarts an agent into the platform. 
 *			 Inputs: The Platform instance itself and the agent's AID.
 * 			Outputs: None.
 */
void restartFunction(Agent_Platform* platform, Agent_AID aid) {
	if (taskIdSelf() == platform->description.AMS_AID)
	{
		MAESAgent* a;
		a = (MAESAgent*)env.get_taskEnv(&env, aid);
		Queue_ID m;
		// delete Task and Mailbox
		m = a->agent.queue_id; 
		taskDelete(aid);
		msgQDelete(m);
		env.erase_TaskEnv(&env,aid);
		// Mailbox = Queue
		a->agent.queue_id = msgQCreate(1,MAXmsgLength,MSG_Q_FIFO);
		
		// Task
		a->agent.aid=taskCreate(a->agent.agent_name, a->agent.priority,VX_FP_TASK,a->resources.stackSize,a->resources.function, a->resources.taskParameters, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		env.set_TaskEnv(&env,a->agent.aid, a); 
	}
};


/**
 * Agent Platform Constructor: This function assigns the class pointers to its corresponding function.
 *		 	  Inputs: Ponter to the Agent Platform class.
 *			 Outputs: None.
 */
void ConstructorAgent_Platform(Agent_Platform* platform, sysVars* env) {
	platform->ptr_env = env;
	platform->Agent_Platform = &Agent_PlatformFunction;
	platform->Agent_PlatformWithCond = &Agent_PlatformWithCondFunction;
	platform->boot = &bootFunction;
	platform->agent_init = &agent_initFunction;
	platform->agent_initConParam = &agent_initConParamFunction;
	platform->agent_search = &agent_searchFunction;
	platform->agent_wait = &agent_waitFunction;
	platform->agent_yield = &agent_yieldFunction;
	platform->get_running_agent = &get_running_agentFunction;
	platform->get_state = &get_stateFunction;
	platform->get_Agent_description = &get_Agent_descriptionFunction;
	platform->get_AP_description = &get_AP_descriptionFunction;
	platform->register_agent = &register_agentFunction;
	platform->deregister_agent = &deregister_agentFunction;
	platform->kill_agent = &kill_agentFunction;
	platform->suspend_agent = &suspend_agentFunction;
	platform->resume_agent = &resume_agentFunction;
	platform->restart_agent = &restartFunction;
};
