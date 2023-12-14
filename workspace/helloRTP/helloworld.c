/* helloworld.c - helloworld example RTP */

/*
 * Copyright (c) 2003-2004, 2012 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01c,09jan12,pad  Removed non-standard parameters from main() (defect #233278).
01b,01oct04,job   Changed name. Updated to WR docs format
01a,30sep03,job   written
*/

/*
SYNOPSIS

\cs
helloworld.vxe
\ce

DESCRIPTION

The canonical Hello World example application 

This application writes 'Hello World' on the console and exits. 

\NOROUTINES
*/ 

#include <stdio.h>

/*******************************************************************************
* main - User application entry function
*
* This routine is the entry point for user application. A real time process
* is created with first task starting at this entry point. 
* This function writes 'Hello World' on the console and exits. 
*
*/

int main
    (
    int	   argc,	/* number of arguments */
    char * argv[]	/* array of arguments */
    )
    {
    printf("Hello World\n");

    return 0;
    }
