/*
 * OneShotBehaviour.c
 *
 *  Created on: Oct 26, 2023
 *      Author: blobo
 */

#include "VxMAES.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/**
 * Oneshot Action Function: This function is a wrapper that contains the core action of the agent's behavior. This function must be redefined in the main application.
 * 		 	 Inputs: The behavior instance itself and parameters (in case they are needed).
 *		 	Outputs: None.
 */
void actionFunctionOSB(OneShotBehaviour* Behaviour, MAESArgument taskParam,MAESArgument semaphoreX) {
	//Empty
};

/**
 * Oneshot Setup Function: This function is a wrapper that contains some lines of code that must be 
 * 						  executed before the action function is called. 
 * 			 Inputs: The behavior instance itself and parameters (in case they are needed).
 * 			Outputs: None.
 */
void setupFunctionOSB(OneShotBehaviour* Behaviour, MAESArgument taskParam) {
	//Empty
};

/**
 * Oneshot Done Function: This function determines if the behavior will be executed multiple times. 
 * 						  In the Oneshot case, this will always be true, meaning it will only be executed once.  
 * 			 Inputs: The behavior instance itself and parameters (in case they are needed).
 * 			Outputs: True bool meaning the behavior will be executed only once.
 */
bool doneFunctionOSB(OneShotBehaviour* Behaviour, MAESArgument taskParam) {
	return true;
};

/**
 * Oneshot Failure Detection Function: This function is a wrapper that can be used to develop an algorithm
 * 									  for failure detection.  
 * 		  	 Inputs: The behavior instance itself and parameters (in case they are needed).
 * 			Outputs: Bool value indicating if a failure was detected.
 */
bool failure_detectionFunctionOSB(OneShotBehaviour* Behaviour, MAESArgument taskParam) {
	return false;
};

/**
 * Oneshot Failure Identification Function: This function is a wrapper that can be used to develop an 
 * 										   algorithm for failure identification.  
 * 			 Inputs: The behavior instance itself and parameters (in case they are needed).
 * 			Outputs: None.
 */
void failure_identificationFunctionOSB(OneShotBehaviour* Behaviour, MAESArgument taskParam) {
	//Empty
};

/**
 * Oneshot Failure Recovery Function: This function is a wrapper that can be used to develop an algorithm 
 * 									 for failure recovery.  
 *			  Inputs: The behavior instance itself and parameters (in case they are needed).
 * 			 Outputs: None.
 */
void failure_recoveryFunctionOSB(OneShotBehaviour* Behaviour, MAESArgument taskParam) {
	//Empty
};

/**
 * Oneshot Execute Function: This function is a wrapper that determines the agent's behavior. This is the
 * 							 function that must be called from the main application.  
 * 			 Inputs: The behavior instance itself and parameters (in case they are needed).
 * 			Outputs: None.
 */ 
void executeFunctionOSB(OneShotBehaviour* Behaviour, MAESArgument taskParam,MAESArgument semaphoreX) {
	Behaviour->setup(Behaviour,taskParam);
//	SEM_ID mysem =semOpen("Semaphore",SEM_TYPE_COUNTING,3,SEM_Q_PRIORITY,OM_CREATE,0); 
	do
	{
//		semTake(mysem,WAIT_FOREVER);
		Behaviour->action(Behaviour, taskParam,semaphoreX);
		if (Behaviour->failure_detection(Behaviour, taskParam))
		{
			Behaviour->failure_identification(Behaviour, taskParam);
			Behaviour->failure_recovery(Behaviour, taskParam);
		}
//		semGive(mysem);
	} while (!Behaviour->done(Behaviour, taskParam));
};

/**
 * OneShot Behaviour Constructor: This function assigns the class pointers to its corresponding function.
 * 			 Inputs: Pointer to the OneShot Behaviour class.
 *			Outputs: None.
 */
void ConstructorOneShotBehaviour(OneShotBehaviour* Behaviour) {
	Behaviour->action = &actionFunctionOSB;
	Behaviour->setup = &setupFunctionOSB;
	Behaviour->done = &doneFunctionOSB;
	Behaviour->failure_detection = &failure_detectionFunctionOSB;
	Behaviour->failure_identification = &failure_identificationFunctionOSB;
	Behaviour->failure_recovery = &failure_recoveryFunctionOSB;
	Behaviour->execute = &executeFunctionOSB;
};
