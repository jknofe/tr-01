/*TR-01 Rotary Compression Tester
Originally distributed by John Doss 2017

This code assumes you're using a 0-200 psi - 0.5-4.5 vdc pressure transducer connected to the AI0 pin.
You can use sensors with other pressure or voltage ranges but you'll need to modify the code.
*/

#define SENSOR 0          //Analog input pin that sensor is connected too
#define NUM_REV 5         // number of revolutions to be captured
#define SHOWBAR 1

int face[3];              //3 faces per rotor
int results[NUM_REV][4] = {0};
int countRev = 0;         // count captured revoltions

int v;                    //voltage from pressure sensor
int i, j;                 //counter vars
int minimum;              //psi to drop before new face
int firstrun=0;           //first time through test

unsigned long time;       //time for calc RPM
unsigned long OldTime;    //OldTime for calc RPM
unsigned long RPM;        //RPM variable

// http://www.wolframalpha.com/input/?i=linear+fit+%7B%7B102.4,+0%7D,+%7B921.6,+13.79%7D%7D
// http://www.wolframalpha.com/input/?i=linear+fit+%7B%7B102.4,+0%7D,+%7B921.6,+200%7D%7D
float AdcToPres(int x)
{
  #if SHOWBAR == 1
    return (0.0168335*x - 1.72375);
  #else
    return (0.2441410*x - 25);
  #endif
}

void setup() {
  Serial.begin(9600);      //serial speed
  delay(500);               //hang out for half a second
  
  Serial.println("      TR-01 Open-Source");
  Serial.println("Rotary Engine Compression Tester");
  Serial.println("          Firmware v0.1");
  Serial.println("");
}

void loop(){
     
  OldTime=millis();           //record cycle begining time for RPM calculation
  face[0]=face[1]=face[2]=0;  //zero out the faces
  for (i=0; i<3; i++)         //the following code reads the sensor (in psi), looks for a peak pulse, assigns that to one of the faces,
                              //repeats the process for the next 2 faces and then moves on to the rest of the code
  {
    v = analogRead(SENSOR);
    if (v < 0){v = 0;}
    while ((face[i]-v)<=5)
    {
      if(face[i]<v) face[i]=v;
      v = analogRead(SENSOR);
      if (v < 0){v = 0;}
    }   
    minimum=v;
    while((v-minimum)<5)
    {
      v = analogRead(SENSOR);
      if (v < 0){v = 0;}
      if (minimum>v) minimum=v;
    }
  }
  time=millis();              //record cycle ending time for RPM calculation
  RPM=(180000/(time-OldTime));//calculate RPM

  results[countRev][0] = RPM;
  results[countRev][1] = face[0];
  results[countRev][2] = face[1];
  results[countRev][3] = face[2];
  countRev += 1;

  // print results captured above
  if(countRev == NUM_REV) {
    Serial.println("# RESULTS:");
    Serial.println("RPM\tR-1\tR-2\tR-3");
    for (i=0; i<NUM_REV; i++){
      Serial.print(results[i][0], DEC);
      Serial.print("\t");
      for (j=1; j<4; j++){
        Serial.print(AdcToPres(results[i][j]), 1);
        Serial.print("\t");
      }
      Serial.println();         //sends the "newline" command to the serial port
    }
    countRev = 0;
  }
}
