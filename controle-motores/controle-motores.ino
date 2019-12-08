    
const int EnableL = 6;
const int HighL = 10;       // LEFT SIDE MOTOR
const int LowL =11;

const int EnableR = 5;
const int HighR = 9;       //RIGHT SIDE MOTOR
const int LowR = 8;

const int D0 = A0;       //Raspberry pin 21    LSB
const int D1 = A1;       //Raspberry pin 22
const int D2 = A2;       //Raspberry pin 23
const int D3 = A3;       //Raspberry pin 24    MSB
  
int a,b,c,d;
int data = 0;
int last_data = 3;



void setup() {
    Serial.begin(9600);
    Serial.println("Carrinho iniciado");
    pinMode(EnableL, OUTPUT);      
    pinMode(HighL, OUTPUT);
    pinMode(LowL, OUTPUT);
    
    
    pinMode(EnableR, OUTPUT);
    pinMode(HighR, OUTPUT);
    pinMode(LowR, OUTPUT);
    
    pinMode(D0, INPUT_PULLUP);
    pinMode(D1, INPUT_PULLUP);
    pinMode(D2, INPUT_PULLUP);
    pinMode(D3, INPUT_PULLUP);
}

void Data()
{
   a = digitalRead(D0);
   b = digitalRead(D1);
   c = digitalRead(D2);
   d = digitalRead(D3);

   data = 8*d+4*c+2*b+a;
}

void Forward()
{
  digitalWrite(HighL, LOW);
  digitalWrite(LowL, HIGH);
  analogWrite(EnableL,200);

  digitalWrite(HighR, LOW);
  digitalWrite(LowR, HIGH);
  analogWrite(EnableR,200);
  
}


void Backward()
{
  digitalWrite(HighL, HIGH);
  digitalWrite(LowL, LOW);
  analogWrite(EnableL,200);

  digitalWrite(HighR, HIGH);
  digitalWrite(LowR, LOW);
  analogWrite(EnableR,200);
  
}

void Stop()
{
  digitalWrite(HighL, LOW);
  digitalWrite(LowL, HIGH);
  analogWrite(EnableL,0);

  digitalWrite(HighR, LOW);
  digitalWrite(LowR, HIGH);
  analogWrite(EnableR,0);
  
}

void Left1()
{
  digitalWrite(HighL, LOW);
  digitalWrite(LowL, HIGH);
  analogWrite(EnableL,200);

  digitalWrite(HighR, LOW);
  digitalWrite(LowR, HIGH);
  analogWrite(EnableR,180);
  
}

void Left2()
{
  digitalWrite(HighL, LOW);
  digitalWrite(LowL, HIGH);
  analogWrite(EnableL,200);

  digitalWrite(HighR, LOW);
  digitalWrite(LowR, HIGH);
  analogWrite(EnableR,170);
  
}


void Left3()
{
  digitalWrite(HighL, LOW);
  digitalWrite(LowL, HIGH);
  analogWrite(EnableL,200);

  digitalWrite(HighR, LOW);
  digitalWrite(LowR, HIGH);
  analogWrite(EnableR,160);
  
}

void Right1()
{
  digitalWrite(HighL, LOW);
  digitalWrite(LowL, HIGH);
  analogWrite(EnableL,180);

  digitalWrite(HighR, LOW);
  digitalWrite(LowR, HIGH);
  analogWrite(EnableR,200);
}
void Right2()
{
  digitalWrite(HighL, LOW);
  digitalWrite(LowL, HIGH);
  analogWrite(EnableL,170);

  digitalWrite(HighR, LOW);
  digitalWrite(LowR, HIGH);
  analogWrite(EnableR,200);   
  
}

void Right3()
{
  digitalWrite(HighL, LOW);
  digitalWrite(LowL, HIGH);
  analogWrite(EnableL,160);

  digitalWrite(HighR, LOW);
  digitalWrite(LowR, HIGH);
  analogWrite(EnableR,200);   
}



void loop() 
{
  Data();
  Serial.print("Data: ");
  Serial.println(data);
  switch(data){
    case 0:
      Forward();
      break;
    case 1:
      Right1();
      break;
    case 2:
      Right2();
      break;
    case 3:
      Right3();
      break;
    case 4:
      Left1();
      break;
    case 5:
      Left2();
      break;
    case 6:
      Left3();
      break;
    case 7:
      analogWrite(EnableR,0);
      analogWrite(EnableL,0);
      delay(5000);
      analogWrite(EnableR,200);
      analogWrite(EnableL,200);
      delay(500);
    default:
      Stop();
      break;
    }
    delay(200);
    Stop();
    delay(100);
}


/*
void loop(){
  Stop();
  delay(5000);
  
  Forward();
  delay(2000);
  Stop();
  delay(2000);
  
  Right1();
  delay(2000);
  Stop();
  delay(2000);
  
  Right2();
  delay(2000);
  Stop();
  delay(2000);
  
  Right3();
  delay(2000);
  Stop();
  delay(2000);
  
  Left1();
  delay(2000);
  Stop();
  delay(2000);
  
  Left2();
  delay(2000);
  Stop();
  delay(2000);
  
  Left3();
  delay(2000);
  Stop();
}
*/

/*
void loop()
{
  for(int i = 0; i < 50; i++)
  {
    delay(2000);
    digitalWrite(HighL, LOW);
    digitalWrite(LowL, HIGH);
    analogWrite(EnableL,255);

    digitalWrite(HighR, LOW);
    digitalWrite(LowR, HIGH);
    analogWrite(EnableR,255);   
  
    delay(100);
    Stop();
  }
  
}
*/
