/*
  Wireless Networking Fundamentals and Applications - Lab2
  Team 7
  VLC
  Yen Su,MH Yang,WC Lin,Joe Huang,Jack Kao
*/

int led = 5;
byte timecode[60];
int date, year, month, day, hour, minu, weekday ;

void setup()
{
	Serial.begin(9600);
	
	pinMode(led, OUTPUT);
	TCCR3A = _BV(COM3A0) | _BV(WGM30);  //0x0F => fast PWM TOP=OCRnA
	TCCR3B = _BV(WGM33)  |  _BV(CS30);  //CS30 0x01=> presaler = 1
	OCR3A = 100;   // set 100 for 40khz 
    //  OCR3A = 66     // set 66 for 60khz 
}

char buf[18];    
int idx = 0, looptime = 0;

//int year, month, day, hour, min, weekday
//  2015/04/10 22:34 7 
//  0    2  4  6  8 10     

int flag = 0 ;     // let user input the time only at first  
int smile = 0 ;    // print out ^_^ on monitor for waiting input 
void loop()
{
        int cc = 0;
        // print out ^_^ on monitor for waiting input 
        while (cc <= 0 && flag==0 ) {     
          cc = Serial.available();       
          if(smile == 20){
              smile = 0 ;
              Serial.println("");
          }
          
          Serial.print(" ^_^ ");        
          smile ++;
          delay(500);
        }  
        
        // let user input the time and store in buffer
        if(flag == 0){
            for (int i = 0; i < cc; i++) {                
                char incomingByte = Serial.read();   
                buf[idx] = incomingByte;
                idx++;
            }
            flag = 1 ;

            // compute the data which clock needs 
            memset(timecode, 8, sizeof(timecode));            
            year = 1000*(buf[0]-'0')+100*(buf[1]-'0')+10*(buf[2]-'0')+(buf[3]-'0');
            month =10*(buf[5]-'0')+(buf[6]-'0');
            day = 10*(buf[8]-'0')+(buf[9]-'0');
            hour = 10*(buf[11]-'0')+(buf[12]-'0');
            minu = 10*(buf[14]-'0')+(buf[15]-'0');
            weekday = buf[17]-'0'; 
        }
  	
        if (looptime!=0) minu++;   // minute+1 when enter again in the loop
	setupTimeCode();           // initalize the timecode[60]
	looptime++;

	Serial.print(year);
	Serial.print("/");
	Serial.print(month);
	Serial.print("/");
	Serial.print(day);
	Serial.print(" ");
	Serial.print(hour);
	Serial.print(":");
	Serial.print(minu);
	Serial.print(" ");
	Serial.print(weekday);
	Serial.print(" ");
	Serial.println(date);
        
        // modulate the wave 
	for (int i = 0; i < 60; i++) {
		Serial.print("i = ");
		Serial.println(i);
		Serial.print("timecode = ");
		Serial.println(timecode[i]);
		if (timecode[i] == 2) {
			genDuration(200);
		} else if (timecode[i] == 5) {
			genDuration(500);
		} else {
			genDuration(800);
		}
	} // for
}

//=========================== JJY ===========================
void setupTimeCode()
{
	int i ;         
        if(minu == 60) {
             minu = 0 ; 
             hour ++ ;
        }
        
        if(hour == 24) {
             hour = 0 ; 
             day ++ ;
             weekday ++ ;
        }
        
        if(weekday == 8)  
             weekday = 1;   
        
        if(day == 31)
            if(month ==4 || month ==6 || month ==9 || month ==11){
                  month ++ ;        
                  day = 1 ;
            }
        if(day == 32)
            if(month ==1 || month ==3 || month ==5 || month ==7 || month ==8 || month ==10 || month ==12){
                  month ++ ;
                  day = 1 ;
            }
        if(day == 30 && month==2 )
            if( (year % 400 ==0) || (year % 4 ==0) && (year % 100 !=0) ){ 
                  month ++ ;      
                  day = 1 ;     
            }
        if(day == 29 && month==2 )
            if( ! ((year % 400 ==0) || (year % 4 ==0) && (year % 100 !=0)) ){ 
                  month ++ ;
                  day = 1 ;
            }
            
        if(month==13){
            month = 1 ;
            year ++ ;
        }          
        
        date = dayofYear(year,month,day) ;
        
        /*      
        Serial.println("I am buffer: ");
        for(int i =0; i<18 ; i++){      
        Serial.print(buf[i]);      
        }
        Serial.print("\n--------------\n");
        */
        Serial.println("");
	setupTimeCode100(minu, 0);			// min  timecode 1~8                                                        
	timecode[0] = 2;
	setupTimeCode100(hour, 10);         // hour timecode 12~18      
	setupTimeCode100(date/10, 20);		// day of year timecode 20~29 
	setupTimeCode100(date%10*10, 30);	// day of year timecode 30~39 

	int parity1 = 0, parity2 = 0;
	for (i = 12; i < 20; i++) parity1 ^= (timecode[i] == 5);
	for (i =  1; i < 10; i++) parity2 ^= (timecode[i] == 5);
	timecode[36] = parity1 ? 5 : 8;	// timecode 36
	timecode[37] = parity2 ? 5 : 8;	// timecode 37

	setupTimeCode100(year%100, 40);	// timecode 45~49
	for (i = 44; i > 40; i--)
	timecode[i] = timecode[i-1];	// timecode 41~44
	timecode[40] = 8;				// timecode 40

	timecode[50] = (weekday & 4) ? 5 : 8;   
	timecode[51] = (weekday & 2) ? 5 : 8;
	timecode[52] = (weekday & 1) ? 5 : 8;
	timecode[59] = 2;

	/* dump */
	for (i = 0; i < 60; i++) {
		Serial.print(timecode[i], DEC);
		Serial.print(i % 10 == 9 ? "\r\n" : " ");
	} // for
}

void setupTimeCode100(int m, int i)
{
        timecode[i+0] = ((m/10) & 8) ? 5 : 8;     // 1->5   0->8  
        timecode[i+1] = ((m/10) & 4) ? 5 : 8;
        timecode[i+2] = ((m/10) & 2) ? 5 : 8;
        timecode[i+3] = ((m/10) & 1) ? 5 : 8;
        timecode[i+4] = 8;
        timecode[i+5] = ((m%10) & 8) ? 5 : 8;
        timecode[i+6] = ((m%10) & 4) ? 5 : 8;
        timecode[i+7] = ((m%10) & 2) ? 5 : 8;
        timecode[i+8] = ((m%10) & 1) ? 5 : 8;
        timecode[i+9] = 2;
}

int dayofYear(int y, int m, int d){
    int i ;
    int ans = 0 ;
    int mon[] = {31,28,31,30,31,30,31,31,30,31,30,31,30} ;
        
    for(i=0 ; i < m-1 ; i++){
		if(m != 1)
			ans += mon[i];
		else
			break; 
    }
    
    ans += d ;
    if( ( ( (y % 400 ==0) || (y % 4 ==0) && (y % 100 !=0) ) ) && (m > 2))
       ans += 1 ;
      
    return ans;
}

void genDuration(int second) {                 
	TCCR3A = _BV(COM3A0) | _BV(WGM30);
	delay(second);
	TCCR3A =_BV(COM3A1) | _BV(COM3A0);
	delay(1000-second);
}



