#define CLOCK 13

#define Q0 6
#define Q1 7
#define Q2 8
#define Q3 9

#define D0 2
#define D1 3
#define D2 4
#define D3 5

int write(int A, int B, int C, int D){
  digitalWrite(D0, A);
  digitalWrite(D2, B);
  digitalWrite(D1, C);
  digitalWrite(D3, D);
}

int A(int W, int X, int Y, int Z){
  return (!W);
}

int B(int W, int X, int Y, int Z){
  return (!W && X) || (W && !X && !Z);
}

int C(int W, int X, int Y, int Z){
  return (!X && Y) || (!W && Y) || (W && X && !Y);
}

int D(int W, int X, int Y, int Z){
  return (!W && Z) || (W && X && Y);
}


void setup() {
  Serial.begin(9600);
  pinMode(CLOCK, OUTPUT);

  pinMode(Q0, INPUT);
  pinMode(Q1, INPUT);
  pinMode(Q2, INPUT);
  pinMode(Q3, INPUT);

  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
}

void loop() {
  digitalWrite(CLOCK, HIGH);

  Serial.print(String(digitalRead(Q0)) + " " + String(digitalRead(Q1)) + " " + String(digitalRead(Q2)) + " " + String(digitalRead(Q3)) + "\n");

  write(A(digitalRead(Q0),digitalRead(Q1),digitalRead(Q2),digitalRead(Q3)),
        B(digitalRead(Q0),digitalRead(Q1),digitalRead(Q2),digitalRead(Q3)),
        C(digitalRead(Q0),digitalRead(Q1),digitalRead(Q2),digitalRead(Q3)),
        D(digitalRead(Q0),digitalRead(Q1),digitalRead(Q2),digitalRead(Q3)));
 
  delay(1);
  digitalWrite(CLOCK, LOW);
  delay(999);
}
