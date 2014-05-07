#include<avr/io.h>
#include<avr/interrupt.h>
#include<stdbool.h>
const int secondCount = 1000; //1 second
const int doorOpenSecondCount = 2;
static int doorOpenCount = 0;
static int timeCounter = 0;
short int currentFloor = 0;
bool floors[5] = {false,false,false,false,false};
bool direction = true;     // current set direction
                           // true = up
                           // false = down

bool movement = false;     // check if floors[] has any true states
                           // true = elevator is going up or down
                           // false = elevator is stopped
                       
bool doorOpen = false;     // check if door is opened
                           // true = door is open
                           // false = door is closed

bool floorPressed = false; // check if current floor was pressed
                           // true = current floor was pressed (execute stuff)
                           // false = current floor was not pressed (continue program)
void init(); 
void up();
void down();
void stop();
void open();
void close();
int checkForInput();
bool checkMovement(bool []);

ISR(TIMER0_OVF_vect)
{
    TCNT0 = 131;
    timeCounter++;

    if(timeCounter==secondCount) // 1 second has elapsed
    {
        timeCounter = 0;
        if(direction == true && movement == true) //elevator is going up
        {
            if(currentFloor == 4) direction = false;
            if(floors[currentFloor] == true)
            {
                floors[currentFloor] = false;
                floorPressed = true;
                stop();
                open();
                doorOpen = true;
            }
            currentFloor = currentFloor + 1;
        }

        else if(direction == false && movement == true) //elevator is going down
        {
            if(currentFloor == 0) direction = true;
            if(floors[currentFloor] == true)
            {
                floors[currentFloor] = false;
                floorPressed = true;
                stop();
                open();
                doorOpen = true;
            }
            currentFloor = currentFloor - 1;
        }
        else{;}

        if(doorOpen = true)
        {
            if(doorOpenCount == doorOpenSecondCount)
            {
                close();
                doorOpen = false;
                floorPressed = false;
                doorOpenCount = 0;
            }

            else if(doorOpenCount < doorOpenSecondCount)
            {
                doorOpenCount++;
            }

            else
            {
                doorOpenCount = 0;
            }
        }
    }

    else{;}
}

int main()
{
    int input = 0;
    init();
    while(1)
    {
        movement = checkMovement(floors);
        if(movement == false && (~floorPressed)) stop();
        else if(movement == true && (~floorPressed))
        {
            if(direction == true) // current direction is up
            {
                for(int i = currentFloor; i < 5; i++)
                {
                    if(floors[i] == true)
                    {
                        up();
                        break;
                    }
                    else
                    {
                        direction = false;
                    }
                }

            }

            else                        //current direction is down
            {
                for(int i = currentFloor; i > -1; i--)
                {
                    if(floors[i] == true)
                    {
                        down();
                        break;
                    }
                    else
                    {
                        direction = true;
                    }
                }		
            }
        }
        else{;}

        input = checkForInput();
        if(input != -1); //something valid was pressed
        {
            if(floors[input] == false && input != currentFloor) floors[input] = true;
        }
    }
}

void up()
{
    OCR1A = 1700; //elevator goes up
    OCR1B = 1000; //door stays closed
}

void down()
{
    OCR1A = 1300; //elevator goes down
    OCR1B = 1000; //door stays closed
}

void stop()
{
    OCR1A = 1500; //elevator stops
    OCR1B = 1000; //door stays closed
}

void open()
{
    OCR1A = 1500; //elevator stays still
    OCR1B = 2000; //door opens
}

void close()
{
    OCR1A = 1500; //elevator stays still
    OCR1B = 1000; //door closes
}

void init()
{
    //////////////////////////////
    //  START OF TIMER 0 CONFIG //
    //////////////////////////////

    TCCR0 = 0x02; //clock prescale 8
    TIMSK = 0x01; //unmask overflow interrupt
    TCNT0 = 131;  //initialize timer to 131

    ///////////////////////////
    // END OF TIMER 0 CONFIG //
    ///////////////////////////

    //////////////////////////////
    //  START OF TIMER 1 CONFIG //
    //////////////////////////////

    TCCR1A = 0b10100010; // compare match occurs timer = OCR1A
    TCCR1B = 0b00011001; // WGM13:WGM12=11; CS12:CS0=001: internal clock 1MHz, no prescaler

    ICR1  = 21500;       // period of PWM signal
    OCR1A = 1500;	     // high time of PWM for PD5 (controls duty cycle) controls up and down
    OCR1B = 1000;        // high time of PWM for PD4 (controls duty cycle) controls open and close

    ////////////////////////////
    //  END OF TIMER 1 CONFIG //
    ////////////////////////////	

    ////////////////////////////
    // START I/O PORTS CONFIG //
    ////////////////////////////

    DDRA = 0b00000000;  //port A = input for keypad
    DDRB = 0b11111111;  //port B = outputs for LEDs
    DDRD = 0b00110000;  //port D = enable output for PWM 
    //PD5 = OC1A
    //PD4 = OC1B
    /////////////////////////////
    // END OF I/O PORTS CONFIG //
    /////////////////////////////

    sei(); //enable interrupts
}

int checkForInput()
{
    DDRA = 0x0F;         //set four right-most bits to output, four left-most bits to input
    PORTA = 0x0F;        //setting all 1 to input bits, to determine row
    int row = PINA;      //read row value
    row &= 0xF0;         //mask lower four bits, to prevent error

    DDRA = 0xF0;         //set four right-most bits to input, four left-most bits to output
    PORTA = 0xF0;        //setting all 1 to input bits, to determine column
    int column = PINA;   //read column value
    column &= 0x0F;      //mask upper four bits, to prevent error

    int userInput = row | column;

    switch(userInput)
    {
        case 0x77: 			// 1
            return 1;
            break;

        case 0x7B:			// 2
            return 2;
            break;

        case 0x7D:			// 3
            return 3;
            break;

        case 0xB7: 			// 4
            return 4;
            break;

        case 0xBB: 			// 5
            return 5;
            break;

        default: 
            return -1;      // nothing was pressed or invalid.
    }
}

bool checkMovement(bool floorcheck[])
{
    for(int i = 0; i < 5 ; i++)
    {
        if(floorcheck[i] == true)
        {
            return true;
        }
    }

    return false;
}

void displayfloor(short int floor)
{
    switch(floor)
    {
        case 0: PORTB = 0b11111110;
                break;
        case 1: PORTB = 0b11111101;
                break;
        case 2: PORTB = 0b11111011;
                break;
        case 3: PORTB = 0b11110111;
                break;
        case 4: PORTB = 0b11101111;
                break;
        default: PORTB = 0b11111111;
                break;
     }
}
