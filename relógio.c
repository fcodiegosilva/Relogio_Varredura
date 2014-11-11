#include <p18f4550.h>
#include <string.h>
#include <stdio.h>
#include <delays.h>

// Configuração do microcontrolador para execução de instruções

#pragma config FOSC     = HS    /// EXTERN CLOCK 8MHZ
#pragma config IESO     = OFF   /// INTERNAL/EXTERNAL OSCILATOR DISABLE
#pragma config PWRT     = OFF   /// DISABLE POWER-UP TIMER
#pragma config BORV     = 3     /// BROWN-OUT RESET MINIMUM
#pragma config WDT      = OFF   /// DISABLE WATCHDOG TIMER
#pragma config WDTPS    = 32768 /// WATCHDOG TIMER 32768s
#pragma config MCLRE    = OFF   /// MASTER CLEAR PIN (RE3) DISBALE
#pragma config LPT1OSC  = OFF   /// TIMER1 LOW POWER OPERATION
#pragma config PBADEN   = OFF   /// PORTB.RB0,1,2,3,4 AS I/O DIGITAL
#pragma config STVREN   = ON    /// STACK FULL/UNDERFLOW CAUSE RESET
#pragma config LVP      = OFF   /// DISABLE LOW VOLTAGE PROGRAM (ICSP DISABLE)
//#pragma interrupt intTimer0


//declarar as suas variaveis de hardware

#define c1 PORTBbits.RB3
#define c2 PORTBbits.RB2
#define c3 PORTBbits.RB1
#define l1 PORTBbits.RB4
#define l2 PORTBbits.RB5
#define l3 PORTBbits.RB6
#define l4 PORTBbits.RB7 //Pinos para leitura do keyboard

#define alarme PORTDbits.RD7

#define dis3 PORTAbits.RA2
#define dis4  PORTAbits.RA5
#define dis2 PORTEbits.RE0
#define dis1 PORTEbits.RE2


//#define Start_Bit 0

//declara as suas variaveis de software

short modo=0, setModo = 0, pisca = 0;
short dezena = -1, unidade=-1; 
unsigned short horaDez=0, horaUnid=0, minDez=0, minUnid=0, segDez=0, segUnid=0;
unsigned short aHoraDez=0, aHoraUnid=0, aMinDez=0, aMinUnid=0;


short VarrerKey(void){
					
	static unsigned char x = 0; 		
	
	TRISB = 0xf0; 						//seta linhas para saida e colunas para entrada
	
	EnablePullups();
	
//Rastrear coluna 1
	PORTB = 0xf7;						 //ativa coluna 1, desativa restante //(0b11111011)
	Nop();
	if(!l1)								//Varre a linha 1
		if(x==1) return -1; 			//Se 1 foi teclado, verifica se a tecla já foi tratada
		else{							//Se não
			TRISB = 0;					//Seta porta B para leitura
			x = 1;						//Seta x, informando que existe uma tecla a ser tratada
			return 1;					//Retorna o digito correspondente a tecla
		}
	if(!l2)								//A mesma lógica para as demais linhas e colunas
		if(x==1) return -1; 
		else{
			TRISB = 0;
			x = 1;
			return 4;
		}
	if(!l3)
		if(x==1) return -1;
	else{
		TRISB = 0;
		x = 1;
		return 7;
	}
	if(!l4)
		if(x==1) return -1; 
	else{
		TRISB = 0;
		x = 1;
		return -2;
	}
	
//Rastrear coluna 2
	PORTB = 0xfb; //ativa coluna 2, desativa restante
	Nop();
	if(!l1)
		if(x==1) return -1; 
	else{
		TRISB = 0;
		x = 1;
		return 2;
	}
	if(!l2)
		if(x==1) return -1; 
	else{
		TRISB = 0;
		x = 1;
		return 5;
	}
	if(!l3)
		if(x==1) return -1; 
	else{
		TRISB = 0;
		x = 1;
		return 8;
	}
	if(!l4)
		if(x==1) return -1; 
	else{
		TRISB = 0;
		x = 1;
		return 0;
	}
	
//Rastrear coluna 3
	PORTB = 0xfd;
	Nop();
	if(!l1)
		if(x==1) return -1; 
	else{
		TRISB = 0;
		x = 1;
		return 3;
	}
	if(!l2)
		if(x==1) return -1;
	else{
		TRISB = 0;
		x = 1;
		return 6;
	}
	if(!l3)
		if(x==1) return -1;
	else{
		TRISB = 0;
		x = 1;
		return 9;
	}
	if(!l4)
		if(x==1) return -1; 
	else{
		TRISB = 0;
		x = 1;
		return -3;
	}
	
	TRISB = 0;
	x = 0;
	return -1;
	
}

