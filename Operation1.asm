#include <avr/interrupt.h>  //it can substitute <avr/io.h> and has interruptions

const unsigned char tabela[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xFF,0xBF,0x8E};
    unsigned char digit[4]={10,0,12,12} ,SW,temp,i; //OFF
    volatile unsigned char cont=0, direction=1,cont_disp,pwm1;
    

    
void inicio(void){
    DDRA=0b11000000; //sw

    DDRC=0xff; //display
    PORTC=0xff;
    
    DDRB=0b11100000;  //motor
    PORTB=0b01000000; //sentido=1
    
    OCR0=77;
    TCCR0=0b00001111; //pre-scaler 128
    
    OCR2=0;
    TCCR2=0b01100011;
    
    TIMSK|=0b00000010;
    SREG|=0x80; //0b10000000 (SEI)
}

void display(void){
    
    PORTA=cont_disp<<6;
    PORTC=tabela[digit[cont_disp]];
    cont_disp++;
    if(cont_disp==4)
        cont_disp=0;
}


void P_disp(void){
    
    digit[2]=pwm1/10;
    digit[3]=pwm1%10;
    //digit[0]=10;
    if(direction==1)
        digit[1]=10;
    else
        digit[1]=11;
}

ISR(TIMER0_COMP_vect){        ////100ms///        case    0b00001111:        //default
        cont++;
        display();
}

void switches(void){
    SW=PINA & 0b00011111;
    switch(SW)
    {
        case 0b00001110:    //SW1
        cont=0;
        while(cont!=1);
        SW=PINA & 0b00011111;
        if(SW==0b00011110){
        pwm1=25;
        OCR2=pwm1*255/100;
        P_disp();}
        break;
        
        case 0b00011101:        //SW2
        cont=0;
        while(cont!=1);
        SW=PINA & 0b00011111;
        if(SW==0b00001101){
        pwm1=50;
        OCR2=pwm1*255/100;
        P_disp();}
        break;
        
        case 0b00011011:        //SW3
        cont=0;
        while(cont!=1);
        SW=PINA & 0b00011111;
        if(SW==0b00011011){
        pwm1=90;
        OCR2=pwm1*255/100;
        P_disp();}
        break;
        
        case  0b00010111:        //SW4
        cont=0;
        while(cont!=1);
        SW=PINA & 0b00011111;
        
        if(SW==0b00010111 && i==0){
        i=1;
        temp=OCR2;
        OCR2=0;
        cont=0;
        while(cont!=100);
        
        
        PORTB=0b00000000;        //braking
        
        if(direction==1)    {direction=0; PORTB=0b00100000;}        //direction 0
        else    {direction=1; PORTB=0b01000000;}
            
            OCR2=temp;    
            P_disp();        }    //direction 1}
        

        break;
/////////////////////////////////////////////////////////////////////////////
        case  0b00001111:        //SW5
        cont=0;
        while(cont!=1);
        SW=PINA & 0b00011111;
        
        if(SW==0b00010111 && i==0){
        i=1;
        temp=OCR2;
        OCR2=0;
        cont=0;
        while(cont!=100);
        
        
        PORTB=0b00000000;        //braking
        
        if(direction==1)    {direction=0; PORTB=0b00100000;}        //direction 0
        else    {direction=1; PORTB=0b01000000;}
            
            OCR2=temp;    
            P_disp();        }    //direction 1}
        

        break;
/////////////////////////////////////////////////////////////////////////////  
      
        case 0b00011111:
        i=0;
        break;
        
    }
    
}

int main(void)
{    
    start();
    while(1)
    {
        switches();
    }
    
    
    
}
