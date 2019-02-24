//Source: http://dduino.blogspot.com/2011/11/arduino-ps2-emulator-computer-control.html

//KBD stuff
#include <ps2dev.h>  // to emulate a PS/2 device
PS2dev keyboard(3,2);  // PS2dev object (2:data, 3:clock)
int enabled = 0;  // pseudo variable for state of "keyboard"
unsigned char c;  //char stores data recieved from computer for KBD

//Sonar stuff
const int numReadings = 5;   // set a variable for the number of readings to take
int index = 0;                // the index of the current reading
int total = 0;                // the total of all readings
int average = 0;              // the average
int oldAverage = 0;           // the old average
int echoPin = 6;              // the SRF05's echo pin
int trigPin = 7;              // the SRF05's init pin
unsigned long pulseTime = 0;  // variable for reading the pulse
unsigned long distance = 0;   // variable for storing distance

void ack()
{
  //acknowledge commands
  while(keyboard.write(0xFA));
}

int keyboardcommand(int command)
{
  unsigned char val;
  switch (command)
  {
  case 0xFF: //reset
    ack();
    //the while loop lets us wait for the host to be ready
    while(keyboard.write(0xAA)!=0);
    break;
  case 0xFE: //resend
    ack();
    break;
  case 0xF6: //set defaults
    //enter stream mode
    ack();
    break;
  case 0xF5: //disable data reporting
    //FM
    enabled = 0;
    ack();
    break;
  case 0xF4: //enable data reporting
    //FM
    enabled = 1;
    ack();
    break;
  case 0xF3: //set typematic rate
    ack();
    keyboard.read(&val); //do nothing with the rate
    ack();
    break;
  case 0xF2: //get device id
    ack();
    keyboard.write(0xAB);
    keyboard.write(0x83);
    break;
  case 0xF0: //set scan code set
    ack();
    keyboard.read(&val); //do nothing with the rate
    ack();
    break;
  case 0xEE: //echo
    //ack();
    keyboard.write(0xEE);
    break;
  case 0xED: //set/reset LEDs
    ack();
    keyboard.read(&val); //do nothing with the rate
    ack();
    break;
  }
  return 0;
}

void setup()
{
  // send the keyboard start up
  while(keyboard.write(0xAA)!=0);
  delay(10);
  // make the init pin an output:
  pinMode(trigPin, OUTPUT);
  // make the echo pin an input:
  pinMode(echoPin, INPUT);
  // initialize the serial port:
  Serial.begin(9600);
}

void loop()
{
  // loop for a number of readings on the SRF-05 to get an average to smooth the results. Much like all my other examples
  for (index = 0; index<=numReadings;index++)
  {
    digitalWrite(trigPin, HIGH);
    delay(1);
    digitalWrite(trigPin, LOW);
    pulseTime = pulseIn(echoPin, HIGH);
    distance = pulseTime/58;
    total = total + distance;
    delay(5);
  }
  // store the previous reading
  oldAverage = average;
  // store the current reading
  average = total/numReadings;
  // debug to check for spikes in the sensor etc..
  Serial.println(average);

  //if host device wants to send a command:
  if( (digitalRead(3)==LOW) || (digitalRead(2) == LOW))
  {
    while(keyboard.read(&c)) ;
    keyboardcommand(c);
  }
  else
  {
    // now the fun part...
    // if my distance is less than 5...
    if (average <=5)
    {
      int x = 0;
      if(x<=5)  //how many times to press tab
      {
        Serial.println("TAB+ALT");
        keyboard.write(0x11); //send ALT
        keyboard.write(0x0D); //then send Tab

        keyboard.write(0xF0);  //tab break
        keyboard.write(0x0D);  //...
        delay(700); // wait so doesnt tab too fast

        if(x<=5)
        {
          keyboard.write(0x0D); //send another Tab
          keyboard.write(0xF0);  //tab break
          keyboard.write(0x0D);  //...
          delay(700);
        }
      }
      else //x is nolonger <=5 must be 6, set x to 0, and end all keys
      {
        x=0; //reset tab counter
        keyboard.write(0xF0);  //tab break
        keyboard.write(0x0D);  //...

        keyboard.write(0xF0);  //alt break
        keyboard.write(0x11);  //...

      }
      x++; //add 1 to tab counter
    }
    else // otherwise if my hand is higher
    {
      // check to see if my hand is in the registered space above the sensor
      if (average <=20 && average >=10 && oldAverage >=10)
      {
        // the below statement is our sensitive the readings are so if the current and previous readings are different with a tolerance of +/
        //we can look at the direction of movement
        if ((average != oldAverage) && (average+1 != oldAverage) && (average-1 != oldAverage))
        {
          // if the current reading is higher than the previous, then my hand is moving upwards
          if (average > oldAverage)
          {
            Serial.println("Sound up");
            keyboard.write(0xE0); //make volume +
            keyboard.write(0x32); //...
            keyboard.write(0xE0); //break
            keyboard.write(0xF0); //...
            keyboard.write(0x32); //...
            delay(150);  //wait so it doesnt change volume rapidly
          }
          else
          {
            // otherwise if it is below then my hand is moving downwards
            if (average < oldAverage && oldAverage <=20)
            {
              Serial.println("Sound down");
              keyboard.write(0xE0); //make volume -
              keyboard.write(0x21); //...
              keyboard.write(0xE0); //break
              keyboard.write(0xF0); //...
              keyboard.write(0x21); //...
              delay(150);
            }
          }
          // otherwise my hand must be stationary so check where it is.
        }
        else
        {
          // if my hand is stationary between 10 and 15 cms away from the sensor
          if (average >= 10 && average <=15)
          {
            Serial.println("Copy");
            keyboard.write(0x14); //make CNTRL
            keyboard.write(0x21); //make 'c'

            keyboard.write(0xF0); //break 'c' break
            keyboard.write(0x21); //...
            keyboard.write(0xF0); //break FO capital o) CNTRL break
            keyboard.write(0x14); //...
            delay(250);
          }
          else
          {
            // if my hand is a bit higher up...
            if (average >= 16 && average <=20)
            {
              Serial.println("Paste");
              keyboard.write(0x14); //make CNTRL
              keyboard.write(0x2A); //make 'v'

              keyboard.write(0xF0); //break 'v' break
              keyboard.write(0x2A); //...
              keyboard.write(0xF0); //break FO capital o) CNTRL break
              keyboard.write(0x14); //...
              delay(250);
            }
          }
        }
      }
    }
  }
  // clear our index and total for the next reading just in case
  if (index >= numReadings)
  {
    index = 0;
    total = 0;
  }
}
