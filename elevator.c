#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

//Function Headers
void up(); //Timer 1
void down(); //Timer 1
void stop(); //Timer 1
void open(); //Timer 2
void close(); //Timer 2
void checkUpDown(bool [], bool*, int);
bool checkIfEmpty(bool []);
void checkCurrentFloor(int, bool []);

//Function Variables
bool up=true;
bool floor[8] = {false, false, false, false, false, false, false, false};
int currentfloor = 0;


//Functions
void checkCurrentFloor(int thisfloor, bool floors[],bool direction)
{
    if(floors[thisfloor] == true)
    {
        stop();
        open();
        close();
        floor[thisfloor] = false;
    }
    else
    {
        if(direction)
        {
    
    }
}

bool checkIfEmpty(bool check[])
{
    for(int i = 0; i <= 7; i++)
    {
        if(check[i] == true)
        {
            return true;
        }

        else
        {
            return false;
        }
    }
}

void checkUpDown(bool check[], bool* direction, int currfloor)//only called when buffer is not empty
{
    if(*direction)
    {
        for(int i = currfloor; i <= 7; i++)
        {
            if(floor[i] == true)
            {
                *direction = true
            }
        }
    }

    *direction = false;
}

void up(int* updatefloor)
{
    OCR1A = 1705;
    counter = 0;
    TCNT0 = 131;
    loopcounter = 0;
    updatefloor += 1;
}

void down(int* updatefloor)
{
    OCR1A = 1305;
    counter = 0;
    TCNT0 = 131;
    loopcounter = 0;
    updatefloor -= 1;
}

void stop()
{
    OCR1A = 1505;
}

int main()
{
    init();
    while(1)
    {
        checkForInput();
        if(~checkIfEmpty())
        {
            checkUpDown();
            if(up)
            {
                up();
            }
            else
            {
                down();
            }
            checkCurrentFloor();
        }
        else
        {
            stop();
        }
    }
}



void init()
{
    //set port A as input for keypad
    DDRA = 0x00;
    //setting port B as outputs for LEDs
    DDRB = 0xFF;
    //initialize port B
    PORTB = 0b00000000;
    
    DDRD = 0b00100000;        // set pin D.5 for output (OC1A)
    
    //setting up timer
    TCCR0 = 0x02; //clock prescale = 8
    TIMSK = 0x01; //unmask overflow interrupt
    TCNT0 = 131; //initialize timer to 131
    
    // WGM11:WGM10   = 10: with WGM13-WGM12 to select timer mode 1110
    //                     Fast PWM, timer 1 runs from 0 to ICR1
    // COM1A1:COM1A0 = 10: clear OC1A when compare match, set OC1A when 0
    TCCR1A = 0b10000010; // compare match occurs timer = OCR1A
    TCCR1B = 0b00011001; // WGM13:WGM12=11; CS12:CS0=001: internal clock 1MHz, no prescaler
    ICR1 = 21500;
    OCR1A = 1505;
    // enable interrupts
    sei();
}

void checkForInput()
{
    int row = calculate_row();
    int column = calculate_column();
    int inputfloor = row|column;
    int floorpressed = 0;
    if(inputfloor == 0xFF)
    {
        return;
    }
    
    else
    {
        if(testforzeros(inputfloor)==2)
        {
            floorpressed = checkFloor(inputfloor);
            if((currentFloor < floorpressed) && upDown == false) //going up
            {
                floor[floorpressed-1] = true;     
            }
            
            else if(currentFloor > floorpressed) && upDown == true) //going down
            {
                floor[floorpressed-1] = true;
            }
            
            else
            {
                return;
            }
        }
                
    }       
}
    

int calculate_row()
{
    DDRA = 0x0F;         //set four right-most bits to output, four left-most bits to input
    PORTA = 0x0F;        //setting all 1 to input bits, to determine row
    int row = PINA;      //read row value
    row &= 0xF0;         //mask lower four bits, to prevent error
    return row;          //return row value
}

//Calculate column function
int calculate_column()
{
    DDRA = 0xF0;         //set four right-most bits to input, four left-most bits to output
    PORTA = 0xF0;        //setting all 1 to input bits, to determine column
    int column = PINA;   //read column value
    column &= 0x0F;      //mask upper four bits, to prevent error
    return column;       //return column value
}

//Checks if more than 1 button is being pressed.
int testforzeros(int hold)
{
    int numberofzeros = 0;
    int test = hold;
    for (int count = 0; count <= 7; count++)
    {
        if(((test>>count) & 0x01)==0)
        {
            numberofzeros++;
        }
    }
    return numberofzeros;
}

int checkFloor(int hold)
{
    switch(hold)
    {
        case 0x77:          // 1
            return 1;
        
        case 0x7B:          // 2
            return 2;
        
        case 0x7D:          // 3
            return 3;
            
        case 0xB7:          // 4
            return 4;

        case 0xBB:          // 5
            return 5;
               
        case 0xBD:          // 6
            return 6;

        case 0xD7:          // 7
            return 7;
        
        case 0xDB:          // 8        
            return 8;
            
        default: 
            return -1;
    }
}

void setFloorLED(int hold)
{
    switch(hold)
    {
        case 1:          // 1
            PORTB = PORTB|0b00000001;
            break;
        
        case 2:          // 2
            PORTB = PORTB|0b00000010;
            break;
        
        case 3:          // 3
            PORTB = PORTB|0b00000100;
            break;
            
        case 4:          // 4
            PORTB = PORTB|0b00001000;
            break;

        case 5:          // 5
            PORTB = PORTB|0b00010000;
            break;
            
        case 6:          // 6
            PORTB = PORTB|0b00100000;
            break;
b
        case 7:          // 7
            PORTB = PORTB|0b01000000;
            break;
        
        case 8:          // 8        
            PORTB = PORTB|0b10000000;
            break;
            
        default: 
            PORTB = PORTB;
            break;
    }
}
