/*****************************************************
This program was produced by the
CodeWizardAVR V1.24.8b Standard
Automatic Program Generator
© Copyright 1998-2006 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : 
Version : 
Date    : 24.08.2006
Author  : Andrey                          
Company : none                            
Comments: 


Chip type           : ATmega8L
Program type        : Application
Clock frequency     : 8,000000 MHz
Memory model        : Small
External SRAM size  : 0
Data Stack size     : 256
*****************************************************/

#include <mega8.h>
#include <delay.h>
#include <math.h>        

#define ADC_VREF_TYPE 0x60   

 
 #define Koef 4.4  // сколько сигм
 #define Razi 30 // сколько раз интегрируем 
 /*  
eeprom long int EE1;
eeprom long int EE2;
eeprom long int EE3;
eeprom long int EE4;
*/
  
unsigned char POG; // 1+4 // добавка к погрешности 1 импульс от дискретности + 1 имп от датчика  
unsigned int porog=169;  // 89; // соответствует 400 вольт на выходе   
unsigned char koridor=2; // диапазрон изменения U высокого.  
unsigned char TimH,TimL;


unsigned char IntegralFlag,AlarmFlag;
unsigned char InternalCounter=0;
unsigned char MircFlag=1;
unsigned long int MegaImpulsCounter,NulImpuls;;

unsigned long int Tpor;


unsigned long int OverTime=0; 

unsigned long int ImpulsCounter,LastImpuls;

// еже 2 екундный счёт 
interrupt [TIM1_CAPT] void timer1_capt_isr(void)
{ 

OverTime++;
InternalCounter++; 

if (OverTime>3600) 
{ IntegralFlag=0;
  OverTime=0;
  MegaImpulsCounter=0;
}

if (IntegralFlag<Razi) // считает до 10, суммирует загрузки
{
        MegaImpulsCounter=MegaImpulsCounter+ImpulsCounter;;
        IntegralFlag++; 
               
}
if (IntegralFlag==Razi) 
{
        Tpor=MegaImpulsCounter/Razi+POG + Koef*sqrt(MegaImpulsCounter/Razi+POG);
        PORTD.0=0;
        PORTD.1=0; 
        IntegralFlag++;
}  


if (ImpulsCounter==0)   // если ошибка по ХХ
{
NulImpuls++;
}  

if (NulImpuls>=5)
{
 #asm("cli")
                         OCR2=0; PORTD.0=0; PORTD.1=1;
                        while(1){} 
   
 #asm("sei")   
}

if (ImpulsCounter!=0) NulImpuls=0;  

 LastImpuls=ImpulsCounter;
 ImpulsCounter=0;  // Обнуление счётчика 
 
PORTD.7=1;
delay_ms(10);
PORTD.7=0;

}
  


interrupt [TIM0_OVF] void timer0_ovf_isr(void) // каждый импульс - прерывание
{   

 ImpulsCounter++;
 
 if (ImpulsCounter>Tpor) 
 { 
AlarmFlag=1;   
 }
   if ((IntegralFlag<Razi)&(IntegralFlag>=1))
   {
         MircFlag=MircFlag+0xFF/4; 
        if (MircFlag<128) 
        {
                PORTD.0=1;
                PORTD.1=0;
        }
        else
        {
                PORTD.0=0;
                PORTD.1=1;
        }    
    }
    
TCNT0=(255);   

 
}


// чтение АЦП
unsigned char read_adc(unsigned char adc_input)
{
ADMUX=adc_input|ADC_VREF_TYPE;
ADCSRA|=0x40;
while ((ADCSRA & 0x10)==0);
ADCSRA|=0x10;
return ADCH;
}

   

