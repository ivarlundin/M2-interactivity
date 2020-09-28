//Chargestate 01 - Sketch #6
#include <math.h>

enum ledStates {START, INCREASE, DECREASE, STAY, WAVE, OFF, ON, INCREASEAGAIN, HALFSINE, OFF2, FADEOUT}; // Here we make nicknames for the different states our program supports.
enum ledStates ledState; // We define 'ledState' as type ledStates'
enum ledStates previousLedState = ledState;

unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;

int brightness = 0; // our main variable for setting the brightness of the LED
float velocity = 1.0; // the speed at which we change the brightness.
int ledPin = 9; // we use pin 9 for PWM
int sineCounter = 0;

//CONTROL PATTERN
int maxLength;
int chargeState;    //255 is 100 percent
int holdChargeState;    //255 is 100 percent
int pauseTime;    //255 is 100 percent

int globalState = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(ledPin, OUTPUT); // set ledPin as an output.
  Serial.begin(9600); // initiate the Serial monitor so we can use the Serial Plotter to graph our patterns

pinMode(A0, INPUT);
}

//Debug loop

void loop() {
  
  int sensorValue = analogRead(A0);
  sensorValue = sensorValue/1023*100;
  sensorValue = round(sensorValue);

  Serial.print("Sensor value: ");
  Serial.println(sensorValue);
  
/*
  if (sensorValue > 1023) {
    globalState == 1;
}

  // put your main code here, to run repeatedly:
   if (globalState == 0) {        // 75
    //Serial.println("50 percent ////////////////////////////// 50 percent ////////");
    maxLength = 50;
    chargeState = 0;    //255 is 100 percent
    holdChargeState = 0;    //255 is 100 percent
    pauseTime = 0;    //255 is 100 percent
    
    compose();
    delay(10);
    analogWrite(ledPin, brightness);
    currentMillis = millis(); //store the current time since the program started

   } else if (globalState == 1) {        // 75
    //Serial.println("50 percent ////////////////////////////// 50 percent ////////");
    maxLength = 100;
    chargeState = 255;    //255 is 100 percent
    holdChargeState = 1000;    //255 is 100 percent
    pauseTime = 1000;    //255 is 100 percent
    
    compose();
    delay(10);
    analogWrite(ledPin, brightness);
    currentMillis = millis(); //store the current time since the program started







   } else if (globalState == 2) {        // 75
    //Serial.println("50 percent ////////////////////////////// 50 percent ////////");
    maxLength = 100;
    chargeState = 130;    //255 is 100 percent
    holdChargeState = 1200;    //255 is 100 percent
    pauseTime = 1000;    //255 is 100 percent
    
    compose();
    delay(10);
    analogWrite(ledPin, brightness);
    currentMillis = millis(); //store the current time since the program started
    
  } else if (globalState == 3) {        // 50
    //Serial.println("50 percent ////////////////////////////// 50 percent ////////");
    maxLength = 100;
    chargeState = 60;    //255 is 100 percent
    holdChargeState = 800;    //255 is 100 percent
    pauseTime = 1000;    //255 is 100 percent
    
    compose();
    delay(10);
    analogWrite(ledPin, brightness);
    currentMillis = millis(); //store the current time since the program started
    
  } else if (globalState == 4) {  //25
    //Serial.println("25 percent ////////////////////////////// 25 percent ////////");
    maxLength = 100;
    chargeState = 15;    //255 is 100 percent
    holdChargeState = 400;    //255 is 100 percent
    pauseTime = 1000;    //255 is 100 percent
    
    compose();
    delay(10);
    analogWrite(ledPin, brightness);
    currentMillis = millis(); //store the current time since the program started
    
  } else if (globalState == 5) {            // 75
    globalState = 0;
    //Serial.println("Start over ////////////////////////////// Start over ////////");
    
  }
  //compose();
  //delay(10);
  //analogWrite(ledPin, brightness);
  //currentMillis = millis(); //store the current time since the program started
  */
}

