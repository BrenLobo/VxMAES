/*
 * CyclicBehaviour.c
 *
 *  Created on: Oct 26, 2023
 *      Author: blobo
 */

#include "VxMAES.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/**
 * Cyclic Action Function: This function is a wrapper that contains the core action of the agent's behavior. This function must be redefined in the main application.
 * 		 	 Inputs: The behavior instance itself and parameters (in case they are needed).
 *		 	Outputs: None.
 */
void actionFunction(CyclicBehaviour* Behaviour, MAESArgument taskParam) {
	// Empty
};

/**
 *  Cyclic Setup Function: This function is a wrapper that contains some lines of code that must be 
 * 						  executed before the action function is called. 
 * 			 Inputs: The behavior instance itself and parameters (in case they are needed).
 * 			Outputs: None.
 */
void setupFunction(CyclicBehaviour* Behaviour, MAESArgument taskParam) {
	// Empty
};

/**
 * Cyclic Done Function: This function determines if the behavior will be executed multiple times. 
 * 						 In the Cyclic case, this will always be false, meaning it will be executed 
 * 						 indefinitely.  
 * 		  	 Inputs: The behavior instance itself and parameters (in case they are needed).
 * 			Outputs: True bool meaning the behavior will be executed indefinitely.
 */
bool doneFunction(CyclicBehaviour* Behaviour, MAESArgument taskParam) {
	return false;
};

/**
 * Cyclic Failure Detection Function: This function is a wrapper that can be used to develop an algorithm
 * 									  for failure detection.  
 * 		  	 Inputs: The behavior instance itself and parameters (in case they are needed).
 * 			Outputs: Bool value indicating if a failure was detected.
 */
bool failure_detectionFunction(CyclicBehaviour* Behaviour, MAESArgument taskParam) {
	return false;
};

/**
 * Cyclic Failure Identification Function: This function is a wrapper that can be used to develop an 
 * 										   algorithm for failure identification.  
 * 			 Inputs: The behavior instance itself and parameters (in case they are needed).
 * 			Outputs: None.
 */
void failure_identificationFunction(CyclicBehaviour* Behaviour, MAESArgument taskParam) {
	// Empty
};

/**
 * Cyclic Failure Recovery Function: This function is a wrapper that can be used to develop an algorithm 
 * 									 for failure recovery.  
 *			  Inputs: The behavior instance itself and parameters (in case they are needed).
 * 			 Outputs: None.
 */
void failure_recoveryFunction(CyclicBehaviour* Behaviour, MAESArgument taskParam) {
	// Empty
};

/**
 * Cyclic Execute Function: This function is a wrapper that determines the agent's behavior. This 
 * 						    function must be called from the application for its correct execution.  
 *			 Inputs: The behavior instance itself and parameters (in case they are needed).
 * 			Outputs: None.
 */
void executeFunction(CyclicBehaviour* Behaviour, MAESArgument taskParam) {
	Behaviour->setup(Behaviour, taskParam);
//	SEM_ID mysem =semOpen("Semaphore",SEM_TYPE_COUNTING,2,SEM_Q_PRIORITY,OM_CREATE,0); 
	
	do
	{
//		semTake(mysem,WAIT_FOREVER);
		Behaviour->action(Behaviour, taskParam);
		if (Behaviour->failure_detection(Behaviour, taskParam))
		{
			Behaviour->failure_identification(Behaviour, taskParam);
			Behaviour->failure_recovery(Behaviour, taskParam);
		}
//		semGive(mysem);
	} while (!Behaviour->done(Behaviour, taskParam));
	
};


/** 
 * Cyclic Behaviour Constructor: This function assigns the class pointers to its corresponding function.
 * 			 Inputs: PoInter to the Cyclic Behaviour class.
 * 			Outputs: None.
 */ 
void ConstructorCyclicBehaviour(CyclicBehaviour* Behaviour) {
	Behaviour->action = &actionFunction;
	Behaviour->setup = &setupFunction;
	Behaviour->done = &doneFunction;
	Behaviour->failure_detection = &failure_detectionFunction;
	Behaviour->failure_identification = &failure_identificationFunction;
	Behaviour->failure_recovery = &failure_recoveryFunction;
	Behaviour->execute = &executeFunction;
	
};

