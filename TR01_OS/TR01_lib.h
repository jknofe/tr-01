// define the engine types
typedef enum {
	RT_13B_RENESIS_04_11,
	RT_13B_REW_93_95,
	RT_13B_TURBO_89_92,
	RT_13B_NA_89_92,
	RT_13B_TURBO_86_88,
	RT_13B_NA_86_88,
	RT_13B_84_85,
	RT_13B_74_78,
	RT_12A_83_85,
	RT_12A_76_82
} EngineType;

// prototypes for test functions
#if TESTING == 1
void TestAdcToPres();
void TestRECorFact();
void TestNormTo250();
#endif

// protpypes
int AdcToPres(int,bool);
int RECorFact(int, EngineType);
int NormTo250(int, int, EngineType);

#if TESTING == 1
void TestAdcToPres(){
	int adcval[10] = { 0, 103, 200, 300, 400, 400, 600, 700, 922, 1023 };
	long tmp;
	Serial.println("Testing: AdcToPres()");
	Serial.println("adc\tbar\tbar\tpsi\tpsi");
	Serial.println("val\tfixP\tfloat\tfixP\tfloat");
	for(int i=0;i<10;i++){
		Serial.print(String(adcval[i]) + "\t");
		tmp = AdcToPres(adcval[i],true);
		Serial.print(String(tmp) +  "\t");
		Serial.print(float(tmp)/1000.0);
		Serial.print("\t");
		tmp = AdcToPres(adcval[i],false);
		Serial.print(String(tmp) +  "\t");
		Serial.println(float(tmp)/10.0);
	}
	Serial.println();
	Serial.println();
	delay(1000);
}
void TestRECorFact(){
	long tmp;
	Serial.println("Testing: RECorFact()");
	Serial.print("pres@");
	Serial.print("\t");
	for(int et=0;et<=9;et++){
	  Serial.print(String(et) + "\t");
	}
	Serial.println();
	for(int pres=5000;pres<=8000;pres+=500){
		Serial.print(String(pres) + "\t");
		for(int et=0;et<=9;et++){
			tmp = RECorFact(pres, (EngineType)et);
			Serial.print(String(tmp) + "\t");
		}
		Serial.println();
	}
	Serial.println();
	Serial.println();
	delay(1000);
}
void TestNormTo250(){
	long tmp;
	Serial.println("Testing: NormTo250()");
	Serial.print("pres@");
	Serial.print("\t");
	for(int rpm=200;rpm<=300;rpm+=10){
	  Serial.print(String(rpm) + "\t");
	}
	Serial.println();
	for(int pres=5000;pres<=8000;pres+=500){
	  Serial.print(String(pres) + "\t");
	  for(int rpm=200;rpm<=300;rpm+=10){
		tmp = NormTo250(pres, rpm, ENGINE_TYPE);
		Serial.print(String(tmp) + "\t");
	  }
	  Serial.println();
	}
	Serial.println();
	Serial.println();
	delay(1000);
}
#endif

// convert ADC value (0-1023) to physical pressure for 200psi sensor
// 0.5V - 4.5V -> 0bar to 13.79bar
// 0.5V - 4.5V -> 0psi to 200psi
int AdcToPres(int x, bool show_bar)
{
	// limit adc values, to sensor defined range 0.5 - 4.5V
	if( (x < 102) || (x > 922) ){
		return 0;
	}
	long a1;
	long a0;
	long fixp;
	long res;
	if(show_bar) {
		// linear fit {{102.4,0},{921.6,13.79}}
		a1 = 168335;
		a0 = -17237500;
		fixp = 10000;
	}
	else
	{
		// linear fit {{102.4,0},{921.6,200}}
		a1 = 2441410;
		a0 = -250000000;
		fixp = 1000000;
	}
	res = a1*(long)x + a0;
	// fixed point correction
	res /= fixp; 
	return (int)res;
}


// apply rotary engine corection factor
// derived from http://foxed.ca/index.php?page=rotarycalc
int RECorFact(int pres, EngineType myengine){
	long res;
	switch(myengine){
		case RT_13B_RENESIS_04_11:
			res = (long)pres * 1039;
			break;
		case RT_13B_REW_93_95:
		case RT_13B_TURBO_89_92:
			res = (long)pres * 1034;
			break;
		case RT_13B_NA_89_92:
			res = (long)pres * 1038;
			break;
		case RT_13B_TURBO_86_88:
			res = (long)pres * 1033;
			break;
		case RT_13B_NA_86_88:
		case RT_13B_84_85:
			res = (long)pres * 1036;
			break;
		case RT_13B_74_78:
			res = (long)pres * 1035;
			break;
		case RT_12A_83_85:
		case RT_12A_76_82:
			res = (long)pres * 1042;
			break;
		default:
			res = (long)pres * 1000;
			break;
	}
	// fixed point correction
	res /= 1000; 
	return (int)res;
}

// normalize the pressure reuslts to 250rpm
int NormTo250(int pres, int rpm, EngineType myengine)
{	
	long a2; 
	long a1;
	long a0;
	long b0;
	long res;

	switch(myengine){
		case RT_13B_RENESIS_04_11:
			// rpm correction from chart in mazda service manual 4EngineDetails.pdf page 11
			// quadratic fit {{200, 740}, {250, 850}, {300, 940}}
			// tranforrmed to factor
			// plot (850 / (-0.004x^2 + 4x + 100)), x=200...300, y=0.8...1.2
			// used fixed point calculation
			a2 = (-4)*(long)rpm*(long)rpm; 
			a1 = 4000 * (long)rpm;
			a0 = 100000;
			b0 = 850000000;
			break;
		case RT_13B_REW_93_95:
		case RT_13B_TURBO_89_92:
		case RT_13B_NA_89_92:
		case RT_13B_TURBO_86_88:
		case RT_13B_NA_86_88:
		case RT_13B_84_85:
		case RT_13B_74_78:
		case RT_12A_83_85:
		case RT_12A_76_82:
		default:
			// no  normalization
			a2 = 1; 
			a1 = 1;
			a0 = 1;
			b0 = 3000;
			break;
	}
	// calc result
	res = (long)pres * (b0 / (a2 + a1 + a0));
	// fixed point correction
	res /= 1000; 
	return (int)res;
}


// altitude correction
// still dummy
int NormToSeeLevel(int pres)
{
	long a2; 
	long a1;
	long a0;
	long b0;
	long res;

	// t.b.d. dummy func
	a2 = 1; 
	a1 = 1;
	a0 = 1;
	b0 = 3000;

	// calc result
	res = (long)pres * (b0 / (a2 + a1 + a0));
	// fixed point correction
	res /= 1000; 
	return (int)res;
}