void inic_regs (void){
	
	TRISA = 0x00;
	TRISB =0x00;
	TRISC =0x00;
	TRISD =0x00;
	TRISE = 0x00; //seta portas como saida
	ADCON1 = 0x0F;
	PORTA = 0;
	PORTB = 0;
	PORTC = 0;
	PORTD = 0;
	PORTE = 0;  //Limpa portas

}

void ativaDisplay(unsigned char d1, unsigned char d2, unsigned char d3, unsigned char d4){
	
	static unsigned char atual = 1;
	const char numero[] = { 0x3F, 	//vetor com os codigos dos numeros para 7seg
							0x06,
							0x5B,
							0x4F,
							0x66,
							0x6D,
							0x7C,
							0x07,
							0x7F,
							0x67,
							0x00};
							
	
	//Se setModo estiver ligado, deixa o display apagado por meio segundo(contado pela variável pisca						
	if(pisca && setModo == 1 && (atual == 1 || atual == 2)) atual = 3;   
	else if(pisca && setModo == 2 && (atual == 3 || atual == 4)) atual = 1;
										
					
	dis1 = 0;						//Desativa todos os Displays
	dis2 = 0;
	dis3 = 0;
	dis4 = 0;

	if(atual == 1){					
		PORTD = numero[d1];			//coloca o valor solicitado na porta D
  		dis1 = 1;					//Acende o display 1
		atual = 2;					//Atualiza a variavel para na proxima interação acender o proximo display
		if(modo == 1) alarme = 1;	//se Modo = 1, ativa o ponto
	}
	else if(atual == 2){			//A mesma lógica para os demais displays
		PORTD = numero[d2];
		dis2 = 1;
		atual = 3;
		if(modo == 1) alarme = 1;
	}
	else if(atual == 3){
		PORTD = numero[d3];
		dis3 = 1; 
		atual = 4;
		if(modo == 2) alarme = 1;
	}
	else if(atual == 4){
		PORTD = numero[d4];
		dis4 = 1; 
		atual = 1;
		if(modo == 2) alarme = 1;
	}
	
	// Se modo = 0, compara hora e alarme, se igual, pisca os pontos do dispaly	
		if (modo == 0){
			if(horaDez==aHoraDez && horaUnid==aHoraUnid && minDez==aMinDez && minUnid==aMinUnid){  
				if(pisca) alarme = 0;
				else alarme = 1;
			}
		}
}


void trataKeyboad (void){  								//função para trarar as teclas do keyboard

	short x = 0;
	short i;		
		x = VarrerKey();								//chama a função varrer teclado
		if(x!=-1){										//Se x=-1, Nenhuma tecla foi teclada ou ainda não foi tratada
			 if(x==-2){              					//alterna entre os modos do relogio (* = -2)
				if(modo == 0) modo = 1; 				//modo=1, mostra MM:SS
				else if(modo == 1) modo = 2;			//modo=2, mostra alarme HH:MM
				else modo = 0;							//modo=0, mostra HH:MM
			}
			else if(x==-3){								//altera o setModo
				if(setModo == 0) setModo = 1; 			//modo=1, posivel setar a hora (relogio ou alarme)
				else if(setModo == 1) setModo = 2;		//modo=2, possivel setar o minuto (relogio ou alarme)
				else setModo = 0;						//modo=0, modo set desligado
			}			
			else for(i = 0; i<10 && setModo!=0; i++){  //Tratar o teclado numerico
					if(x==i){							//Valor digitado = i?
						if(dezena == -1) {				//dezena está vazia?
							dezena = i;					//associa o valor digitado a dezena
							i = 10;						//i > que 10, para sair do for
						}
						else {							//se não(dezena ja possui um valor)
							unidade = i;				//associa o valor digitado a unidade
							i=10;						//sai do for
						}
					}
				}
			
		}
		if(dezena>5) dezena = 0;						//se for informado um valor maior que 5 para dezena, sera tratado com 0
		if(dezena>=2 && unidade>3 && setModo==1)		//Se estiver editando a hora(setModo == 1), valores maiores que 23 seram tradados como zero
			unidade = dezena = 0;
}
	

