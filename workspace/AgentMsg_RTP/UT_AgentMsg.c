/*
 * UT_AgentMsg.c
 *
 *  Created on: Oct 12, 2023
 *      Author: blobo
 */

/* includes */
//#include <vxWorks.h>
#include "VxMAES.h"
#include <string.h>

/*Defining the app's variables*/

MAESAgent AgentA, AgentB, AMS_test, AgentC, AgentD, AgentE,AgentF;
sysVars env;
Agent_Msg MsgA, MsgB, MsgX, MsgF;


/*Task functions*/

int hello(void){
	printf("Hello this is a test.\n");
	//taskDelay(100);
	return 0;
}

int task1() {
	TASK_ID runningtask=taskIdSelf();
	MsgA.caller=runningtask;
	printf("Executing task (ID: %d)\n", runningtask);
    while (1) {
        printf("Task is running\n");
        taskDelay(1 * sysClkRateGet()); // Espera 1 segundo
    }
}

int taskE(void) {
	TASK_ID taske=taskIdSelf();
	MsgX.caller=taske;
	printf("Executing  task (ID: %d)\n", taske);
	
	env.set_TaskEnv(&env,taske, &AgentE);
	AgentE.agent.AP = AMS_test.agent.aid;
	printf("AGENT E AP %d\n", AgentE.agent.AP);

//	printf("\n*******REGISTER REQUEST FUNCTION********\n");
//	
//	ERROR_CODE agentregister = MsgX.registration(&MsgX,AgentD.agent.aid);
//		
//	if(agentregister==NO_ERRORS){
//		printf("Agent registration request success\n");
//	}else if(agentregister==HANDLE_NULL){
//		printf("Agent don't exist.\n");
//	}else if(agentregister==INVALID){
//		printf("Agent registration request fails.");
//	}
//	
//	printf("\n*******DEREGISTER REQUEST FUNCTION********\n");
//	
//	ERROR_CODE agentderegister = MsgX.deregistration(&MsgX,AgentD.agent.aid);
//			
//	if(agentderegister==NO_ERRORS){
//		printf("Agent deregistration request success\n");
//	}else if(agentderegister==HANDLE_NULL){
//		printf("Agent don't exist.\n");
//	}else if(agentderegister==INVALID){
//		printf("Agent deregistration request fails.");
//	}
//		

//	printf("\n*******SUSPEND REQUEST FUNCTION********\n");
//	ERROR_CODE agentsuspend = MsgX.suspend(&MsgX,AgentD.agent.aid);
//			
//	if(agentsuspend ==NO_ERRORS){
//		printf("Agent suspend request success\n");
//	}else if(agentsuspend ==HANDLE_NULL){
//		printf("Agent don't exist.\n");
//	}else{
//		printf("Agent suspend request fails.");
//	}
//
//	printf("\n*******RESUME REQUEST FUNCTION********\n");
//	ERROR_CODE agentresume= MsgX.resume(&MsgX,AgentD.agent.aid);
//	
//	if(agentresume==NO_ERRORS){
//		printf("Agent resume request success\n");
//	}else if(agentresume ==HANDLE_NULL){
//		printf("Agent don't exist.\n");
//	}else{
//		printf("Agent resume request fails.");
//	}
	
//	printf("\n*******KILL REQUEST FUNCTION********\n");
//	ERROR_CODE agentkill= MsgX.kill(&MsgX,AgentD.agent.aid);
//	
//	if(agentkill==NO_ERRORS){
//		printf("Agent kill request success\n");
//	}else if(agentkill ==HANDLE_NULL){
//		printf("Agent don't exist.\n");
//	}else{
//		printf("Agent kill request fails.");
//	}
//	
	printf("\n*******RESTART REQUEST FUNCTION********\n");
	ERROR_CODE agentrestart = MsgX.restart(&MsgX);
	
	if(agentrestart==NO_ERRORS){
		printf("Agent restart request success\n");
	}else if(agentrestart ==HANDLE_NULL){
		printf("Agent don't exist.\n");
	}else{
		printf("Agent restart request fails.");
	}
	
	return 0;
	
}

