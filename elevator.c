
#define __DELAY_BACKWARD_COMPATIBLE__
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>



void checkForInput();
int checkUpDown();
bool checkIfEmpty();
int checkFloor();
void up();
void down();
void stop();
void open();
int checkFloor(int);
int calculate_column();
int calculate_row();
void init();
void displayfloor();


//Function Variables
int direction = 0;//no direction, 1 = up, 2 = down
int floors[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // 0 = no call, 1 = call
int currentfloor = 0;
int floordifference = 0;


//Functions
bool checkIfEmpty()
{
    for(int i = 0; i <= 9; i++)
    {
        if(floors[i] == 1)
        {
            return false;
        }
    }
    return true;
}

int checkUpDown()
{
    checkForInput();
    for(int i = currentfloor; i<10;i++)
    {
        if(floors[i] == 1)
        {
            direction = 1;
            return(i-currentfloor);
        }
    }
    for (int i =currentfloor; i>=0;i--)
    {
        if(floors[i] == 1)
        {
            direction = 2;
            return(currentfloor-i);
        }
    }
}

void stop()
{
    checkForInput();
    OCR1A = 1500;
    OCR1B = 1000;
    direction = 0;
}

void open()
{
    floordifference=0;
    floors[currentfloor] = 0;
    checkForInput();
    for (int i = 0; i<=20; i++)
    {
        OCR1A = 1500;
        OCR1B = 2000;
        _delay_ms(100);
        checkForInput();
    }
    OCR1A = 1500;
    OCR1B = 1000;
    floors[currentfloor] = 0;
}

void up()
{
    //
    //displayfloor(currentfloor);
    for (int i = 1; i<=(floordifference*20)+1; i++)
    {
        OCR1A = 1700;
        OCR1B = 1000;
        _delay_ms(100);
        if((i%20) == 0) currentfloor++;
        displayfloor();
        checkForInput();
    }
    OCR1A = 1500;
    OCR1B = 1000;
    
    
}

void down()
{
    checkForInput();
    for (int i = 1; i<=(floordifference*20)+1; i++)
    {
        OCR1A = 1300;
        OCR1B = 1000;
        _delay_ms(100);
        if((i%20) == 0) currentfloor--;
        displayfloor();
        checkForInput();
    }
    OCR1A = 1500;
    OCR1B = 1000;
}

int main()
{
    init();
    while(1)
    {
        do{
            checkForInput();
        } while(checkIfEmpty());
        
        floordifference = checkUpDown();
        if(floordifference == 0)
        {
            open();
        }
        else if((floordifference > 0) && (direction == 1))
        {
            up();
            open();
            stop();
        }
        
        else if((floordifference > 0) && (direction == 2))
        {
            down();
            open();
            stop();
        }
        else
        {
            stop();
        }
        
        direction = 0;
    }
}


void init()
{

    TCCR1A = 0b10100010; // compare match occurs timer = OCR1A
    TCCR1B = 0b00011001; // WGM13:WGM12=11; CS12:CS0=001: internal clock 1MHz, no prescaler

    ICR1  = 21500;       // period of PWM signal
    OCR1A = 1500;      // high time of PWM for PD5 (controls duty cycle) controls up and down
    OCR1B = 1000;        // high time of PWM for PD4 (controls duty cycle) controls open and close

    ////////////////////////////
    //  END OF TIMER 1 CONFIG //
    ////////////////////////////

    ////////////////////////////
    // START I/O PORTS CONFIG //
    ////////////////////////////

    DDRA = 0b00000000;  //port A = input for keypad
    DDRB = 0b11111111;  //port B = outputs for LEDs
    PORTB = 0b11111111;
    DDRD = 0b00110000;  //port D = enable output for PWM
    //PD5 = OC1A
    //PD4 = OC1B
    /////////////////////////////
    // END OF I/O PORTS CONFIG //
    /////////////////////////////

    sei(); //enable interrupts
}

void checkForInput()
{
    DDRA = 0x0F;         //set four right-most bits to output, four left-most bits to input
    PORTA = 0x0F;        //setting all 1 to input bits, to determine row
    int row = PINA;      //read row value
    row &= 0xF0;         //mask lower four bits, to prevent error

    DDRA = 0xF0;         //set four right-most bits to input, four left-most bits to output
    PORTA = 0xF0;        //setting all 1 to input bits, to determine column
    int column = PINA;   //read column value
    column &= 0x0F;      //mask upper four bits, to prevent error

    int userInput = (row | column);

    switch(userInput)
    {
        case 0x77:             // 1
        floors[0] = 1;
        break;

        case 0x7B:         // 2
        floors[1] = 1;
        break;

        case 0x7D:         // 3
        floors[2] = 1;
        break;

        case 0xB7:             // 4
        floors[3] = 1;
        break;

        case 0xBB:             // 5
        floors[4] = 1;
        break;
        
        case 0xBD:          // 6
        floors[5] = 1;
        break;

        case 0xD7:          // 7
        floors[6] = 1;
        break;

        case 0xDB:          // 8
        floors[7] = 1;
        break;


        case 0xDD:          // 9
        floors[8] = 1;
        break;
        
        case 0x7E:
        floors[9] = 1;
        break;

        default:
        break;      // nothing was pressed or invalid.
    }
    return;
}



void displayfloor()
{
    switch(currentfloor)
    {
        case 0: PORTB = 0xDD;
        break;
        case 1: PORTB = 0xA4;
        break;
        case 2: PORTB = 0x94;
        break;
        case 3: PORTB = 0xD1;
        break;
        case 4: PORTB = 0x92;
        break;
        case 5: PORTB = 0x82;
        break;
        case 6: PORTB = 0xDC;
        break;
        case 7: PORTB = 0x80;
        break;
        case 8: PORTB = 0x90;
        break;
        case 9: PORTB = 0xC0;
        break;
        default: PORTB = 0xFF;
        break;
    }
    
}
