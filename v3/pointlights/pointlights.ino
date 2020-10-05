//Version 3 - Adding behaviour


//For neopixels...
#include <Adafruit_NeoPixel.h>
const byte neoPin = 11;       //Global GPIO pin
const byte neoPixels = 24;    //Global constant for number of pixels
byte neoBright = 100;         //Variable for Neopixel brightness

// Create new Neopixel ring object
Adafruit_NeoPixel ring = Adafruit_NeoPixel(neoPixels, neoPin, NEO_GRB);


enum ledStates {START, INCREASE, DECREASE, STAY, WAVE, OFF, ON, INCREASEAGAIN, HALFSINE, OFF2, FADEOUT}; // Here we make nicknames for the different states our program supports.
enum ledStates ledState; // We define 'ledState' as type ledStates'
enum ledStates previousLedState = ledState;

unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;

//Global variables
int brightness = 0; // our main variable for setting the brightness of the LED
float velocity = 1.0; // the speed at which we change the brightness.
int ledPin = 9; // we use pin 9 for PWM

//Variables for controlling patterns
int maxLength;
int chargeState;    //255 is 100 percent
int holdChargeState;    //255 is 100 percent
int pauseTime;    //255 is 100 percent
int storeState;
int fading = 0; 
//State
int globalState = 0;
int painCounter = 0; 
int painSwitch = 0; 

//Am I alive?
int alive = 1;

int hlrCount = 0;
int hlrFreqCheck = 0;
int lifeCounter = 0;
int requiredHlrCount = 10;
int sensitivity = 0;

int lives = 2;
int maxPain = 20;

void setup() {
  // put your setup code here, to run once:
  pinMode(ledPin, OUTPUT); // set ledPin as an output.
  Serial.begin(9600); // initiate the Serial monitor so we can use the Serial Plotter to graph our patterns

  //Sensor
  pinMode(A0, INPUT);

  //Start over
  pinMode(10, INPUT_PULLUP);

  //Setup for ring
  ring.begin();
  ring.setBrightness(neoBright); 
  ring.show();

}

void loop() {
  int readPiezo = analogRead(A0);
  readPiezo = map(readPiezo, 0, 1023, 0, 20);
  readPiezo = map(readPiezo, 0, 20, 0, 255);
  //Serial.println("Piezo = ");
  Serial.println(readPiezo);
  //Serial.println(" %");
  //delay(50);
  
  
  if (alive == 0) {
    if (hlrCount >= requiredHlrCount && lifeCounter < lives) {
      Serial.println("Alive!!!!");
      alive = 1;
      lifeCounter++;
      requiredHlrCount = requiredHlrCount + 5;

      //reset those badboys
      hlrCount = 0;
      hlrFreqCheck = 0;

      //painswitch?
      
    } else {
      die();
    
      int startPin = digitalRead(10);
      
      if (startPin == 0 && hlrFreqCheck == 0) {
        Serial.println("Wake up!");
        hlrCount++;
        hlrFreqCheck = 1;

        
      } else if (startPin == 1) {
        hlrFreqCheck = 0;
      } 
      delay(10);
    }
       
  } else {
    //Read sensor value
    int sensorValue = analogRead(A0);
    sensorValue = sensorValue * 10;
    sensorValue = map(sensorValue, 0, 9500, 0, 100); //is this correct? Not 1023?
    
    if (sensorValue > 100) {    //Set max value
      sensorValue = 100;
    }   
    sensorValue == readPiezo;
    //Serial.print("Sensor : ");
    //Serial.println(sensorValue);
    
    //brightness = map(sensorValue, 0, 100, 0, 255);
    
    //Map sensor value to conditions and state  
    if (sensorValue > 80) {     //100
      globalState = 1;
      //Serial.println("above 80");
  
    } else if (sensorValue > 50 - sensitivity) {    //75
      globalState = 2;
      //Serial.println("above 50");
  
    } else if (sensorValue > 5 - sensitivity) {            //0
      globalState = 3;
      //Serial.println("else");
    } else {
      globalState = 4;
    }

    //States
    if (globalState == 1) {        // 100
     //Define variables
       if (painCounter > maxPain) {
        die(); 
       } else {
        increaseDecrease();      //Composing function
       }
      delay(10);
      setLed();
      
      //analogWrite(ledPin, brightness);
      currentMillis = millis(); //store the current time since the program started
  
     } else if (globalState == 2) {        // 75
      //Define variables
      maxLength = 100;
      chargeState = 130;    //255 is 100 percent
      holdChargeState = 1200;    //255 is 100 percent
      pauseTime = 1000;    //255 is 100 percent
      
      changeState(WAVE);
      waveThem();    //Composing function
  
      delay(10);
      setLed();
      
      //analogWrite(ledPin, brightness);
      currentMillis = millis(); //store the current time since the program started
      
    } else if (globalState == 3) {        // 50
      waveSlow();
      delay(10);

      setLed();
      //analogWrite(ledPin, brightness);
      currentMillis = millis(); //store the current time since the program started
  
    } else if (globalState == 4) {
    
      if (painCounter % 2 == true && painSwitch == 0) {
        painSwitch = 1;
      } else {
        painCounter = painCounter * 0.99999997;
        painSwitch = 0;
      }
      
      Serial.print("The current pain is: ");
      Serial.print(painCounter);
      Serial.println(" painThymes");
      constantWave();
      delay(10);
      
      setLed();
      //Write(ledPin, brightness);
      currentMillis = millis(); //store the current time since the program started
  
    }
  }
  
}

