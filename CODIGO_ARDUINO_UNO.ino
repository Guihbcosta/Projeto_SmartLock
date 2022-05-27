#include <Keypad.h>
#include <Servo.h>

#define tamSenha 5
Servo myservo;

char Cod[tamSenha];
char Senha[tamSenha]="692A";
byte keycount=0;

const byte ROWS = 4; 
const byte COLS = 4; 

boolean wifiConectado = false;
String estadoTranca = "";

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6}; 
byte colPins[COLS] = {5, 4, 3, 2}; 

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void setup(){
  Serial.begin(9600);
  myservo.attach(12);
  
  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
}
  
void loop(){
  //boolean StringReady = false;
  String IncomingString = "";
  
  while (Serial.available()) {
    IncomingString = Serial.readString();
    if (IncomingString == "ESP ok") {
      wifiConectado = true;
      digitalWrite( LED_BUILTIN, HIGH);
    }  
    //StringReady = true;
  }

  if (estadoTranca == "aberta") {
    Serial.write("1");
    digitalWrite( LED_BUILTIN, LOW);
    estadoTranca = "";
  }
  if (estadoTranca == "fechada") {
    Serial.write("2");
    digitalWrite( LED_BUILTIN, HIGH);
    estadoTranca = "";
  }

  if (wifiConectado) {
    char customKey = customKeypad.getKey();
    if(customKey){
      //Serial.println(customKey);
      Cod[keycount]=customKey;
      //Serial.print(Cod[keycount]);
      keycount++;
    }
    if(keycount==tamSenha-1){
      //Serial.println(" ");
      
      if(!strcmp(Cod,Senha)){
        //Serial.println("Correct");
        myservo.write(90); //unlock
        digitalWrite(10, HIGH);
        digitalWrite(11, LOW);
        estadoTranca = "aberta";
        delay(1000);
        
      }
      else{
         //Serial.println("Incorrect");
         myservo.write(0); //lock
         digitalWrite(11, HIGH);
         digitalWrite(10, LOW);    
         estadoTranca = "fechada";
         delay(1000);
      }
      
      deleteCount();
    }
  }
  
}
  
void deleteCount(){
  while(keycount !=0){
    Cod[keycount--]=0;
  }
  return;
}
