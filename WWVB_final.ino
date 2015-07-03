/*
  Wireless Networking Fundamentals and Applications - Lab2
  Team 7
  VLC
  Yen Su,MH Yang,WC Lin,Joe Huang,Jack Kao
*/

int led = 5;
int timecode[60], date, year, month, day, hour, minu, weekday ;

void setup()
{
       Serial.begin(9600);
    
       pinMode(led, OUTPUT);
       TCCR3A = _BV(COM3A0) | _BV(WGM30); //0x0F => fast PWM TOP=OCRnA
       TCCR3B = _BV(WGM33)  |  _BV(CS30); //CS30 0x01=> presaler = 1

       OCR3A = 67;    // set 67 for 60khz
}

char buf[18];    
int idx = 0 ;
int looptime = 0 ;
int flag = 0 ;                                     // let user input the time only at first  
int smile = 0 ;                                    // print out ^_^ on monitor for waiting input 

//int year, month, day, hour, min, weekday
 //  2015/04/10 22:34 7 
 //  0    2  4  6  8 10

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
        
        if(looptime!=0)  minu++; // minute+1 when enter again in the loop
        setupWWVBCode();         // initalize the timecode[60]
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
        
        for(int i = 0; i < 60; i++) {
          Serial.print("i = ");
          Serial.println(i);
          Serial.print("timecode = ");
          Serial.println(timecode[i]) ;
        
          if (timecode[i] == 0) { genDuration(200); } else if (timecode[i] == 1) { genDuration(500); } else { genDuration(800); }
        }     
}

int dayofYear(int y, int m, int d){
    int i ;
    int ans = 0 ;
    int mon[] = {31,28,31,30,31,30,31,31,30,31,30,31,30} ;
        
    for(i=0 ; i < m-1 ; i++){  //if is useless
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

//=====================WWVB============================
void setupWWVBCode(){
  int i , j;
  ///////////////////////minu up to hour/////////////////////
  
  if(minu == 60) {
             minu = 00 ; 
             hour ++ ;
        }
        
        if(hour == 24) {
             hour = 00 ; 
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
  
  ////////////////////////////////////////////////////////////////////////////////////////////////
  
  
  
  /////////////////////////preprocessing//////////////////////
  date = dayofYear(year,month,day) ;
  
  for(j = 0 ; j < 60 ; j++) 
  timecode[j]=0;
  
  timecode[0]=-1;
  
  if( ( (year % 400 ==0) || (year % 4 ==0) && (year % 100 !=0) ) )
  timecode[55]=1; //leap year set 1
   
   for(j = 0; j < 6 ; j++)
   timecode[10*j+9] = -1 ; //Marker  M

  timecode[37]=1;
 
  //minu begin
  int minukeep = minu;
  if(minu>=40){
    timecode[1]=1;
    minu -=40;
  }
  if(minu>=20){
    timecode[2]=1;
    minu -=20;
  }
  if(minu>=10){
    timecode[3]=1;
    minu -=10;
  }
  if(minu>=8){
    timecode[5]=1;
    minu -=8;
  }
  if(minu>=4){
    timecode[6]=1;
    minu -=4;
  }
  if(minu>=2){
    timecode[7]=1;
    minu -=2;
  }
  if(minu>=1){
    timecode[8]=1;
    minu -=1;
  }
  minu = minukeep;
  //hour begin
  int hourkeep = hour;
  if(hour>=20){
    timecode[12]=1;
    hour -=20;
  }
  if(hour>=10){
    timecode[13]=1;
    hour -=10;
  }
  if(hour>=8){
    timecode[15]=1;
    hour -=8;
  }
  if(hour>=4){
    timecode[16]=1;
    hour -=4;
  }
  if(hour>=2){
    timecode[17]=1;
    hour -=2;
  }
  if(hour>=1){
    timecode[18]=1;
    hour -=1;
  }
  hour = hourkeep;
  //dayofyear begin
  int datekeep = date;
  if(date>=200){
    timecode[22]=1;
    date -=200;
  }
  if(date>=100){
    timecode[23]=1;
    date -=100;
  }
  if(day>=80){
    timecode[25]=1;
    date -=80;
  }
  if(date>=40){
    timecode[26]=1;
     date -=40;
  }
  if(date>=20){
    timecode[27]=1;
     date -=20;
  }
  if( date>=10){
    timecode[28]=1;
     date -=10;
  }
  if( date>=8){
    timecode[30]=1;
     date -=8;
  }
  if( date>=4){
    timecode[31]=1;
     date -=4;
  }
  if( date>=2){
    timecode[32]=1;
     date -=2;
  }
  if( date>=1){
    timecode[33]=1;
     date -=1;
  }
  date = datekeep;
  //DUT1 36~38  and 40~43 ???


  //Year begin
  int yearkeep = year;
  year %= 100;  //(00-99)

  if(year>=80){
    timecode[45]=1;
     year -=80;
  }
  if(year>=40){
    timecode[46]=1;
     year -=40;
  }
  if(year>=20){
    timecode[47]=1;
     year -=20;
  }
  if(year>=10){
    timecode[48]=1;
     year -=10;
  }
  if(year>=8){
    timecode[50]=1;
     year -=8;
  }
  if(year>=4){
    timecode[51]=1;
     year -=4;
  }
  if(year>=2){
    timecode[52]=1;
     year -=2;
  }
  if(year>=1){
    timecode[53]=1;
     year -=1;
  }
  year = yearkeep;
  //bit 56 LSW??
  //bit 57 58

}

void genDuration(int second) {                 
 
   TCCR3A =_BV(COM3A1) | _BV(COM3A0);
   delay(second);
  
  TCCR3A = _BV(COM3A0) | _BV(WGM30);
   delay(1000-second);
}