void compose() {
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
    
    setLed();
    //analogWrite(ledPin, brightness);
    
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
      //globalState++;
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

void waveThem() {
  plot("WAVE", brightness);
  brightness = brightness;
  brightness = sinewave(1000,180,50); // you can tweak the parameters of the sinewave
  setLed();
  //analogWrite(ledPin, brightness);
  
  if (currentMillis - startMillis >= 5000){ //change state after 5 secs by comparing the time elapsed since we last change state
    changeState(START);
    }
}
void waveSlow() {
  plot("WAVE", brightness);
  brightness = brightness;
  brightness = sinewave(2000,100, 25); // you can tweak the parameters of the sinewave
  
  setLed();
  //analogWrite(ledPin, brightness);

  if (currentMillis - startMillis >= 5000){ //change state after 5 secs by comparing the time elapsed since we last change state
    changeState(START);
    }
}

void constantWave() {
  plot("WAVE", brightness);
  brightness = brightness;
  brightness = sinewave(3000,50,0); // you can tweak the parameters of the sinewave
  
  if (currentMillis - startMillis >= 5000){ //change state after 5 secs by comparing the time elapsed since we last change state
    changeState(START);
    }
}
void increaseDecrease() {
  if (ledState == 0 || ledState == 4) {
    changeState(INCREASE);
  }
  switch (ledState){

  case INCREASE:
    brightness = increase_brightness(brightness, 20 + (painCounter * painCounter/2));
    //brightness = expInc_brightness(brightness, 20);
    
    plot("INCREASING", brightness);
        
    if (brightness >= 250){
      //ledState = WAVE;
      changeState(DECREASE);
      }
   break;
   
  case DECREASE:
    brightness = decrease_brightness(brightness, 40 + (painCounter * painCounter/2));
    //brightness = expDec_brightness(brightness, maxLength/10);
    
    plot("DECREASING", brightness);
      if (brightness <= 0){
      changeState(INCREASE);
      painCounter++;
      Serial.print("Pain for: ");
      Serial.print(painCounter);
      Serial.println(" painThymes");
      }
     break;
  }
}

//Helper functions
void die() {
  alive = 0;
     
  if (brightness <= 0){
    //Callback for died
    //Serial.print("died ... ");
  } else {
    brightness = expDec_brightness(brightness, 2);
  
    plot("DECREASING", brightness);
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


void setLed() {
  for(int i = 0; i < neoPixels; i++){
    int ledValue = brightness;
    ring.setPixelColor(i, ring.Color(ledValue, ledValue, ledValue));
    ring.show();
  }
}
