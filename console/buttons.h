#ifndef __buttons_h
#define __buttons_h

#define PE0   			(*((volatile uint32_t *)0x40024004))
#define PE1   			(*((volatile uint32_t *)0x40024008))
#define PE2   			(*((volatile uint32_t *)0x40024010))

/***************** Buttons_Init ****************
	Initializes Port E as button inputs
	Inputs: none
	Outputs: none
*/
void Buttons_Init(void);

#endif
