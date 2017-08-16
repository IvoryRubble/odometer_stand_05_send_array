//формат вывода:
//t
//
//counter* - номер срабатывания датчика с момента старта программы *=1-первого, *=2-второго
//dt* - время между последними двумя срабатываниями
//t* - время србатывания датчика с момента старта программы
#define IN3 6 
#define IN4 4
#define ENB 5
#define interruptPin1 2

//использовать в прерываниях микросекунды или миллисекунды
#define TIME_UNITS micros
//#define TIME_UNITS millis

const int arrayLenght = 100;
unsigned long data[arrayLenght];

volatile unsigned long counter = 0;

void setup() {
  Serial.begin(115200);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  digitalWrite(ENB, LOW);
  pinMode(interruptPin1, INPUT);
  pinMode(interruptPin2, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin1), sensorHandler, CHANGE);
  motor(140);
  Serial.println("counter1,dt1,t1,counter2,dt2,t2");
  //waitSerial();
}

void sensorHandler() {
  if (counter < arrayLenght) {
      data[counter] = micros();
      counter++;
  }
}

void loop() {
  if (counter > arrayLenght) {
    detachInterrupt(digitalPinToInterrupt(interruptPin1));
    printData();
    attachInterrupt(digitalPinToInterrupt(interruptPin1), sensorHandler, CHANGE);
  }
  delay(10);
  //motorControl(); //функция управления мотором
}

void printData() {
  for (int i = 0; i < dataLenght; i++) {
    Serial.println(data);
  }
}

void motorControl() {
  //функция управления мотором
  // мотор крутится от начальной скорости до 255, 
  //увеличивая скорость на величину шага
  //через интервал motorDuration
  static unsigned long tMotor;
  static unsigned long tMotorp = 0;
  static const unsigned long motorDuration = 5000; //интервал увеличения скорости
  static int motorSpeed[] = {140, 190, 250, -1}; //массив скоростей мотора
  static int motorI = 0;
  static bool started = false;
  if (started == false) {
    started = true;
    tMotorp = millis();
    motor(motorSpeed[motorI]);
    Serial.print("motorSpeed = ");
    Serial.println(motorSpeed[motorI]);
  }
  tMotor = millis();
  if (tMotor - tMotorp > motorDuration) {
    motorI++;
    if (motorSpeed[motorI] < 0) {
      motorI = 0;
      motor(0);
      started = false;
      Serial.println("end of testing");
      waitSerial();   
    }
    motor(motorSpeed[motorI]);
    Serial.print("motorSpeed = ");
    Serial.println(motorSpeed[motorI]);
    tMotorp = tMotor;
  }
}

void motor(int s) {//управление моторами s - скорость мотора                                                              
  int  n3, n4;
  if (s > 0) {
    n3 = HIGH;
    n4 = LOW;
  } else if (s < 0) {
    n3 = LOW;
    n4 = HIGH; 
  } else if (s == 0) {
    n3 = LOW;
    n4 = LOW; 
  }

  //calculate speed
  int absSpeed = abs(s);
  int finalSpeed = constrain(absSpeed, 0, 255); 

  //write it
  digitalWrite (IN3, n3);   digitalWrite (IN4, n4);
  analogWrite(ENB, finalSpeed);
}

void waitSerial() {
  while (Serial.available()) {
    Serial.read();
  }
  while (!Serial.available()) {}
  delay(300);
  while (Serial.available()) {
    Serial.read();
  }
}
