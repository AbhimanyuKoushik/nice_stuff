void sevenseg(int a, int b, int c, int d)
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
void incrdisp(int a, int b, int c, int d){
    int D = ((!a)&b&c&d)|(a&(!b)&(!c)&(!d));
    int C = ((!a)&(b)&(!c))|((!a)&(!b)&(c)&(d))|((!a)&(b)&(!d));
    int B = ((!a)&(!c)&(d))|((!a)&(c)&(!d));
    int A = ((!a)&(!c)&(!d))|((!a)&(c)&(!d))|((!b)&(!c)&(!d));
    sevenseg(D, C, B, A);
}
void loop(){
incrdisp(0,0,0,0);
delay(1000);
incrdisp(0,0,0,1);
delay(1000);
incrdisp(0,0,1,0);
delay(1000);
incrdisp(0,0,1,1);
delay(1000);
incrdisp(0,1,0,0);
delay(1000);
incrdisp(0,1,0,1);
delay(1000);
incrdisp(0,1,1,0);
delay(1000);
incrdisp(0,1,1,1);
delay(1000);
incrdisp(1,0,0,0);
delay(1000);
incrdisp(1,0,0,1);
delay(1000);
}
