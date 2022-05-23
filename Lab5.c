// Lab5.c starter program EE319K Lab 5, Spring 2022
// Runs on TM4C123
// Put your names here
// Last Modified: 1/11/2021

/* Option A1, connect LEDs to PB5-PB0, switches to PA5-3, walk LED PF321
   Option A2, connect LEDs to PB5-PB0, switches to PA4-2, walk LED PF321
   Option A6, connect LEDs to PB5-PB0, switches to PE3-1, walk LED PF321
   Option A5, connect LEDs to PB5-PB0, switches to PE2-0, walk LED PF321
   Option B4, connect LEDs to PE5-PE0, switches to PC7-5, walk LED PF321
   Option B3, connect LEDs to PE5-PE0, switches to PC6-4, walk LED PF321
   Option B1, connect LEDs to PE5-PE0, switches to PA5-3, walk LED PF321
   Option B2, connect LEDs to PE5-PE0, switches to PA4-2, walk LED PF321
  */
// east/west red light connected to bit 5
// east/west yellow light connected to bit 4
// east/west green light connected to bit 3
// north/south red light connected to bit 2
// north/south yellow light connected to bit 1
// north/south green light connected to bit 0
// pedestrian detector connected to most significant bit (1=pedestrian present)
// north/south car detector connected to middle bit (1=car present)
// east/west car detector connected to least significant bit (1=car present)
// "walk" light connected to PF3-1 (built-in white LED)
// "don't walk" light connected to PF3-1 (built-in red LED)
#include <stdint.h>
#include "SysTick.h"
#include "Lab5grader.h"
#include "tm4c123gh6pm.h"

struct State {
uint32_t Out;
uint32_t Delay;
const struct State *Next[4];};

typedef const struct State state;

#define south &fsm[0]
#define stopSouth &fsm[1]
#define west &fsm[2]
#define stopWest &fsm[3]

state fsm[4]={
{0x04, 100, { south, stopSouth, south, stopSouth }},
{0x03, 100, { west, west, west, west }},
{0x02, 100, { west, west, stopWest, stopWest }},
{0x01, 100, { south, south, south, south }}
};

#define BSP_OutPort GPIO_PORTB_DATA_R
#define BSP_InPort GPIO_PORTA_DATA_R

// put both EIDs in the next two lines
char EID1[] = "abc123"; //  ;replace abc123 with your EID
char EID2[] = "abc123"; //  ;replace abc123 with your EID

void DisableInterrupts(void);
void EnableInterrupts(void);

int main(void){
  DisableInterrupts();
  TExaS_Init(NONE);
  SysTick_Init();   // Initialize SysTick for software waits
	state *stater;
	uint32_t input;
	stater = 0;
  // initialize system
	SYSCTL_RCGCGPIO_R |= 0x03;

  GPIO_PORTB_DIR_R |= 0x3E;   // 5) make PD3-0 out
  GPIO_PORTB_DEN_R |= 0x3E;   // 7) enable digital I/O on PD3-0

  GPIO_PORTA_DIR_R &= ~0x30;  //    make PC7-6 input
  GPIO_PORTA_DEN_R |= 0x30;   // 7) enable digital I/O on PC7-6

  EnableInterrupts();
  while(1){
    // 1) output
		BSP_OutPort = (BSP_OutPort&(~0x3E))|((stater->Out)>>0x00);
    // 2) wait
		SysTick_Wait10ms(stater->Delay);
    // 3) input
		input = (BSP_InPort&0x30)>>0x00;
    // 4) next
		stater = stater->Next[input];

  }
}







