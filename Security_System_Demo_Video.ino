/*
 * Vincenzo L
 * Dec 12,2018 - Dec 17,2018
 * Arduino 4x4 Keypad Security System 
 * Comp. Tech Final Summative
 * TEJ3M1
 * /////////////////////////////////////////////////////////////////////////////////////////////////
 * This is an arduino security system i designed using a few basic parts like 2 LED's and resistors
 * servo, buzzer and a 4x4 matrix keypad i got from Amazon or Sayal Electronics
 * This can be used in a wide range of devices like home security systems, safes and much more just by adding
 * a few more sensors like PIR motion, sound, magnetic door/window sensors and more
 */

//importing the lib's
#include<Servo.h>
#include<Keypad.h>
#include<Password.h>

Servo lock;//creates the servo
Password pass = Password("159357");
Password admin = Password("C0D3AD");

//-----Keypad Setup----//
byte rowPins [] = {9,8,7,6}; //row pins 1,2,3,4
byte colPins [] = {5,4,3,2}; //col pins 1,2,4,4

const byte ROWS = 4;//4 rows on my keypad
const byte COLS = 4; //4 cols on keypad

//create the characters of the keypad
char keys[ROWS][COLS]={
  {'1','2','3','A'},//row1
  {'4','5','6','B'},//row2
  {'7','8','9','C'},//row3
  {'*','0','#','D'}//row4
};

Keypad keypad = Keypad(makeKeymap(keys),rowPins,colPins,ROWS,COLS);//creates keypad and makes a map of the characters using the keys array
//----------------------------------//

//other pins
int redLED = A1;//red incorrect LED
int greenLED = A0;//green led for correct
int servoPin = 10;// Servo 
int speaker = 11;//Speaker pin

//Other variables for storing data
int tries=3;//used for amount of login attemps
int timesUnlocked,incorrect = 0;//used to store amount of times system was logged into or pass incorrect

void setup() {
  Serial.begin(9600);//Starts the monitor
  //pin types
  pinMode(redLED,OUTPUT);
  pinMode(greenLED,OUTPUT);
  pinMode(servoPin,OUTPUT);
  pinMode(speaker,OUTPUT);
  //setting servo
  lock.attach(servoPin);//attaches the servo to the servoPin on D10
  lock.write(10);//moves servo to locked pos.
  //Startup text
  Serial.println("Use * to enter the password, Use # to clear...");
  Serial.println("Enter Password: ");

  keypad.addEventListener(keypadEvent);//adds an event listener from the keypad lib to 'listen' for events like presses from keypad
}

void loop() {
  keypad.getKey();//look for keypad presses(then passes it into the functions in the background using the library)
  lock.write(10);//locked pos
}

void keypadEvent(KeypadEvent eKey){

  switch(keypad.getState()){//switch case for state of keypad buttons
    case PRESSED://if pressed
      Serial.print("Tries left: ");
      Serial.print(tries);//prints tries left
      Serial.print(" Enter: ");
      tone(speaker,900,50);//plays beep tone on speaker when key is pressed
      Serial.println(eKey);//prints key pressed
  }

  switch(eKey){//switch case for the key
    case '*'://enter password button
      checkPassword();//call our check password method/function
      delay(10);
      break;
      
    case '#'://reset pass key
      //reset all passwords to clear
      pass.reset();
      admin.reset();
      delay(10);
      break;
      
    default:
      //sends the keypress to the Password Lib
      pass.append(eKey);
      admin.append(eKey);
  }
}

void checkPassword(){//function to check entered passwords from user

  if(pass.evaluate()){// if pass 1 is true
    Serial.println("Password Correct! Unlocked for 10 seconds...");
    delay(5);
    lock.write(110);//moves servo to unlocked pos.
    //------Unlock tone melody---//
    tone(speaker,900,100);
    delay(150);
    tone(speaker,1100,100);
    delay(150);
    tone(speaker,1200,100);
    delay(150);
    //------------------------//
    analogWrite(greenLED,255);//turn green led on max
    delay(10000);//delays 10 seconds
    analogWrite(greenLED,0);//turn green led off
    lock.write(10);//moves to locked pos.
    tries=3;//reset tries
    timesUnlocked++;//adds one to times system was unlocked
    //clear/reset passwords
    pass.reset();
    admin.reset();
    
    Serial.println("Locked...");
    Serial.println("Enter Password: ");
  }
  
  else if(admin.evaluate()){// if pass 2 is true(admin)
    tries=3;//reset tries
    timesUnlocked++;//adds one to times system was unlocked
    Serial.print("Admin Account:");
    lock.write(110);//moves to unlocked pos
    
    //loading thing animation
    Serial.print("Checking: ");
    for(int c=0;c<8;c++){
      Serial.print("*");
      delay(500);
    }
    //-------------//
    Serial.println("\n");//creates empty line7
    //prints times system was unlocked
    Serial.print("Times whole system was unlocked: ");
    Serial.print(timesUnlocked);
    delay(2000);//delay 2 sec
    //prints times passwords were not correct
    Serial.print("Times password was not correct: ");
    Serial.print(incorrect);
    delay(4000);//delay 4 sec
    lock.write(10);//moves to locked pos.
    //clear / reset passwords
    tries=3;//reset tries
    pass.reset();
    admin.reset();
    Serial.println("Locked...");
    Serial.println("Enter Password: ");
  }
  else{
    tries--;//take one away from total tries
    Serial.println("Password incorrect. Denied... Try Again ");
    delay(10);
    lock.write(10);//moves to locked pos.
    incorrect++;//add one to times incorrect
    for(int n=0;n<3;n++){
      tone(speaker,400,500);//play beep
      analogWrite(redLED,255);//turn on the red led
      delay(500);
      noTone(speaker);//stop all sounds
      analogWrite(redLED,0);//turn off the red led
      delay(500);
    }

    if(tries==0){
      Serial.println("Ran out of tries...Locked for 30 seconds.\n");
      //30 sec flashing loop
      for(int t=30;t>0;t--){
        Serial.print(t);
        analogWrite(redLED,255);//turn red led on
        delay(500);//delay half second
        analogWrite(redLED,0);//turn red led off
        delay(500);//delay half second
      }
      tries=3;//reset tries
      //play re-enabled tone
      tone(speaker,900,50);
      delay(50);
      tone(speaker,900,50);
      delay(50);
      
      Serial.println("\nEnter Password: ");
    }
    //reset passwords
    pass.reset();
    admin.reset();
  }
}