int taskF(void) {
	TASK_ID taskf=taskIdSelf();
	MsgF.caller=taskf;
	printf("Executing  task (ID: %d)\n", taskf);
	
	env.set_TaskEnv(&env,taskf, &AgentF);
	AgentF.agent.AP = AMS_test.agent.aid;
	printf("AGENT F AP %d\n", AgentF.agent.AP);
	MsgF.receive(&MsgF,WAIT_FOREVER);
	
//    while (1) {
//    	
//        printf("Task is running\n");
//        taskDelay(1 * sysClkRateGet()); // Espera 1 segundo
//    }
	
}

/*  Main */

int main () {
	printf(" Unit Testing for Agent Class\n\n");
	
	//Agent, Msg and env constructor
	ConstructorAgente(&AgentA);
	ConstructorAgente(&AgentB);
	ConstructorAgente(&AMS_test);
	ConstructorAgente(&AgentC);
	ConstructorAgente(&AgentD);
	ConstructorAgente(&AgentE);
	ConstructorAgente(&AgentF);
	
	ConstructorSysVars(&env);
	
	ConstructorAgent_Msg(&MsgA, &env);
	ConstructorAgent_Msg(&MsgB, &env);
	ConstructorAgent_Msg(&MsgX, &env);
	ConstructorAgent_Msg(&MsgF, &env);
	
	// Agent init
	AgentA.Iniciador(&AgentA,"Agent A",62,20);
	AgentB.Iniciador(&AgentB,"Agent B",63,20);
	AMS_test.Iniciador(&AMS_test,"AMS Test",60,20);
	AgentC.Iniciador(&AgentC,"Agent C",64,20);
	AgentD.Iniciador(&AgentD,"Agent D",65,20);
	AgentE.Iniciador(&AgentE,"Agent E",65,20);
	AgentF.Iniciador(&AgentE,"Agent E",65,20);
	
	//Defining the agent's aid
	AgentA.agent.aid=taskSpawn(AgentA.agent.agent_name,AgentA.agent.priority,0,AgentA.resources.stackSize, hello,0,0,0,0,0,0,0,0,0,0);
	AgentB.agent.aid = taskSpawn(AgentB.agent.agent_name,AgentB.agent.priority,0,AgentB.resources.stackSize, hello,0,0,0,0,0,0,0,0,0,0);
	AgentD.agent.aid = taskSpawn(AgentD.agent.agent_name,AgentD.agent.priority,0,AgentD.resources.stackSize, hello,0,0,0,0,0,0,0,0,0,0);
	AMS_test.agent.aid = taskCreate(AMS_test.agent.agent_name,AMS_test.agent.priority,0,AMS_test.resources.stackSize, hello,0,0,0,0,0,0,0,0,0,0);
	taskActivate(AMS_test.agent.aid);	
	//environment set                   
	env.set_TaskEnv(&env,AgentA.agent.aid, &AgentA);
	env.set_TaskEnv(&env,AgentB.agent.aid, &AgentB);
	env.set_TaskEnv(&env,AMS_test.agent.aid, &AMS_test);
	//env.set_TaskEnv(&env,AgentC.agent.aid, &AgentC); Se comenta porque aun no se ha creado
	env.set_TaskEnv(&env,AgentD.agent.aid, &AgentD);
	
	//***********************************************
	//************** IS REGISTERED*****************
	//***********************************************
	
	//setting the AP aid as env
	printf("AMS agent.aid: %d\n",AMS_test.agent.aid);
	AgentA.agent.AP = AMS_test.agent.aid;
	AgentB.agent.AP = AMS_test.agent.aid;
	AgentC.agent.AP = AMS_test.agent.aid;
	AgentD.agent.AP = AMS_test.agent.aid;
	
	printf("AgentA.AP %d\n",AgentA.agent.AP);
	
	// set Agent B as the caller
	MsgA.caller=AgentB.agent.aid;
	
	printf("\n*******IS REGISTERED FUNCTION********\n");
	if (MsgA.isRegistered(&MsgA,AgentA.agent.aid)==1){
		printf("Agent registration success.\n");
	} else {
		printf("Agent registration fails.\n");
	}
	
	//***********************************************
	//************** GET MAILBOX *****************
	//***********************************************
	
	printf("\n*******GET MAILBOX FUNCTION********\n");
	
	// the queue id is created
	AgentB.agent.mailbox_handle=msgQCreate(1,sizeof(MsgObj),MSG_Q_FIFO);//MSG_Q_PRIORITY
	
	//the mailbox handle is obtained directly from the agent B
	int agenteBmail= AgentB.agent.mailbox_handle;
	printf("The mailbox Agent B is (verify): %d\n",agenteBmail);
	
	if (MsgA.get_mailbox(&MsgA,AgentB.agent.aid)==agenteBmail){
			printf("Mailbox handle verification success: %d\n",MsgA.get_mailbox(&MsgA,AgentB.agent.aid));
		} else {
			printf("Mailbox handle verification fails: %d\n",MsgA.get_mailbox(&MsgA,AgentB.agent.aid));
		}
	
	//***********************************************
	//************** ADD RECEIVER *****************
	//***********************************************
	
	printf("\n*******ADD RECEIVER FUNCTION********\n");
	
	// the agent is subscribed
	ERROR_CODE msgRev=MsgA.add_receiver(&MsgA,AgentB.agent.aid);
	if (msgRev==NO_ERRORS){
		printf("Adding receiver function success %d\n",MsgA.subscribers);
	}else{
		printf("Adding receiver function fails.\n");
	}
	
	// the amount of subscribers is full
	MsgA.subscribers=63;
	ERROR_CODE listA=MsgA.add_receiver(&MsgA,AgentB.agent.aid);
	if (listA==LIST_FULL){
		printf("The amount of subscribers is full %d\n",MsgA.subscribers);
	}else{
		printf("The amount of subscribers is not full.\n");
	}
	
	
	
	// COLOCAR un Agente sin registrar no muestra nada en consola
//	printf("%s",AgentC.agent.aid);
//	MsgA.subscribers=0;
//	ERROR_CODE noaid = MsgA.add_receiver(&MsgA,AgentC.agent.aid);
//	
//	printf("The amount of subscribers is not full.\n");
//	printf("%d",noaid);
//	
//	printf("The amount of subscribers is not full.\n");
//	if (noaid==NOT_FOUND){
//		printf("The agent is not found %d\n",MsgA.subscribers);
//	}else{
//		printf("The agent is found.\n");
//	}
	
	//***********************************************
	//********** CLEAR ALL RECEIVERS ****************
	//***********************************************
	
	printf("\n*******CLEAR ALL RECEIVERS FUNCTION********\n");
	
	//Agent B is already registered
	
	MsgA.clear_all_receiver(&MsgA);
	
	int isEmpty=1;
	for (int i = 0; i < 63; i++) {
		if (MsgA.receivers[i] != NULL) {
			isEmpty = 0; // the char isn't empty
			break;
		}
	}
	if (isEmpty){
		printf("The receivers register has been cleaned\n");
	} else{
		printf("The receivers register hasn't been cleaned\n");
	}
	
	//***********************************************
	//**************  AGENT MESSAGE ****************
	//***********************************************
	
	printf("\n*******AGENT MESSAGE FUNCTION********\n");
	/* the test is not done because it have to 
	 * exist an agent executing. The execution of
	 * the test of taskIdSelf is done inside the
	 * function of Agent C. */
	
	// subscribers and receivers setting
	MsgA.subscribers=0;
	MsgA.add_receiver(&MsgA,AgentB.agent.aid);
	
	//instance of agent C
	AgentC.agent.aid = taskSpawn(AgentC.agent.agent_name,AgentC.agent.priority,0,AgentC.resources.stackSize, task1,0,0,0,0,0,0,0,0,0,0);
	taskSuspend(AgentC.agent.aid);
	
	//MsgA.Agent_Msg(&MsgA); //
	printf("The agent C is suspended");
	
	//***********************************************
	//**************  REMOVE RECEIVER ****************
	//***********************************************
	
	printf("\n******* REMOVE RECEIVER FUNCTION ********\n");
	// Add receivers  --- the agent B is already added
	MsgA.caller=AgentA.agent.aid;
	MsgA.add_receiver(&MsgA,AgentD.agent.aid);
	
	//number of subscribers is 2
	ERROR_CODE msgRemoveB= MsgA.remove_receiver(&MsgA,AgentB.agent.aid);
	if (msgRemoveB==NO_ERRORS){
		printf("Agent B remove success %d\n",MsgA.subscribers);
	}else{
		printf("Agent B remove fails.\n");
	}
	
	// when there is not found the aid of the agent in the receivers char
	ERROR_CODE msgRemoveC= MsgA.remove_receiver(&MsgA,AgentC.agent.aid);
	if (msgRemoveC==NOT_FOUND){
			printf("The agent C isn't found\n");
	}else{
		printf("The result is invalid.\n");
	}
		
	//***********************************************
	//**************  REFRESH LIST ****************
	//***********************************************
	
	printf("\n******* REFRESH LIST FUNCTION ********\n");
	
	//adding receivers
	MsgX.caller=AgentA.agent.aid;
	MsgX.add_receiver(&MsgX,AgentB.agent.aid);
	MsgX.add_receiver(&MsgX,AgentD.agent.aid);
	
		
	MsgX.refresh_list(&MsgX);
	
	int isEmptyRL=1;
	for (int i = 0; i < 63; i++) {
		if (MsgX.receivers[i] != NULL) {
			isEmptyRL = 0; // the char isn't empty
			break;
		}
	}
	if (isEmptyRL){
		printf("The receivers register has been cleaned\n");
	} else{
		printf("The receivers register hasn't been cleaned\n");
	}
	printf("\n******* REFRESH LIST didn't function ********\n");
	
	//***********************************************
	//***********************************************
	printf("\n******* RECEIVE FUNCTION ********\n");
	//inicializa el agente
	AgentF.Iniciador(&AgentF,"Agent F",67,20);	
	// crea el ID del mailbox del agente
	AgentF.agent.mailbox_handle=msgQCreate(1,sizeof(MsgObj),MSG_Q_FIFO);//MSG_Q_PRIORITY
	
	/* se crea la tarea, se define el caller que corresponde al ID de la tarea en
	 * ejecucion agenteF.  Luego registra el agente en el ambiente y se define el AP
	 *
	 * 	MsgF.receive(&MsgB,WAIT_FOREVER);
	 *
	 * */
	AgentF.agent.aid = taskSpawn(AgentF.agent.agent_name,AgentF.agent.priority,0,AgentF.resources.stackSize,taskF,0,0,0,0,0,0,0,0,0,0);
	//***********************************************
	//**************  SEND FUNCTION ****************
	//***********************************************
	printf("\n******* SEND FUNCTION ********\n");
		
	//adding msg caller and receivers
	MsgB.caller=AgentB.agent.aid;
	MsgB.add_receiver(&MsgB,AgentA.agent.aid);
	MsgB.add_receiver(&MsgB,AgentF.agent.aid);
	
	//setting msg
	char* msgXcontent= "Hello test";	
	MsgB.set_msg_content(&MsgB,msgXcontent);
	
	// adding Agent handle
	//AgentD.agent.mailbox_handle=msgQCreate(1,sizeof(MsgObj),MSG_Q_FIFO);//MSG_Q_PRIORITY
	
	ERROR_CODE sendmsgB = MsgB.send1(&MsgB,AgentF.agent.aid,WAIT_FOREVER);
	if(sendmsgB==NOT_REGISTERED){
		printf("The agent is not registered.\n");
	}else if(sendmsgB==TIMEOUT){
		printf("Sending message verification fails.\n");
	}else if(sendmsgB==NO_ERRORS){
		printf("Sending message verification success. \n");
		if (strcmp((const char*)MsgF.msg.content,(const char*)msgXcontent) == 0){
			printf("Msg content same\n");
		}else {
			printf("Msg content diff\n");
		}
	}else{
		printf("Sending message verification invalid. \n");
	}
	
	//AgentB.agent.mailbox_handle=msgQCreate(1,sizeof(MsgObj),MSG_Q_PRIORITY);//MSG_Q_FIFO
	
	// using the function
	//MsgB.receive(&MsgB,WAIT_FOREVER);	
	
	//^^^^^^^^^^^^^^^^^^^^^^
	taskSuspend(AgentF.agent.aid);
	char* contenido=MsgF.msg.content;
	printf("%s\n",contenido);

	//***********************************************
	//**************  SEND0 FUNCTION ****************
	//***********************************************
	
	//***********************************************
	//**********  SET MSG TYPE FUNCTION **************
	//***********************************************
	printf("\n******* SET MSG TYPE FUNCTION ********\n");
	
	
	MsgX.set_msg_type(&MsgX,AGREE);
	if (MsgX.msg.type==AGREE){
		printf("Set message type verification success.\n");
	}else{
		printf("Set message type verification fails.\n");
	}
	
	//***********************************************
	//**********  SET MSG CONTENT FUNCTION **************
	//***********************************************
	printf("\n******* SET MSG CONTENT FUNCTION ********\n");
		//se coloca arriba
//	char* msgXcontent= "Hello test";	
	MsgX.set_msg_content(&MsgX,msgXcontent);
	
	if(strcmp((const char*)MsgX.msg.content,(const char*)msgXcontent) == 0){
		printf("Set message content verification success.\n");
	}else{
		printf("Set message content verification fails.\n");
	}
	
	//***********************************************
	//**********     GET MSG FUNCTION **************
	//***********************************************
	printf("\n******* GET MSG FUNCTION ********\n");
	
	MsgObj* ptrMsgX= MsgX.get_msg(&MsgX);
	printf("%d\n",&ptrMsgX); //&MsgX.msg no sirve comparar con eso
	// SE CONFIRMA POR EL MOMENTO CON DATOS QUE SE OBSERVANEN COMPILACION
	
	//***********************************************
	//**********  GET MSG TYPE FUNCTION **************
	//***********************************************
	printf("\n******* GET MSG TYPE FUNCTION ********\n");
	
	
	if (MsgX.get_msg_type(&MsgX)==AGREE){
		printf("Get message type verification success\n");
	}else{
		printf("Get message type verification fails.\n");
	}
	
	//***********************************************
	//**********  GET MSG CONTENT FUNCTION **************
	//***********************************************
	printf("\n******* GET MSG CONTENT FUNCTION ********\n");
	if (strcmp((const char*)MsgX.get_msg_content(&MsgX),(const char*)msgXcontent) == 0){
		printf("Get message content verification success. \n");
	}else{
		printf("Get message content verification fails.\n");
	}
	
	
	//***********************************************
	//**********  REGISTRATION/ DESREGISTER/*********
	//**** SUSPEND/RESUME/KILL/RESTART FUNCTION *****
	//***********************************************
	
	printf("\n******* REQUEST FUNCTION ********\n");
	/*The taskE have the function of all of then
	 * the test is made commenting the functions*/

	// agente E
	
	AgentE.Iniciador(&AgentE,"Agent E",67,20);	
	AgentE.agent.mailbox_handle=msgQCreate(1,sizeof(MsgObj),MSG_Q_FIFO);//MSG_Q_PRIORITY
	AMS_test.agent.mailbox_handle=msgQCreate(1,sizeof(MsgObj),MSG_Q_FIFO);//MSG_Q_PRIORITY
	//env.set_TaskEnv(&env,AgentE.agent.aid, &AgentE);
	
	AgentE.agent.aid = taskSpawn(AgentE.agent.agent_name,AgentE.agent.priority,0,AgentE.resources.stackSize,taskE,0,0,0,0,0,0,0,0,0,0);


	return 0;
}



	


