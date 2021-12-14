//SEGUNDO PROJETO SEMANAL
//INTERRUP��ES
//baseado no exemplo EdgeInterrupt_4C123 do prof. Valvano

//m�quina de estados
//o estado apenas altera quando uma interrup��o pelo PF4 � chamada
//apenas no caso 2, quando nada � pressionado, vai automaticamente para 3

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers"
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2015
   Volume 1, Program 9.4
   
   "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014
   Volume 2, Program 5.6, Section 5.5

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

//bibliotecas que o prof. utiliza no exemplo
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "../inc/EdgeInterruptPortF.h"
#include "../inc/CortexM.h"
#include "../inc/PLL.h"
#include "../inc/TExaS.h"




//var�avel utilizada para contar as interrup��es
//tamb�m est� presente no watch para verifica��o
//contador2 para controle do sistema
extern volatile uint32_t FallingEdges;
//vezes2 � pra repetir a��o 5
extern unsigned long contador2,vezes2;


//defines para o registrador DATA
//cada define � deixa apenas modificar o bit respectivo de cada porta
//PF1 s� autoriza pelo DATA a modificar o segundo bit
//PD3 apenas altera estado do pino respectivo no PortD
#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
	
#define PD3       (*((volatile uint32_t *)0x40007020))

//prt�tipos das fun��es delay e inicializa��o
void PortD_Init(void);
void Delay(void); //prot�tipo da fun��o delay
void Delaypadrao(void);
void Delay25(void);

//rotina principal: inicializa��es das portas e m�quina de estados
int main(void){
	
	//inicializa��o da interrup��o + inic. PortD
  EdgeCounterPortF_Init();
  EnableInterrupts(); 
	PortD_Init();
	
	//m�quinas de estados
	//mudan�a controlada por FallingEdges
  while(1){ //rotina principal, onde os leds ficar�o piscando
    switch(FallingEdges)
    {
				//ESTADO 0
        case 0:
						//colocando as sa�das PF1, PF2 e PF3 (leds) em zero, ou seja, desligados
            PF1=0x00;
						PF2=0x00;
						PF3=0x00;
						//aplicando n�vel l�gico 0 na sa�da PD3 (s� ela foi autorizada a ter mudan�as)
						PD3=0x00;
            break;
				
				//ESTADO 1
        case 1:
					//pisca led azul com 2,5 Hz
						PF2=0x04;
						Delay25();
						PF2=0x00;
						Delay25();
						//colocando as sa�das PF1 e PF3 (leds) em zero, ou seja, desligados
            PF1=0x00;
						PF3=0x00;
						//aplicando n�vel l�gico 0 na sa�da PD3
						PD3=0x00;
            break;
						
				//ESTADO 2
				case 2:	
					//espera 15s, se PF4 � for pressionado, manda para estado 3
						Delay();
						FallingEdges++;
            break;
						
				//ESTADO 3
        case 3:
				//pisca 5x o led verde e quando o estado termina, pr�x � o 1
					while(contador2<5){	
						PF3=0x08;
						Delaypadrao();
						PF3=0x00;
						Delaypadrao();
						contador2++;
					}
						//colocando as sa�das PF1 e PF3 (leds) em zero, ou seja, desligados
            PF1=0x00;
						PF2=0x00;
						PF3=0x00;
            break;
						
				//ESTADO 4
        case 4:
						//led vermelho pisca 2x
						while(contador2<2){					//mesmo esquema dos anteriores
	            PF1 = 0x02;	//vermelho � PF1
							Delaypadrao();
							PF1 = 0x00;				
							Delaypadrao();
							contador2++;
						}
            break;	
						
				//ESTADO 5				
        case 5:		
						// led vermelho pisca 5x com F de 2,5 Hz		
						while(contador2<5){
	            PF1 = 0x02;						
							Delay25();
							PF1 = 0x00;				
							Delay25();
							contador2++;
						}
							vezes2++;//se estiver na primeira passagem, chama novamente
							//o caso 5, ou seja, repete a a��o
							if(vezes2==1)
							{
								contador2=0;
							}
						break;
				//ESTADO 6						
        case 6:		
						PD3 = 0x08;				//define a sa�da PD3 em n�vel l�gico alto
				//pisca os leds em sequ�ncia 4x
				while(contador2<4){
	            PF1 = 0x02;			//led vermelho			
							PF3 = 0x00;
							Delaypadrao();
							PF1 = 0x00;	
							PF2 = 0x04;			//led azul			
							Delaypadrao();
							PF3 = 0x08;	  //led verde	
							PF2 = 0x00;							
							Delaypadrao();
					contador2++;
						}
            break;
				return 0;
		}
	}
}


void Delay(void){unsigned long volatile time;
	//regra de tr�s para chegar em 0.5 segundo
  time = 5*727240*200/91;  // 1 sec
  while(time){
		time--;
  }
}

