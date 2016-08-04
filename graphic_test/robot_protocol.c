/************************************************************************
  Title     : Robot Body Protocol Source File
  File name : robot_protocol.c

  Author    : adc inc. (oxyang@adc.co.kr)
  History
		+ v0.0  2007/2/14
		+ v1.0  2008/8/6
************************************************************************/
#include <stdio.h>
#include <string.h>
#include "robot_protocol.h"
#include "uart_api.h"
#include "amazon2_sdk.h"
#include "graphic_api.h"
//////////////////////////////////////////////////// Protocol Test

int TestItemSelectRobot(void)
{
  char Item;

	while(1) {
 		PrintBannerRobot();
    Item = checkColorFromYUV;

		switch(Item) {
/*			case 'a' : case 'A' : Hello();				break;
			case 'b' : case 'B' : h_Forward_walk();		break;
			case 'c' : case 'C' : o_Forward_walk();		break;
			case 'd' : case 'D' : t_Forward_walk();			break;
			case 'z' : case 'Z' : return 0;*/
      case 'y' : h_Forward_walk(); break;
			default : printf("\nNo Test Item Selected");	break;
		}
	}
	return 0;
}
void DelayLoop(int delay_time)
{
	while(delay_time)
		delay_time--;
}

void Send_Command(unsigned char command)
{
	int i;
	unsigned char Command_Buffer[1] = {0,};

	Command_Buffer[0] = command;	// Command Byte
	 	for(i=0; i<1; i++) printf("0x%x ",Command_Buffer[i]);

	uart1_buffer_write(Command_Buffer, 1);
}

#define ERROR 0
#define OK	1

/* Command Function */
void Hello()
{
	Send_Command(1);

	DelayLoop(1000000);		// 3second delay
	printf("\nHello Ok!\n");
}

void h_Forward_walk()
{
	DelayLoop(1000000);
	Send_Command(2);

	DelayLoop(1000000);
	printf("\nHalf Forward walk Ok!\n");
}
void o_Forward_walk()
{
	Send_Command(3);

	DelayLoop(1000000);
	printf("\nOne Forward walk Ok!\n");
}

void t_Forward_walk()
{
	Send_Command(4);

	DelayLoop(1000000);
	printf("\nTwo Forward walk Ok!\n");
}
