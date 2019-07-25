//l298n pins (pwm)
#define in1 4//left +ve 
#define in2 3//left -ve
#define ena 5
#define enb 6
#define in3 8//right +ve
#define in4 7//right -ve
//speed values 0~255
#define zero 0
#define one 51
#define two 51*2
#define three 51*3
#define four 51*4
#define five 51*5
//ir pins
#define front_s 9
#define left_s 10
#define right_s 11
//encoders
#define encl 12
#define encr 13
//loop variables
unsigned char rfront,rright,rleft;
unsigned char stage = 0;
unsigned int posx = 3;
unsigned int posy = 0;
//debugging flag
bool db = 0;
String func;
//staging compass
//0 -> .
//3    1
// <- 2 .
//
// 0 : row ~ col +
// 1 : row + col ~
// 2 : row ~ col -
// 3 : row - col ~

void update_location(){
  switch (stage)
  {
  case 0:
    posy++;
    break;
  case 1:
    posx++;
    break;
  case 2:
    posy--;
    break;
  case 3:
    posx--;
    break;
  default:
    break;
  }
}


//left motor: l
//  in1 back b
//  out1 => -ve
//  out2 => +ve
//  in2 forward f
//  out1 => +ve
//  out2 => -ve
//right motor: r
//  in3 back  b
//  out3 => -ve
//  out4 => +ve
//  in4 forward f
//  out3 => +ve
//  out4 => -ve
//  left -> l
//  right -> r
void move(char motor,char dir,char speed = zero){
  if(motor == 'l'){
    if(dir == 'b'){
      digitalWrite(in1,0);
      digitalWrite(in2,1);
      analogWrite(ena,speed);
      }else if(dir == 'f'){
              digitalWrite(in1,1);
              digitalWrite(in2,0);
              analogWrite(ena,speed);
        }else if(dir == 's'){//stop
          digitalWrite(in1,0); 
          digitalWrite(in2,0);
          analogWrite(ena,0);
        }
    }else{
      if(dir == 'b'){
        digitalWrite(in3,0);
        digitalWrite(in4,1);
        analogWrite(enb,speed);
        }else if(dir == 'f'){
          digitalWrite(in3,1);
          digitalWrite(in4,0);
          analogWrite(enb,speed);
          }else if(dir == 's'){//stop
            digitalWrite(in3,0);
            digitalWrite(in4,0);
            analogWrite(enb,0); 
            }
      }
  }

 
void stop(){
  move('l','s');
  move('r','s');
  }
void backward(char speed = one){
  move('l','b',speed);
  move('r','b',speed);
  }

void right(char speed = one){
  move('l','s');
  move('r','f',speed);
  }

void left(char speed = one){
  move('l','f',speed);
  move('r','s');
  }

void forward(char speed = one){
  update_location();
  move('l','f',speed);
  move('r','f',speed);
  //add delay to achieve 3cm
  //note : test with speed five
  //8.125 * 10^-3 sec
  delay(250);
  func += " forward ";
}

//void turnRight(int x){//x ??
//    digitalWrite(In1, LOW);
//    digitalWrite(In2, HIGH);
//    digitalWrite(In3, HIGH);
//    digitalWrite(In4, LOW);
//    
//    if(100<x<140){
//    analogWrite(EnA, 180); //the speed
//    analogWrite(EnB, 60); //the speed  
//        }else{
//      analogWrite(EnB, 50); //the speed
//      analogWrite(EnA, 50*(1/x)); //the speed  
//      
//      }
//      
//  //delay(5);
//}
//
//void turnLeft(int x){//x ??
//  digitalWrite(In1, HIGH);
//  digitalWrite(In2, LOW); 
//  digitalWrite(In3, LOW);
//  digitalWrite(In4, HIGH); 
//  if(140<x<180){
//    analogWrite(EnA, 50); 
//    analogWrite(EnB, 230); 
//  }else{
//    analogWrite(EnA, 50);
//    analogWrite(EnB, 50*(1/x));
//  }
//  //delay(5);
//} 

void ccw_90(){
  stage--;
  if(stage < 0){
    stage = 3;
  }
  //counter-clockwise : right
  move('r','f',five);
  move('l','b',five);
  func += " right 90 ";
  delay(5);    
}
void cw_90(){
  stage++;
  if(stage > 3){
    stage = 0;
  }
  //clockwise : left
  move('l','f',five);
  move('r','b',five);
  func += " left 90 ";
  delay(5);
    
}

void cw_180(){
  //clockwise : left
  move('l','f',five);
  move('r','b',five);
  delay(10);
}
void ccw_180(){
  //counter-clockwise : right
  move('r','f',five);
  move('l','b',five);
  delay(10);
}

bool end(){
//reaching middle
// middle = 1/2(map area / cell) in posx and posy
//>>> (5.76 / (3.24/100) ) / 2
//88.88888888888887
//middle consists of four cells
  if( (posx >= 88 && posx <= 90 )  && (posy >= 88 && posy <= 90 )){
    return 1;
  }
  return 0;
  
}

void search(){
  while(1){
    if(end()){
      func += " break ";
      break;
    }else if(!(digitalRead(left_s))){
      ccw_90();//counter clock-wise
      stop();
      forward();
      stop();
    }else if(!(digitalRead(front_s))){
      forward();
      stop();
    }else{
      cw_90();//clock-wise
      stop();
    }
    
    if(db){
      if(Serial.available()){
        Serial.print("row ,posx ");
        Serial.print(posx);
        Serial.print("; col ,posy ");
        Serial.print(posy);
        Serial.print("; curaction ");
        Serial.println(func);
        func = "";//repeat
      }
      delay(500);
    }
  }
}
void test (){
  forward();
  delay(1000);
  cw_90();
  delay(1000);
  ccw_90();
}
void test_delay(){
  forward();
  stop();
}
void setup() {
  //l298n pins
  pinMode(ena,1);  
  pinMode(in1,1);  
  pinMode(in2,1);
  pinMode(in3,1);
  pinMode(in4,1);
  pinMode(enb,1);
  //IR pins
  pinMode(front_s,0);
  pinMode(left_s,0);
  pinMode(right_s,0);
  //enc
  pinMode(encl,0);
  pinMode(encr,0);
  Serial.begin(9600);
  delay(500);
  if(Serial.available()){
    if(db){
      Serial.println("debugging enabled");
    }else{
      Serial.println("debugging disabled");
    }
  }
}
String reading = "";
void loop() {
  // search();
  if(Serial.available()){
    reading = Serial.read();
    if(reading == "t"){
      test();
    }else if(reading == "g"){
      test_delay();
    }
  }
}


