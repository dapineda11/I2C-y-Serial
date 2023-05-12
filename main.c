#include <xc.h>
#include "init_system.h"
#include <pic18f47k42.h>
#include "funciones.h"
#include "generador.h"

#define RECIBIENDO_COMANDO 0
#define LECTURA_1_DATO 1    
#define ESCRITURA_1_DATO 2
#define LECTURA_N_DATO 3    
#define ESCRITURA_N_DATO 4

#define COMPARANDO_COMANDOS 0
//Reloj de 8M
//Velocidad de transmisión definida en el registro de reloj de I2C1CLK  500k 0011 
int z=0;
int CM; //Ir a linea 340
int num;
int data[16];
int i=0;
int banderaCM;
int RX;
int bitsRX=0;//Bits RX: saber que dato del comando es
int bitsDir=0;
int estados=0;
int dir;
unsigned int direccion;
short dato;
short entrada;
short entrada1;
long cont=0;

void main(void){
    init_system();
    while(1){
        
        switch(estados){
            case COMPARANDO_COMANDOS:
                if(banderaCM==1){
                if(CM==21058){ //Corremos la R (82) 8 bits y se hace un OR con la B (66) en ascii
                    RX=LECTURA_1_DATO;
                    banderaCM=0;
                }
                else if(CM==22338){
                    RX=ESCRITURA_1_DATO;//WB
                    banderaCM=0;
                }
                else if(CM==21075){
                    RX=LECTURA_N_DATO;//RS
                    banderaCM=0;
                }
                else if(CM==22355){
                    RX=ESCRITURA_N_DATO;//WS
                    banderaCM=0;
                }
                else{
                U1TXB=69;//Saca una E y una R en senal de error
                U1TXB=82;
                banderaCM=0;
                }
                }
                break;
            case LECTURA_1_DATO:
                switch(z){
                    case 0:
                I2C1CON0bits.EN = 1; //Enable del módulo I2C
                I2C1TXB=0xA0; //Byte de configuración de control del i2c
                //1010" binario para lectura y escritura.
				//operaciones. Los siguientes tres bits del byte de control son
				//los bits de selección de chip (A2, A1, A0). Los bits Chip Select permiten el uso de hasta ocho dispositivos
                I2C1CNT = 2; 
                /*
				Bits de registro de recuento de bytes
  						Si recibe datos, se reduce el octavo flanco de SCL, 
						cuando se carga un nuevo byte de datos en I2CxRXB. 
						Si se transmiten datos, se reduce el noveno flanco de SCL, 
						cuando se mueve un nuevo byte de datos de I2CxTXB*/
                I2C2CON0bits.S = 1; 
                /*
				Bit de inicio / reinicio maestro (solo modo <2: 0> = 1xx)
				Cuando MMA = 0
				1 = Establecido por el usuario del bit de INICIO o escribir en I2CTXB, 
				espera a que BFRE = 1 comience con un Inicio
				0 = Borrado por hardware después de enviar Start
				Cuando (MMA = 1 & MDR = 1 & pause_for_Restart)
				
				1 = Establecido por el usuario del bit de INICIO o escribir en I2CTXB, reanuda la comunicación con un reinicio
				0 = Borrado por hardware después de enviar Reiniciar
				*/
				/////////////////////////////////////////////////////////////////////////////////////////////
				//CONDICIÓN DE INICIO
				
				/*
				La condición inicial, de bus libre, es cuando ambas señales 
				están en estado lógico alto. En este estado cualquier dispositivo 
				maestro puede ocuparlo, estableciendo la condición de inicio (start). 
				Esta condición se presenta cuando un dispositivo maestro pone en estado bajo 
				la línea de datos (SDA), pero dejando en alto la línea de reloj (SCL).*/
                while(!I2C1STAT1bits.TXBE); //Transmitir búfer de estado vacío, 1 BUFER VACIO
                I2C1TXB=direccion>>8;
                while(!I2C1STAT1bits.TXBE);
                I2C1TXB=direccion;
                
                while(!I2C1STAT1bits.TXBE);
                I2C1CON0bits.RSEN=1; 
                /*
				1= Cuando (I2CCNT = 0 o ACKSTAT = 1), en el noveno SCL descendente establece MDR
				0 = Cuando (I2CCNT = 0 o ACKSTAT = 1), en el noveno SCL descendente; 
				el maestro cambia una condición de parada
				*/
                if(!I2C1CON1bits.ACKSTAT){
                	/*
					1 = No se recibió confirmación para la transmisión más reciente
					0 = Se recibió confirmación de la transmisión más reciente
					*/
                        z=1;
                        while(cont<=5){
                            cont++;
                        }
                        cont=0;
                    }       
                else{
                        z=3;
                    }
                break;
                    case 1:
                I2C1TXB=0xA1; //Direccion
                I2C1CNT = 1; 
                I2C2CON0bits.S = 1;
                I2C1CON0bits.RSEN=0;
                /*
				1= Cuando (I2CCNT = 0 o ACKSTAT = 1), en el noveno SCL descendente establece MDR
				0 = Cuando (I2CCNT = 0 o ACKSTAT = 1), en el noveno SCL descendente; 
				el maestro cambia una condición de parada
				*/
                while(!I2C1STAT1bits.TXBE);
                 //Transmitir búfer de estado vacío, 1 BUFER VACIO
                I2C1CON1bits.ACKDT=0;
                while(!I2C1STAT1bits.RXBF);                
                entrada=I2C1RXB;               
                z=2;
                break;
                    case 2:
                U1TXB=entrada;
                estados=COMPARANDO_COMANDOS;
                RX=RECIBIENDO_COMANDO;
                CM=0;
                z=0;
                break;
                    case 3:
                U1TXB=69;
                U1TXB=82;
                estados=COMPARANDO_COMANDOS;
                RX=RECIBIENDO_COMANDO;
                CM=0;
                z=0;
                break;      
                }
                
                break;
            case ESCRITURA_1_DATO:
                switch(z){
                    case 0:
                    	
                        I2C1CON0bits.EN = 1; // Enable module
                        I2C1CNT = 3; 
                        I2C2CON0bits.S = 1;
                        I2C1TXB=0xA0; //Direccion
                        while(!I2C1STAT1bits.TXBE);
                         //Transmitir búfer de estado vacío, 1 BUFER VACIO
                        I2C1TXB=direccion>>8;
                        while(!I2C1STAT1bits.TXBE);
                         //Transmitir búfer de estado vacío, 1 BUFER VACIO
                        I2C1TXB=direccion;
                        while(!I2C1STAT1bits.TXBE);
                         //Transmitir búfer de estado vacío, 1 BUFER VACIO
                        I2C1TXB=dato;    
                        while(!I2C1STAT1bits.TXBE);
                        if(!I2C1CON1bits.ACKSTAT){
                            z=1;
                        }       
                        else{
                            z=2;
                        }
                        break;
                    case 1:
                U1TXB=79; //OK
                U1TXB=75;
                estados=COMPARANDO_COMANDOS;
                RX=RECIBIENDO_COMANDO;
                CM=0;
                z=0;
                break;
                    case 2:
                U1TXB=69;
                U1TXB=82;
                estados=COMPARANDO_COMANDOS;
                RX=RECIBIENDO_COMANDO;
                CM=0;
                z=0;
                break;
                }
            break;
            case LECTURA_N_DATO:
            	//Comando, N datos, direccion inicial
                switch(z){
                    case 0:
                I2C1CON0bits.EN = 1;
                I2C1TXB=0xA0; //Direccion 
                I2C1CNT = 2; 
                I2C2CON0bits.S = 1; 
                while(!I2C1STAT1bits.TXBE);
                I2C1TXB=direccion>>8;
                while(!I2C1STAT1bits.TXBE);
                I2C1TXB=direccion;                
                while(!I2C1STAT1bits.TXBE);
                I2C1CON0bits.RSEN=1;
                /*
				1= Cuando (I2CCNT = 0 o ACKSTAT = 1), en el noveno SCL descendente establece MDR
				0 = Cuando (I2CCNT = 0 o ACKSTAT = 1), en el noveno SCL descendente; 
				el maestro cambia una condición de parada
				*/
                if(!I2C1CON1bits.ACKSTAT){
                        z=1;
                        while(cont<=5){
                            cont++;
                        }
                        cont=0;
                    }       
                else{
                        z=3;
                    }
                break;
                case 1:
                I2C1TXB=0xA1; //Direccion
                I2C1CNT = num+1; // manda la direccion del esclavo para empezar a recibir datos
                I2C2CON0bits.S = 1;
                I2C1CON0bits.RSEN=0;
                /*
				1= Cuando (I2CCNT = 0 o ACKSTAT = 1), en el noveno SCL descendente establece MDR
				0 = Cuando (I2CCNT = 0 o ACKSTAT = 1), en el noveno SCL descendente; 
				el maestro cambia una condición de parada
				*/
                while(!I2C1STAT1bits.TXBE);
                 //Transmitir búfer de estado vacío, 1 BUFER VACIO
                for(i=0;i<=num;i++){
                I2C1CON1bits.ACKDT=0;
                while(!I2C1STAT1bits.RXBF);                
                U1TXB=I2C1RXB;
                }
                z=2;
                break;
                    case 2:
                estados=COMPARANDO_COMANDOS;
                RX=RECIBIENDO_COMANDO;
                CM=0;
                z=0;
                break;
                    case 3:
                U1TXB=69;
                U1TXB=82;
                estados=COMPARANDO_COMANDOS;
                RX=RECIBIENDO_COMANDO;
                CM=0;
                z=0;
                break;
                }
                break;
            case ESCRITURA_N_DATO:
            	//Escribimos 16 datos seguidos
            	//1 direccion de esclavo
            	//2 Direcion de memoria
            	//3 dato
                switch(z){
                    case 0:
                I2C1CON0bits.EN = 1; // Enable module
                I2C1CNT = 18;
                I2C2CON0bits.S = 1;
                I2C1TXB=0xA0; //Direccion
                while(!I2C1STAT1bits.TXBE);
                dir=direccion+i;
                I2C1TXB=dir>>8;
                while(!I2C1STAT1bits.TXBE);
                I2C1TXB=dir;
                while(!I2C1STAT1bits.TXBE);
                for(i=0;i<16;i++){
                I2C1CON0bits.RSEN=0;
                /*
				1= Cuando (I2CCNT = 0 o ACKSTAT = 1), en el noveno SCL descendente establece MDR
				0 = Cuando (I2CCNT = 0 o ACKSTAT = 1), en el noveno SCL descendente; 
				el maestro cambia una condición de parada
				*/
                I2C1TXB=data[i];
                while(!I2C1STAT1bits.TXBE);
                I2C1CON0bits.RSEN=1;
                /*
				1= Cuando (I2CCNT = 0 o ACKSTAT = 1), en el noveno SCL descendente establece MDR
				0 = Cuando (I2CCNT = 0 o ACKSTAT = 1), en el noveno SCL descendente; 
				el maestro cambia una condición de parada
				*/
                if(i==14){
                I2C1CON0bits.RSEN=0;
                /*
				1= Cuando (I2CCNT = 0 o ACKSTAT = 1), en el noveno SCL descendente establece MDR
				0 = Cuando (I2CCNT = 0 o ACKSTAT = 1), en el noveno SCL descendente; 
				el maestro cambia una condición de parada
				*/
                }
                while(cont<=5){
                            cont++;
                        }
                        cont=0;
                }
                z=1;
                break;                
                    case 1:
                U1TXB=79;
                U1TXB=75;
                estados=COMPARANDO_COMANDOS;
                RX=RECIBIENDO_COMANDO;
                CM=0;
                z=0;
                    break;
                    case 2:
                U1TXB=69;
                U1TXB=82;
                estados=COMPARANDO_COMANDOS;
                RX=RECIBIENDO_COMANDO;
                CM=0;
                z=0;
                break;              
                }
                    
                
        }
}
}

