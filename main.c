#include "tm4c123gh6pm.h"
#include <stdint.h>
#include <stdio.h>
#include "UART.h"

struct State{
	uint8_t Output;
	uint32_t delay;
	const struct State *Next[2];
};
typedef const struct State State_t;

#define S5   &Stepper_FSM[0]
#define S6   &Stepper_FSM[1]
#define S10   &Stepper_FSM[2]
#define S9   &Stepper_FSM[3]
#define S11		&Stepper_FSM[4]

State_t Stepper_FSM[5] = { 
													{5,1000000,S6,S9},
													{6,1000000,S10,S5},
													{10,1000000,S9,S6},
													{9,1000000,S11,S6},
													{11,1000000,S5,S9},
												};

State_t *Pt;
#define STEPPER  (*((volatile uint32_t *)0x4000703C)) //PORTD Data
#define INPUT  (*((volatile uint32_t *)0x40006300))   //PORTC Data
												
void Delay(uint32_t delay_rounds)
{
	uint32_t i=0;
	for(i=delay_rounds;i>0;i--);
}
												
int main(void)
{
	volatile uint32_t Input;
	SYSCTL_RCGCGPIO_R |= 0x0C;  // 1) port C,D clock enabled
  Pt = S5;               
  GPIO_PORTD_LOCK_R = 0x4C4F434B;  // unlock GPIO Port D
  GPIO_PORTD_CR_R = 0xFF;          // allow changes to PD7-0
  GPIO_PORTD_AMSEL_R &= ~0x0F;      // 3) disable analog function
  GPIO_PORTD_PCTL_R &= ~0x0000FFFF; // 4) GPIO
  GPIO_PORTD_DIR_R |= 0x0F;   // 5) make PD3-0 out
  GPIO_PORTD_AFSEL_R &= ~0x0F;// 6) disable alt func on PD3-0
  GPIO_PORTD_DR8R_R |= 0x0F;  // enable 8 mA drive on PD3-0
  GPIO_PORTD_DEN_R |= 0x0F;   // 7) enable digital I/O on PD3-0
  GPIO_PORTC_AMSEL_R &= ~0xC0;      // 3) disable analog function PORTC
  GPIO_PORTC_PCTL_R &= ~0xFFF00000; // 4) GPIO
  GPIO_PORTC_DIR_R &= ~0xC0;  //    make PC7-6 input
  GPIO_PORTC_AFSEL_R &= ~0xC0;// 6) disable alt func on PC7-6
  GPIO_PORTC_DEN_R |= 0xC0;   // 7) enable digital I/O on PC7-6
	while(1){
    STEPPER = Pt->Output;           // Output
    Delay(Pt->delay); 						// Wait
    Input = (INPUT>>8);          // 0,1,2,3,4
    Pt = Pt->Next[Input];        // Next   
  }
}
