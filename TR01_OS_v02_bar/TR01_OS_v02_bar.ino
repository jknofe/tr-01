/*TR-01 Rotary Compression Tester
Originally distributed by John Doss 2017, modified for output in bar and normalized values for 250rpm by Miro Bogner 2018

This code assumes you're using a 0-200 psi - 0.5-4.5 vdc pressure transducer connected to the AI0 pin.
You can use sensors with other pressure or voltage ranges but you'll need to modify the code.
*/

#define SENSOR 0                                                                    //Analog input pin that sensor is connected too

int face[3];                                                                        //3 faces per rotor
int v;                                                                              //voltage from pressure sensor converted to analog input
int i;                                                                              //counting faces
int minimum;                                                                        //sensor input to drop before new face
int firstrun=0;                                                                     //first time through test

unsigned long time;                                                                 //time for calc RPM
unsigned long StartTime;                                                            //StartTime for calc RPM
unsigned long RPM;                                                                  //RPM variable

float dead_space=1.039;                                                             //correction factor for dead space in 13B MSP Renesis engine 
float bar_factor=0.1683290354778;		                                                //factor to calculate bar (x10) from analog sensor values
float a0=8.5944582043344;                                                           //polynom Fit a0 to norm values to 250rpm
float a1=-4.8028702270382E-02;                                                      //polynom Fit a1 to norm values to 250rpm
float a2=5.4250515995872E-05;                                                       //polynom Fit a2 to norm values to 250rpm
float correction;                                                                   //correction value to norm values to 250rpm

char buf1[5],buf2[20],buf3[5],buf4[10],buf5[45];                                    //setting up arrays for compression and rpm data

void setup() {
  Serial.begin(19200);                                                              //serial speed
  delay(500);                                                                       //hang out for half a second
  
  Serial.println("      TR-01 Open-Source");
  Serial.println("Rotary Engine Compression Tester");
  Serial.println("          Firmware v0.2");
  Serial.println("");
}

void loop(){
    if (firstrun==0){                                                               //prints below text on bootup
      Serial.println("BAR: 0.0 0.0 0.0   RPM: 000");
      firstrun=1;
    }
    
  StartTime=millis();                                                               //record cycle begining time for RPM calculation           
  face[0]=face[1]=face[2]=0;                                                        //zero out the faces
  for (i=0; i<3; i++)                                                               //the following code reads the sensor (in raw sensor values from 0-1023), looks for a peak pulse, assigns that to one of the faces,
  {                                                                                 //repeats the process for the next 2 faces and then moves on to the rest of the code
    v = (analogRead(SENSOR));
    while ((face[i]-v)<=20)
    {
      if(v>face[i]) face[i]=v;
      v = (analogRead(SENSOR));
    }   
    minimum=v;
    while((v-minimum)<20)
    {
      v=(analogRead(SENSOR));
      if (minimum>v) minimum=v;
    }
  }          
  time=millis();                                                                    //record cycle ending time for RPM calculation
  RPM=(180000/(time-StartTime));                                                    //calculate RPM
  if (RPM<=0 || RPM>999) RPM=0;                                                     //Set RPM 0 if values are out of range

  if (face[0]<=102.4) face[0]=102.4;                                                //Make sure there are no negative faces
  if (face[1]<=102.4) face[1]=102.4;
  if (face[2]<=102.4) face[2]=102.4;
 
  strcpy(buf2,"BAR: ");                                                             //Prep the "BAR results" array
  
  dtostrf(round((face[0]-102.4)*bar_factor*dead_space)/10,3,1,buf1);                //convert first face peak compression pulse to bar, string and dump it into an array
  strcat(buf2,buf1);                                                                //take the contents of that array and dump it into the "BAR results" array
  strcat(buf2," ");                                                                 //dump a space into the "test results" array
  
  dtostrf(round((face[1]-102.4)*bar_factor*dead_space)/10,3,1,buf1);
  strcat(buf2,buf1);
  strcat(buf2," ");
  
  dtostrf(round((face[2]-102.4)*bar_factor*dead_space)/10,3,1,buf1);
  strcat(buf2,buf1);
  
  strcpy(buf4,"RPM: ");                                                             //Prep the "RPM results" array
  
  itoa(RPM,buf3,10);                                                                //convert the calculated RPM from int to string and dump it into an array
  if (strlen(buf3)<3) strcat(buf4,"0");                                             //if value is only 1 digit then dump a 0 into the array
  if (strlen(buf3)<2) strcat(buf4,"0");                                             //if value is only 2 digits then dump a 0 into the array
  strcat(buf4,buf3);                                                                //take the contents of that array and dump it into the "RPM results" array

  
  if (RPM<2 || RPM>350)                                                           //Check if RPM is in a valid range for normalization
  {
    strcpy(buf5,"RPM out of range for normalization");                              //Prep the "normalized BAR results" array if out of spec
  }
  else
  {
    strcpy(buf5,"BAR normalized for 250rpm: ");                                     //Prep the "normalized BAR results" array if in spec

    correction=a0+a1*RPM+a2*sq(RPM);                                                //Calculate correction value to norm compression values to 250rpm

    dtostrf(round((face[0]-102.4)*bar_factor*dead_space)/10+correction,3,1,buf1);   //convert first face peak compression pulse to string and dump it into an array
    strcat(buf5,buf1);                                                              //take the contents of that array and dump it into the "BAR results" array
    strcat(buf5," ");                                                               //dump a space into the "test results" array
  
    dtostrf(round((face[1]-102.4)*bar_factor*dead_space)/10+correction,3,1,buf1);
    strcat(buf5,buf1);
    strcat(buf5," ");
  
    dtostrf(round((face[2]-102.4)*bar_factor*dead_space)/10+correction,3,1,buf1);
    strcat(buf5,buf1);
  }
    
  Serial.print(buf2);                                                               //print the "BAR results" array to the serial port
  Serial.print("   ");                                                              //print a space to the serial port
  Serial.print(buf4);                                                               //print the "RPM resutls" array to the serial port
  Serial.print("   ");                                                              //print a space to the serial port
  Serial.print(buf5);                                                               //print the "normalized BAR results" array to the serial port
  Serial.println();                                                                 //sends the "newline" command to the serial port
}
