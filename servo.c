#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

const unsigned int elevatorcounter = 2000;//CHANGE VALUE LATER;
const unsigned int doorcounter = 1000;
static unsigned int counter = 0;
unsigned char button;
bool door = false;
bool elevatorStop = true;
int main(void)
{
    
    DDRA = 0x00; DDRB = 0xFF; // set port A for input, port B for output
    DDRD = 0b00110000;        // set pin D.5 for output (OC1A)
    
    //setting up timer
    TCCR0 = 0x02; //clock prescale = 8
    TIMSK = 0x01; //unmask overflow interrupt
    TCNT0 = 131; //initialize timer to 131
    
    sei();
    // WGM11:WGM10   = 10: with WGM13-WGM12 to select timer mode 1110
    //                     Fast PWM, timer 1 runs from 0 to ICR1
    // COM1A1:COM1A0 = 10: clear OC1A when compare match, set OC1A when 0
    TCCR1A = 0b10100010; // compare match occurs timer = OCR1A
    TCCR1B = 0b00011001; // WGM13:WGM12=11; CS12:CS0=001: internal clock 1MHz, no prescaler

    ICR1  = 21500;                   // period of output PWM signal
    OCR1A = 1500;
    OCR1B = 1500;                     // set high time of output PWM signal
    
    while (1)
    {
        button = PINA;
        
        if(button == 0b01111111 && door != true)
        {
            OCR1A = 1700; //elevator start
            OCR1B = 1500; //door stop
            TCNT0 = 131;
            counter = 0;
            elevatorStop = false;
        }

        else if(door == true && elevatorStop == false)
        {
            OCR1A = 1500; //elevator stop
            OCR1B = 1700; //door start
            if(~elevatorStop)
            {
                TCNT0 = 131;
                counter = 0;
                elevatorStop = true;
            }
        }
        
        else{}
    }
}

ISR(TIMER0_OVF_vect)
{
    TCNT0 = 131;
    counter++;

    if(counter == elevatorcounter && door == false)
    {
        OCR1A = 1500;
        OCR1B = 1500;
        counter = 0;
        door = true;
    }
    
    else if(counter == doorcounter && door == true)
    {
        OCR1A = 1500;
        OCR1B = 1500;
        counter = 0;
        door = false;
    }
}
