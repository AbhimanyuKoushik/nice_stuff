void sevenseg(int a,int b,int c,int d)
{
  digitalWrite(5, a); 
  digitalWrite(4, b); 
  digitalWrite(3, c); 
  digitalWrite(2, d);  
}
void setup() 
{
    pinMode(5, OUTPUT);  
    pinMode(4, OUTPUT);
    pinMode(3, OUTPUT);
    pinMode(2, OUTPUT);
}
void loop(){
sevenseg(0,0,0,0);
delay(1000);
sevenseg(0,0,0,1);
delay(1000);
sevenseg(0,0,1,0);
delay(1000);
sevenseg(0,0,1,1);
delay(1000);
sevenseg(0,1,0,0);
delay(1000);
sevenseg(0,1,0,1);
delay(1000);
sevenseg(0,1,1,0);
delay(1000);
sevenseg(0,1,1,1);
delay(1000);
sevenseg(1,0,0,0);
delay(1000);
sevenseg(1,0,0,1);
delay(1000);
}