void setRelogio (void){								//Funcão para atualizar a hora e alarme conforme informações do teclado
	
	if(setModo==1 && modo == 0){					//se setModo=1(seta hora) e modo = 0 (relogio)
		horaDez = dezena;							//atualiza dezena da hora
		horaUnid = unidade;							//atualiza unidade da hora
		setModo = 2;								//Atualiza setModo, ativando a ediçao dos minutos
		unidade = dezena = -1;						//zera as variaveis dezena e unidade
	}
	else if(setModo==2 && modo==0){					//O mesmo da função acima, mas trata os minutos
		minDez = dezena;
		minUnid = unidade;
		setModo = 0;
		unidade = dezena = -1;
	}
	else if(setModo==1 && modo == 2){				//se setModo=1(seta hora) e modo = 1 (alarme)
		aHoraDez = dezena;							//atualiza dezena da hora
		aHoraUnid = unidade;						//atualiza unidade da hora
		setModo = 2;								//Atualiza setModo, ativando a ediçao dos minutos
		unidade = dezena = -1;						//zera as variaveis dezena e unidade
	}
	else if(setModo==2 && modo==2){					//O mesmo da função acima, mas trata os minutos
		aMinDez = dezena;
		aMinUnid = unidade;
		setModo = 0;
		unidade = dezena = -1;
	}
	
}

void atualizaDisplay(void){
	
	if(modo == 0) ativaDisplay(horaDez,horaUnid,minDez,minUnid);			//Se modo = 0, mostra as horas
	else if(modo == 1) ativaDisplay(minDez,minUnid,segDez,segUnid);			//Se modo = 1, mostra MM:SS
	else if(modo == 2) ativaDisplay(aHoraDez,aHoraUnid,aMinDez,aMinUnid);	//Se modo = 2, mostra o alarme
}

void atualizaHora(void){  //Função para incrementar a hora

	segUnid++;
	if(segUnid>=10){
		segUnid=0;
		segDez++;
		if(segDez>=6){
			segDez=0;
			minUnid++;
			if(minUnid>=10){
				minUnid=0;
				minDez++;
				if(minDez>=6){
					minDez = 0;
					horaUnid++;
					if(horaUnid>=10){
						horaUnid=0;
						horaDez++;
					}
					else if(horaDez==2 && horaUnid==4){   
						horaDez=0;
						horaUnid=0;
					}
				}
			}
		}
	}
}

void main (void){
	
	unsigned char cont;
	inic_regs();
	
	while(1){
		
		trataKeyboad();									//função que ira varrer e tratar o keyboard
		if((setModo==1 || setModo==2) && unidade!=-1) 	//Função para setar a hora seguindo informações recebidas do teclado;
			setRelogio();
		
		Delay1KTCYx(8);  								//delay de 4ms

		if(cont!=240) cont++;   						//Incrementa contador
		else{											//Quando contador = 240
			cont=0;										//zera contador
			atualizaHora();								//Atualiza as variáveis da hora
			pisca = 0;									//zera variavel pisca
		}
		
		if(cont == 120) pisca = 1;						//ativa variavel pisca, na metade do contador, (meio segundo)
		atualizaDisplay();								//Função que verifica em que modo o relogio se encontra e atualiza display
		
	}
}
