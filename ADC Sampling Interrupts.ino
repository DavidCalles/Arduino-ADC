
/*Constants within the code*/
const int aPin2 = A1;
const int aPin  = A0;
const float vRef = 4.5;

/*Precompiler definitions*/
#define PRINT_TIMES 0

/*Setup function*/
void setup() {
  //Serial setup at 115200 bauds
  Serial.begin(115200);
  cli();//stop interrupts
  //set timer1 interrupt at 1KHz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1Khz increments
  OCR1A = 15;//(16*10^6) / (1000*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  sei();//allow interrupts
}

/*Global volatile variables (To be modified in interruption)*/
volatile unsigned long t2, t1 = micros();
volatile float fVal=0.0, fVal2=0.0;
volatile unsigned short aVal=0, aVal2=0;
volatile unsigned short aprox=0, aprox2=0;
volatile unsigned short prev_val=0;

/*Loop function (While(1)) */
void loop() {
}

//Timer1 Interrupt 1KHz routine for analog sampling
ISR(TIMER1_COMPA_vect){
  // Read pin values from adc (built-in function)
  aVal = analogRead(aPin);
  aVal2 = analogRead(aPin2);
  // Turn ADC values into voltage
  fVal = (float)aVal *vRef/1024;
  fVal2= (float)aVal2*vRef/1024;
  // Binary signal (comparator)
  aprox  = (fVal>3.0 ? 1:0);
  aprox2 = (fVal2>3.0 ? 1:0);
  // ¨Print values
  Serial.print(fVal);
  Serial.print("\t");
  Serial.print(-fVal2);

//Get and print times between edges of the signal
//Works fine for square and sinusoidal functions
  if (PRINT_TIMES){
    if (prev_val != aprox){
      t2 = micros();    
      prev_val = aprox;
      Serial.print("\t");
      Serial.print(t2-t1);
      t1 = t2;    
    }
    else{
      Serial.print("\t");
      Serial.print("0");
    }     
  }
  Serial.print("\n");
}