void main(void)
{
   

unsigned int inADC,oldADC, i,LampFlash2;    
float PWM=1; 
float addADC;   

PORTB=0x00;
DDRB=0x08;

PORTC=0x00;
DDRC=0x00;

PORTD=0b01011000;
DDRD=0b10000111;
   
//  таймера 0 - стоп.
TCCR0=0x00;
TCNT0=0; 

// Timer 1 - стоп
TCCR1A=0x00;
TCCR1B=0x00;
TCNT1H=0x00;
TCNT1L=0x00;  
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;


// Таймер 2 - ШИМ   

ASSR=0x00;
TCCR2=0x6A; 
TCNT2=0; 
OCR2=1;  

// External Interrupt(s) initialization
// INT0: Off
// INT1: Off
MCUCR=0x00;


// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=0b00100001;

// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 1: Off
ACSR=0x80;
SFIOR=0x00;  
          

// ADC initialization
ADMUX=ADC_VREF_TYPE;
ADCSRA=0x86;      




  
// Global enable interrupts
#asm("sei")

// ******************************************************  
 
   NulImpuls=0;
   ImpulsCounter=0;
   MegaImpulsCounter=0;
   IntegralFlag=Razi+1; 
       
       PWM=1;
       OCR2=PWM;   


TimH=0x7A;
TimL=0x12; 

POG=1+4;
      
    if (PIND.6==0)
    {
     
                            for (i=0;i<255;i++)
                      {
                         PORTD.2=1;
                         delay_ms(1);
                         PORTD.2=0;
                         delay_ms(1);
                      }    
     
     TimH=0xF4;
     TimL=0x24; 
     POG=1+4*2;                   
    }
   
// ******* Повышене напряжения ***********************  
    
 oldADC=0;
while ((oldADC<=porog)&(PWM<(128)))
{
     PWM=PWM+1;  
     OCR2=PWM; 

                     
    addADC=0;
    i=0;
   while (i<300) 
   {
   inADC=read_adc(5);
   addADC=addADC+inADC;
   i=i+1;
   }  
   i=0;
   while (i<300) 
   {
   inADC=read_adc(5);
   addADC=addADC+inADC;
   i=i+1;
   }  
        
   oldADC=addADC/600;         
}  
//********************************************      
   
   
     

 //*****************************************************************************
 //******** работа по контролю дозы *****************   

// Пуск таймера 0 - счёт импульсов.
TCCR0=0x06;
TCNT0=(255); 
     

// Пуск таймера 1 - счет времени   31250 kHz раз в 2 секунды
TCCR1A=0x00;
TCCR1B=0x1C;
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=TimH;    //F4
ICR1L=TimL;    // 24
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;
               

   ImpulsCounter=0;
   MegaImpulsCounter=0;
   IntegralFlag=0;
   Tpor=0xFFFFFF;   
   
 while (IntegralFlag<=1); 
 
 Tpor=LastImpuls+POG+5*sqrt(LastImpuls+POG);  
 
  
   AlarmFlag=0;
   
   
   
                    


while(1)
{   


     
     if (IntegralFlag>Razi)
     { 
     PORTD.1=0;   // красная не горит       
     if (LampFlash2<35) {PORTD.0=0;}
     if (LampFlash2==35) { PORTD.0=1; }
     if (LampFlash2==38) { PORTD.0=0;}
     if (LampFlash2==41) {PORTD.0=1;}
     if (LampFlash2==44) {PORTD.0=0;};
     if (LampFlash2==47) {PORTD.0=1;}; 
     if (LampFlash2==50) {PORTD.0=0;};  
     if (LampFlash2>50) {LampFlash2=0;}  
     LampFlash2++; 
     }
              //*** регул напряжения
        addADC=0;
         i=0;
        while (i<300) 
                 {
                        inADC=read_adc(5);
                        addADC=addADC+inADC;
                        i=i+1;
                 }  
    
                 oldADC=addADC/300;     
                 if  (oldADC<(porog-koridor)) 
                 {   
                        PWM=PWM+0.1;  
                        OCR2=PWM; 
                 } 
                 if  (oldADC>(porog+koridor)) 
                 {
                        PWM=PWM-0.1;  
                        OCR2=PWM;   
                 }    

                if (PWM>128) //
                {
                        OCR2=0; PORTD.0=0; PORTD.1=1;
                        while(1){} 
                }  
                   
                
                   
                if (PIND.3==0)
                { 
                PORTD.1=1;
                PORTD.0=0;        
                  InternalCounter=0;
                  while ((InternalCounter<5)&(PIND.3==0));
                  if (PIND.3==0)
                        {
                                Tpor=0xFFFFFF;
                                IntegralFlag=0;
                                MegaImpulsCounter=0; 
                                ImpulsCounter=0;
                                
                        } 
                  PORTD.0=1;
                  PORTD.1=0;
                  while (PIND.3==0);
                  PORTD.0=0;
                  PORTD.1=0;                       
                }    
                
                
                if (AlarmFlag==1) 
                {
                    #asm("cli")
                  
               //  EE1=Tpor;
               //  EE2=LastImpuls;
                
                   PORTD.1=1;
                   PORTD.0=0;  
                   
                 //  EE3=Tpor;
                 //  EE4=LastImpuls;
                       
                
                
                   while (AlarmFlag==1)
                    { 
                    
                  
                      for (i=0;i<255;i++)
                      {
                         PORTD.2=1;
                         delay_ms(1);
                         PORTD.2=0;
                         delay_ms(1);
                         if (PIND.3==0) AlarmFlag=0;
                      }
 
                       for (i=0;i<255;i++)
                      {
                         PORTD.2=0;
                         delay_ms(1);
                         PORTD.2=0;
                         delay_ms(1);
                         if (PIND.3==0) AlarmFlag=0;
                      }                     
                         
                   } 
                
                   AlarmFlag=0; 
                           ImpulsCounter=0;
                                           
                   PORTD.0=0;
                   PORTD.1=0;
                   if ( IntegralFlag<10 ) 
                   {

                           IntegralFlag=0;  
                           MegaImpulsCounter=0;   
                   }
                     
                #asm("sei")   
                
             }     


}

    

while (1);   
      
}