void compose() {
  // this is a state machine which allows us to decouple the various operations from timed loops. 
  // instead we just switch from state to state when particular conditions are met.
  // we switch states by calling the changeState() function.
  
  switch (ledState){

  case START:
    plot("START", brightness);
    brightness = 255;
    if (currentMillis - startMillis >= maxLength){
      changeState(DECREASE);
      }
    break;
  
  case INCREASE:
    //brightness = increase_brightness(brightness, 18);
    brightness = expInc_brightness(brightness, 20);    
    
    plot("INCREASING", brightness);
        
    if (brightness > 250){
      //ledState = WAVE;
      changeState(OFF);
      }
   break;
   
  case DECREASE:
    //brightness = decrease_brightness(brightness, 20);
    brightness = expDec_brightness(brightness, maxLength/10);
    
    plot("DECREASING", brightness);
      if (brightness <= chargeState){
      changeState(OFF2);
      }
     break;

  case INCREASEAGAIN:
    brightness = increase_brightness(brightness, 20);

    plot("INCREASING", brightness);
        
    if (brightness >= 130){
      //ledState = WAVE;
      changeState(OFF);
    }
    
    break;
    
  case WAVE:
    plot("WAVE", brightness);
    brightness = 180;
    brightness = sinewave(1000,256,0); // you can tweak the parameters of the sinewave
    analogWrite(ledPin, brightness);
    
    if (currentMillis - startMillis >= 5000){ //change state after 5 secs by comparing the time elapsed since we last change state
      changeState(START);
      }
    break;
    
  case STAY:
    plot("STAY", brightness);
    brightness = brightness;
    break;

  case ON:
    plot("ON", brightness);
    brightness = 255;
    break;

  case OFF:
    plot("OFF", brightness);
    brightness = 0;
    if (currentMillis - startMillis >= pauseTime){
      changeState(START);
      // globalState++;
      }
    break;
    
  case OFF2:
  plot("OFF", brightness);
  brightness = chargeState;
  if (currentMillis - startMillis >= holdChargeState){
    changeState(FADEOUT);
    }
  break;

  case FADEOUT:
  //brightness = decrease_brightness(brightness, 20);
    brightness = expDec_brightness(brightness, maxLength/5);;
    
    plot("DECREASING", brightness);
      if (brightness <= 0){
      changeState(OFF);
      }
     break;
  }
}

void changeState(ledStates newState){
    // call to change state, will keep track of time since last state
    startMillis = millis();
    ledState = newState;
  }
  
void plot(char *state, int brightness){
    // use this function to plot a graph.
    // it will normalize the auto-scaling plotter 
    Serial.print(state);
    Serial.print(", ");
    Serial.print(brightness);
    Serial.println(", 0, 300");
  }

int increase_brightness (int brightness, float velocity){
    return brightness = brightness + 1 * velocity;
  }

int expInc_brightness (int brightness, float velocity){
  int output = brightness + brightness/velocity + 1;
  
  if (output >= 255) {
    return 255;
  } else {
    return output;
  }
}


int decrease_brightness (int brightness, float velocity){
  int output = brightness = brightness - 1 * velocity;
  
  if (brightness > 0) {
    return output;
  } else {
    return 0;
  }
}

int expDec_brightness (int brightness, float velocity){
  int output = brightness = brightness - brightness/velocity - 1;
  
  if (brightness > 0) {
    return output;
  } else {
    return 0;
  }
}

int sinewave(float duration, float amplitude, int offset){
    // Generate a sine oscillation, return a number.
    // In case you are using this for analogWrite, make sure the amplitude does not exceed 256
    float period = millis()/duration; // Duration in ms determines the wavelength.
    float midpoint = amplitude / 2; // set the midpoint of the wave at half the amplitude so there are no negative numbers
    int value = midpoint + midpoint * sin ( period * 2.0 * PI );
    value = value + offset; //offset allows you to move the wave up and down on the Y-axis. Should not exceed the value of amplitude to prevent clipping.
    return value;
  }

int halSine(float duration, float amplitude, int offset){
    // Generate a sine oscillation, return a number.
    // In case you are using this for analogWrite, make sure the amplitude does not exceed 256
    float period = millis()/duration; // Duration in ms determines the wavelength.
    float midpoint = amplitude / 2; // set the midpoint of the wave at half the amplitude so there are no negative numbers
    int value = midpoint + midpoint * sin ( period * 2.0 * PI );
    value = value + offset; //offset allows you to move the wave up and down on the Y-axis. Should not exceed the value of amplitude to prevent clipping.
    return value;
  }
