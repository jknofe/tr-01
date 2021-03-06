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
void PrintPres(int, bool);

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
	for(int rpm=125;rpm<=375;rpm+=25){
	  Serial.print(String(rpm) + "\t");
	}
	Serial.println();
	for(int pres=5000;pres<=8000;pres+=500){
	  Serial.print(String(pres) + "\t");
	  for(int rpm=125;rpm<=375;rpm+=25){
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
// 0.5V - 4.5V -> 0bar to 13.79bar -> return fixpoint 1 = 1mBar
// 0.5V - 4.5V -> 0psi to 200psi   -> return fixpoint 1 = 10mPsi
int AdcToPres(int x, bool show_bar)
{
	// only accept adc values in sensor defined range 0.5 - 4.5V
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


// apply dead space correction factor
// derived from http://foxed.ca/index.php?page=rotarycalc
// RECorFact = (camber_displacement + sensor_volume) / camber_displacement
// initial assumption is that value from foxed are matching, but they need to be verified for own setup
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
	long a3,a2, a1, a0, b0, cor, res;
	// do only for valid rpms
	if( (rpm > 149) && (rpm < 351) )
	{
		switch(myengine){
			case RT_13B_RENESIS_04_11:
				// form newer RX8 workshop manual, A-B Chart
				// interpolation {{150, 565}, {200, 715}, {250, 830}, {300, 925}}
				// plot 0.00002 x^3 - 0.019 x^2 + 7.8 x - 245
				// (830*50000 / (x^3 - 950 x^2 + 390000 x - 12250000))
				a3 = (long)rpm*(long)rpm*(long)rpm; 
				a2 = (-950)*(long)rpm*(long)rpm; 
				a1 = 390000 * (long)rpm;
				a0 = -12250000;
				b0 = 830*50000;		// 830kpa = 8.3bar
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
				a3 = 1;
				a2 = 1; 
				a1 = 1;
				a0 = 1;
				b0 = 4000;
				break;
		}

		//cor = (b0 - (a3 + a2 + a1 + a0));
		cor = (a3 + a2 + a1 + a0);
		// fixed point correction
		cor /= 1000;
		cor = b0 / cor;
		// apply correction to pressure
		res = (long)pres * cor;
		// fixed point correction
		res /= 1000;
	}
	else
	{
		// return for rpm out of range
		res = 0;
	}
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

// covert fixed points to float for printing
void PrintPres(int pres, bool show_bar){
	if(show_bar){
		Serial.print((float)pres/1000.0,2);
	}
	else {
		Serial.print((float)pres/1000.0,1);
	}
}
