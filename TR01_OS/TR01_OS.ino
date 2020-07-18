/*TR-01 Rotary Compression Tester
  Originally distributed by John Doss 2017
  Some math parts were originally distributed by to Rotary Compression Calculator http://foxed.ca/index.php?page=rotarycalc

  Rewrite by jknofe using corrected math, fixed pioint operation,
  included rmp nomalization and sensors volume correction.

  rpm correction is based on 13B RX8 and 12B RX7 workshop manuals

  Hardware: Sparkfun Arduino Pro Micro 5V 16MHz

  addional option to meassure presssures like oil continuously.
  
  This code assumes you're using a 0-200 psi - 0.5-4.5 vdc pressure transducer connected to the AI0 pin.
  You can use sensors with other pressure or voltage ranges but you'll need to modify the code.
*/
#define TESTING 0
#define OILPRES 0

#define TMR1_FRQ 30 // Hz (valid range 1-30Hz)
#define TMR1_VAL 16000000 / (TMR1_FRQ * 1024) -1

#define SENSOR 0          //Analog input pin that sensor is connected too
#define ENGINE_TYPE RT_13B_RENESIS_04_11

#define NUM_REV 5         // number of revolutions to be captured
#define PEAK_DET 5       // adc value for peak detection
#define SHOWBAR 1         // swhow bar or psi values from pressure

#include "TR01_lib.h"

int face[3];              //3 faces per rotor
int results[NUM_REV][4] = {0};
int countRev = 0;         // count captured revoltions

int v;                    //voltage from pressure sensor
int i, j;                 //counter vars
int minimum;              //psi to drop before new face
int firstrun = 0;         //first time through test

unsigned long time;       //time for calc RPM
unsigned long OldTime;    //OldTime for calc RPM
unsigned long RPM;        //RPM variable

float final_results[3] = {0.0};

void setup() {
  Serial.begin(19200);      //serial speed
  delay(5000);               //hang out for 5 seconds to give use time to open shell

  Serial.println("TR-01 Rotary Compression Tester");
  Serial.println("-------------------------------");

#if OILPRES == 1
  cli();//stop interrupts
  //set timer1 interrupt
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register
  OCR1A = TMR1_VAL; //7800;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  sei();//allow interrupts
#endif
}

// simple bar graph, with number per 10 increments
void PrintBarGraph( int val, int scaler) {
  val /= scaler;
  for (int i = 0; i < val; i++) {
    Serial.print("#");
  }
  return;
}

#if OILPRES == 1
ISR(TIMER1_COMPA_vect) {
  int p = AdcToPres(analogRead(SENSOR), SHOWBAR);
  // print results
  Serial.print(millis() - OldTime);
  Serial.print("ms \t");
  PrintPres(p, SHOWBAR);
  Serial.print("\t");
  // print simple bar graph
  PrintBarGraph(p, 100);
  Serial.println("");
  OldTime = millis();
}
#endif

// main loop
void loop() {
#if TESTING == 1
  TestAdcToPres();
  TestRECorFact();
  TestNormTo250();
  return;
#endif


#if OILPRES == 1
  // nothing to do in main loop
  return;
#endif

  OldTime = millis();         //record cycle begining time for RPM calculation
  face[0] = face[1] = face[2] = 0; //zero out the faces
  for (i = 0; i < 3; i++)     //the following code reads the sensor (in psi), looks for a peak pulse, assigns that to one of the faces,
    //repeats the process for the next 2 faces and then moves on to the rest of the code
  {
    v =  analogRead(SENSOR);
    if (v < 0) {
      v = 0;
    }
    while ((face[i] - v) <= PEAK_DET)
    {
      if (face[i] < v) face[i] = v;
      v = analogRead(SENSOR);
      if (v < 0) {
        v = 0;
      }
    }
    minimum = v;
    while ((v - minimum) < PEAK_DET)
    {
      v =  analogRead(SENSOR);
      if (v < 0) {
        v = 0;
      }
      if (minimum > v) minimum = v;
    }
  }
  time = millis();            //record cycle ending time for RPM calculation
  RPM = (180000 / (time - OldTime)); //calculate RPM

  results[countRev][0] = RPM;
  results[countRev][1] = face[0];
  results[countRev][2] = face[1];
  results[countRev][3] = face[2];
  countRev += 1;

  // print results captured above
  if (countRev == NUM_REV) {
    Serial.println("----------------------------");
    Serial.println("# RAW RESULTS:");
    Serial.println("RPM\tR-1\tR-2\tR-3");
    for (i = 0; i < NUM_REV; i++) {
      Serial.print(results[i][0], DEC);
      Serial.print("\t");
      for (j = 1; j < 4; j++) {
        // print result
        Serial.print(AdcToPres(results[i][j], SHOWBAR), 1);
        Serial.print("\t");
      }
      Serial.println();
    }
    Serial.println("----------------------------");
    Serial.println("# @250 corrected RESULTS:");
    Serial.println("RPM\tR-1\tR-2\tR-3");
    for (i = 0; i < NUM_REV; i++) {
      Serial.print(250, DEC);
      Serial.print("\t");
      for (j = 1; j < 4; j++) {
        // print result
        // raw pressure
        int tmp = AdcToPres(results[i][j], SHOWBAR);
        // apply engine correction depending on engie type
        tmp = RECorFact(tmp, ENGINE_TYPE);
        // normalize to 250rpm
        tmp = NormTo250(tmp, results[i][0], ENGINE_TYPE);
        // look for max value over all results
        if (face[j - 1] < tmp) face[j - 1] = tmp;
        Serial.print(String(tmp));
        Serial.print("\t");
      }
      Serial.println();
    }
    Serial.println("----------------------------");
    Serial.println("Max values:");
    Serial.print("@250:\t");
    PrintPres(face[0], SHOWBAR);
    Serial.print("\t");
    PrintPres(face[1], SHOWBAR);
    Serial.print("\t");
    PrintPres(face[2], SHOWBAR);
    Serial.print("\t");
    Serial.println();
    Serial.println();
    countRev = 0;

  }
}