void __interrupt() SUBRUTINA_INTERRUPCIONES(void){
     if(PIE3bits.U1RXIE==1  && PIR3bits.U1RXIF==1) // Verifica si la interrupcion de la recepcion esta habiltada y si la bandera de recepcion lista esta en 1
    {
        PIR3bits.U1RXIF=0; // Baja la bandera de la recepcion de la UART
        switch(RX){
            case RECIBIENDO_COMANDO:
                if(bitsRX==0){ //Bits RX: saber que dato del comando es
                CM=U1RXB<<8;//Buffer de recepcion de la UART
                bitsRX++;
                }
                else if(bitsRX==1){
                    CM=U1RXB+CM;
                    bitsRX=0;
                    banderaCM=1;
                }
                break;
            case LECTURA_1_DATO:
                if(bitsDir==0){
                direccion=U1RXB<<8;
                bitsDir++;
                }
                else if(bitsDir==1){
                direccion=U1RXB+direccion;
                bitsDir++;
                }
                if(bitsDir==2){
                bitsDir=0;
                estados=LECTURA_1_DATO;               
                }
                break;
            case ESCRITURA_1_DATO:
                if(bitsDir==0){
                direccion=U1RXB<<8;
                bitsDir++;
                }
                else if(bitsDir==1){
                direccion=U1RXB+direccion;
                bitsDir++;                
                }
                else if(bitsDir==2){
                    dato=U1RXB;
                    bitsDir++; 
                }
                if(bitsDir==3){
                bitsDir=0;
                estados=ESCRITURA_1_DATO;
                }
                break;
            case LECTURA_N_DATO:
                if(bitsDir==0){
                direccion=U1RXB<<8;
                bitsDir++;
                }
                else if(bitsDir==1){
                direccion=U1RXB+direccion;
                bitsDir++;                
                }
                else if(bitsDir==2){
                    num=U1RXB;
                    bitsDir++; 
                }
                if(bitsDir==3){
                bitsDir=0;
                estados=LECTURA_N_DATO;
                }
                break;
            case ESCRITURA_N_DATO:
                if(bitsDir==0){
                direccion=U1RXB<<8;
                bitsDir++;
                }
                else if(bitsDir==1){
                direccion=U1RXB+direccion;
                bitsDir++;                
                }
                else if(bitsDir==2){
                    data[i]=U1RXB;
                    i++;
                    if(i==16){
                        bitsDir++;
                        i=0;
                    }
                }
                if(bitsDir==3){
                bitsDir=0;
                estados=ESCRITURA_N_DATO;
                }
                break;
        }
    }
}
