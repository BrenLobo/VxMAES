/*
 * VxMAESconfig.h
 *
 *  Created on: Nov 19, 2023
 *      Author: blobo
 */

#ifndef VXMAESCONFIG_H_
#define VXMAESCONFIG_H_

/***************************************************/
/*        		    Definitions		  	           */
/***************************************************/

/* MAES | VxWorks Definitions*/
#ifndef MAEStskKMAJOR 
#define MAESTaskHandle_t TASK_ID           // Task Identifier
#define MAESQueueHandle_t MSG_Q_ID         // Message Queue
#define MAESArgument _Vx_usr_arg_t         // Parameter
#define MAESTickType_t _Vx_ticks_t         // Ticks type
#define MAESTaskFunction_t void*//FUNCPTR  // Function Pointer
#define MAESUBaseType_t  long unsigned int // integer
#define MAESStackSize size_t 			   // Stack type
#endif


/* MAES Definitions */
#define Agent_AID MAESTaskHandle_t		  // Agent ID
#define Mailbox_Handle MAESQueueHandle_t  // Agent's Queue ID

/***************************************************/
/*            User Adjustable Configuration        */
/***************************************************/

#define AGENT_LIST_SIZE 64				   // Maximum Agents per platform
#define MAX_RECEIVERS AGENT_LIST_SIZE - 1  // Maximum receivers available for any agent
#define BEHAVIOUR_LIST_SIZE 8			   // Behaviour list size
#define ORGANIZATIONS_SIZE 16			   // Maximum Members by Organization
#define MAESmaxPriority 100 			   // The Higher priority for application      
#define MAESminStacksize 50 			   // Minimum stack size
#define MAXmsgLength 100				   // Maximum Message Lenght (bytes)
#define ONE_MINUTE_IN_TICKS (sysClkRateGet() * 60)

#endif /* VXMAESCONFIG_H_ */
