#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <math.h>
#include "graphic_api.h"
#include "uart_api.h"
#include "robot_protocol.h"
#include "amazon2_sdk.h"

int main(void){
  int ret;
  init_console();
  int i = 4000;
  U16* fpga_videodata = (U16*)malloc(180 * 120 * 2);
  U16* grayed_data;
  U16* processed_data;
  S16 maskX[9] = {1, 2, 1, 0, 0, 0, -1, -2, -1};
  S16 maskY[9] = {1, 0, -1, 2, 0, -2, 1, 0, -1};
  S32 gaussian_mask[9] = {113, 838, 113, 838, 6196, 838, 113, 838, 113};
  YUV* yuv_PixelData = (YUV*)malloc(12);
  uvset* decision_queue = (uvset*)malloc(8*5);
  uvset* spec = (uvset*)malloc(8);
  S16* p_radius[30];
  U16* p_theta[30];
  memset(p_radius, 0, 30*sizeof(S16));
  memset(p_theta, 0, 30*sizeof(U16));
  memset(decision_queue, 0, 10*sizeof(float));
  memset(spec, 0, 2*sizeof(float));
  int scale;
  ret = uart_open();
  if(ret <0)return EXIT_FAILURE;
  uart_config(UART1,9600,8,UART_PARNONE,1);
  Send_Command(2);
  Send_Command(2);
  TestItemSelectRobot();
  uart_close();
  return 0
}
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
