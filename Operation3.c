#include <avr/interrupt.h>  //it can be replaced by <avr/io.h> and has interruptions
#include <stdio.h>
const unsigned char table[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xFF,0xBF,0x8E};
    char buffer[80];
    unsigned char digit[4]={10,0,12,12},c,tempc,tempT,temp,i,letter=0,capt_ant,dcicl,capt1,capt,capt2,overflow,overflow_ant,cont_overflow,cont_capt; //OFF
    volatile unsigned char cont=0, direction=1,cont_disp,pwm1,SW,flag_rx,character;
    

    
void start(void){
    DDRA=0b11000000; //sw

    DDRC=0xff; //display
    PORTC=0xff;
    
    DDRB=0b11100000;  //motor
    PORTB=0b01000000; //direction=1
    
    OCR0=77;
    TCCR0=0b00001111; //pre-scaler 128
    
    OCR2=0;
    TCCR2=0b01100011; //pre-scaler 64
    UBRR1H=0;
    UBRR1L=51;
    UCSR1A=0b00000000;
    UCSR1B=0b10011000;
    UCSR1C=0b00000110;


    
    TIMSK|=0b00000010;
    SREG|=0x80; //0b10000000 (SEI)
}


void dutycil(void){
    if(c==2){
        
        if(overflow==1){
            tempT=(65536-capt_ant)+capt1;
            temp=(65536-capt1)+capt2;
            
        }
        if(overflow==0 && overflow_ant==0)
        {
            temp=capt2-capt1;
            tempT=capt1-capt_ant;
            
        }
        if(overflow==0 && overflow_ant==1)
        {
            temp=capt2-capt1;
            tempT=(65536-capt_ant)+capt1;
        }
        capt=0;
        if(tempT==0){
            
            dcicl=0;
        }
        else{
            
            dcicl=((float)temp/tempT)*100;
            
        }
    }
}

ISR(TIMER1_CAPT_vect){
    if(cont_capt==0)
    {
        capt_ant=capt1;
        overflow_ant=cont_overflow;
        capt1=ICR1;
        cont_overflow=0;
        TCCR1B |=0b10111111;
        TIFR |=0b00100000;
    }
    if(cont_capt==1)
    {
        capt2=ICR1;
        TCCR1B &=0b01000000;
        TIFR |=0b00000000;
    }
    cont_capt++;
}
ISR(TIMER1_OVF_vect){

    cont_overflow++;
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
void display(void){
    
    PORTA=cont_disp<<6;
    PORTC=table[digit[cont_disp]];
    cont_disp++;
    if(cont_disp==4)
    cont_disp=0;
}



ISR(TIMER0_COMP_vect){        ////100ms///        case    0b00001111:        //default
    
        cont++;
        display();
}

ISR(USART1_RX_vect){
    
    caracter=UDR1;
    flag_rx=1;
}

void tx_buffer(void)
{
    
    unsigned char i=0;
    while(buffer[i]!=0){
        while((UCSR1A &(1<<UDRE1))==0);
        UDR1=buffer[i];
        i++;
    }
    
    
}
void switchpc(void){

        switch(character){
            case 'L' :
                if(direction==1)
                    {sprintf(buffer,"%d%%\r\n",pwm1);tx_buffer();}
                else
                    {
                    sprintf(buffer,"-%d%%\r\n",pwm1);
                    tx_buffer();
                    }
                break;
            case 'l':
                if(direction==1)
                    {sprintf(buffer,"%d%%\r\n",pwm1);tx_buffer();}
                else
                    {
                    sprintf(buffer,"-%d%%\r\n",pwm1);
                    tx_buffer();
                    }
                break;
            case 'P' :
                sprintf(buffer,"Engine stopped\r\n");
                tx_buffer();
                pwm1=0;
                OCR2=0;
            break;
            case 'p' :
                sprintf(buffer,"Engine stopped\r\n");
                tx_buffer();
                pwm1=0;
                OCR2=0;
                break;
            case '1':
                sprintf(buffer,"25%%\r\n");
                tx_buffer();
                pwm1=25;
                OCR2=pwm1*255/100;
                P_disp();
            break;
            case '2':
                sprintf(buffer,"50%%\r\n");
                tx_buffer();
                pwm1=50;
                OCR2=pwm1*255/100;
                P_disp();
            break;
            case '3':
                sprintf(buffer,"90%%\r\n");
                tx_buffer();
                pwm1=90;
                OCR2=pwm1*255/100;
                P_disp();
            break;
            default:
        break;}
        
    
    

}
void switches(void){
    SW=PINA & 0b00001111;

    switch(SW)
    {
        case 0b00001110:    //SW1
        cont=0;
        while(cont!=1);
        SW=PINA & 0b00001111;
        if(SW==0b00001110){
        pwm1=25;
        OCR2=pwm1*255/100;
        P_disp();}
        break;
        
        case 0b00001101:        //SW2
        cont=0;
        while(cont!=1);
        SW=PINA & 0b00001111;
        if(SW==0b00001101){
        pwm1=50;
        OCR2=pwm1*255/100;
        P_disp();}
        break;
        
        case 0b00001011:        //SW3
        cont=0;
        while(cont!=1);
        SW=PINA & 0b00001111;
        if(SW==0b00001011){
        pwm1=90;
        OCR2=pwm1*255/100;
        P_disp();}
        break;
        
        case  0b00000111:        //SW4
        cont=0;
        while(cont!=1);
        SW=PINA & 0b00001111;
        
        if(SW==0b00000111 && i==0){
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
        
        case 0b00001111:
        i=0;
        break;
        
    }
    
}

int main (void){    
    inicio();
    
    while(1)
    {
        
        switches();
        if(flag_rx==1)
        {    
            switchpc();
            flag_rx=0;
        }
    }  
}
