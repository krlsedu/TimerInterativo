#include <LiquidCrystal.h> 
#include <EEPROM.h>
 
LiquidCrystal lcd(10, 9, 5, 4, 3, 2);

int pinoSensorTemp = 0; //pino que está ligado o terminal central do LM35 (porta analogica 0)
int pinoSensorLum = 1;
int addr = 0;  
int ledPin1 = 8;
int ledPin2 = 12;
int ledPin3 = 11;
int ligado1 = 0;
int ligado2 = 0;
int eraNoite = 0;
int ehNoite = 0;
int ligouUltimo = 0;
int count = 0;
int i = 0;
unsigned long tempoInicioDes1 = 0;
unsigned long tempoInicioLig1 = 0;
unsigned long tempoInicioDes2 = 0;
unsigned long tempoInicioLig2 = 0;
unsigned long tempoAgora = 0;
unsigned long tempoDesligado1 = 0;
unsigned long tempoLigado1 = 0;
unsigned long tempoDesligado2 = 0;
unsigned long tempoLigado2 = 0;

unsigned long tempoDesligadoMin1 = 0;
unsigned long tempoLigadoMin1 = 0;
unsigned long tempoDesligadoMin2 = 0;
unsigned long tempoLigadoMin2 = 0;

unsigned long esperarDesligado = 0;
unsigned long esperarLigado = 0;
unsigned long esperarDesligadoMin = 0;
unsigned long esperarLigadoMin = 0;
unsigned long tempoAnt =0;
unsigned long tempoAntSensors =0;
unsigned long valorLidoTemp = 0; //valor lido na entrada analogica
unsigned long valorLidoLum = 0; //valor lido na entrada analogica
float temperatura = 0; //valorLido convertido para temperatura


int botaoMais = 0;
int botaoMenos = 0;



void setup() {
  Serial.begin(9600); //Inicializa comunicação Serial
  lcd.begin(16, 2); 
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  tempoInicioLig1 = millis();
  tempoInicioDes2 = millis();
  ligado1=1;
  ligouUltimo = 1;
  ligado2=0;
  digitalWrite(ledPin3, HIGH);
  esperarLigado = ((EEPROM.read(addr)));
  esperarLigado = (esperarLigado*1000*60);
}

void loop() {  
  tempoAgora = millis();
  botaoMais = digitalRead(7);
  botaoMenos = digitalRead(6);
  
  if(count == 0){
    valorLidoLum = 0;
    i=0;
    while(i<50){
      valorLidoLum = valorLidoLum + analogRead(pinoSensorLum);
      i++;
    }
    valorLidoLum = valorLidoLum/i;
    valorLidoTemp = 0;
    i=0;
    while(i<50){
      valorLidoTemp = valorLidoTemp + analogRead(pinoSensorTemp);
      i++;
    }
    valorLidoTemp = valorLidoTemp/i;
    Serial.println(valorLidoTemp);
    Serial.println(valorLidoLum);
    count++;
  }else{
    if((millis() - tempoAntSensors) >=300000){      
    //if((millis() - tempoAntSensors) >=120000){      
      valorLidoLum = 0;
      i=0;
      while(i<50){
        valorLidoLum = valorLidoLum + analogRead(pinoSensorLum);
        i++;
      }
      valorLidoLum = valorLidoLum/i; 
      Serial.println(valorLidoLum);     
      tempoAntSensors = millis();
    }else{
      valorLidoTemp = 0;
      i=0;
      while(i<50){
        valorLidoTemp = valorLidoTemp + analogRead(pinoSensorTemp);
        i++;
      }
      valorLidoTemp = valorLidoTemp/i;
    }
  }
  temperatura = (valorLidoTemp * 0.00488)* 100;  
  
  if (botaoMais == HIGH){
    esperarLigado = esperarLigado + 60000;
    EEPROM.write(addr,esperarLigado/1000/60);
    lcd.setCursor(0,1);
    if(esperarLigado/1000/60 <10){
      lcd.print("L 0");
      lcd.print(esperarLigado/1000/60); 
    }else{
      lcd.print("L ");
      lcd.print(esperarLigado/1000/60); 
    }
    delay(250);
  }
  if (botaoMenos == HIGH){
    esperarLigado = esperarLigado - 60000;
    EEPROM.write(addr,esperarLigado/1000/60);
    lcd.setCursor(0,1);
    if(esperarLigado/1000/60 <10){
      lcd.print("L 0");
      lcd.print(esperarLigado/1000/60); 
    }else{
      lcd.print("L ");
      lcd.print(esperarLigado/1000/60); 
    }
    delay(250);
  }
  
  if (temperatura <= 36){
    esperarDesligado = ((2160000+(6000*3))-(temperatura*1000*60));
  }else{
    esperarDesligado = (60000*3);
  }
  
  if(valorLidoLum < 100)
  {
    digitalWrite(ledPin2, HIGH);
    esperarDesligado = (esperarDesligado*12);
    if (esperarDesligado > (3600000*3)){
      esperarDesligado = (3600000*3);
    }
    ehNoite =1;
  }
  else
  {
    digitalWrite(ledPin2, LOW);
    if (ehNoite == 1){
      eraNoite = 1;
      ehNoite =0;      
    }
  }
  
  if(eraNoite == 1 && ligouUltimo == 2){
      tempoInicioDes2 = tempoAgora;
      Serial.println("eraNoite == 1 && ligouUltimo == 2");
      eraNoite = 0;
  }else{
    if(eraNoite == 1 && ligouUltimo == 1){
      tempoInicioDes1 = tempoAgora;      
      Serial.println("eraNoite == 1 && ligouUltimo == 1");
      eraNoite = 0;
    }
  }
  
  if(ligado1==0){
    tempoDesligado1 = tempoAgora-tempoInicioDes1;
    tempoDesligadoMin1 = (tempoDesligado1/1000/60);    
    lcd.setCursor(10,0);
    if(tempoDesligadoMin1 < 10){
      lcd.print("1D   ");
      lcd.print(tempoDesligadoMin1);
    }else{
      if(tempoDesligadoMin1<100 && tempoDesligadoMin1>=10){
        lcd.print("1D  ");
        lcd.print(tempoDesligadoMin1);
      }else{
        lcd.print("1D ");
        lcd.print(tempoDesligadoMin1);
      }
    }
  }else{
    tempoLigado1 = tempoAgora-tempoInicioLig1;
    tempoLigadoMin1 = (tempoLigado1/1000/60);
    lcd.setCursor(10,0);
    if(tempoLigadoMin1 < 10){
      lcd.print("1L   ");
      lcd.print(tempoLigadoMin1);
    }else{
      if(tempoLigadoMin1 < 100 && tempoLigadoMin1 >= 10){
        lcd.print("1L  ");
        lcd.print(tempoLigadoMin1);
      }else{
        lcd.print("1L  ");
        lcd.print(tempoLigadoMin1);
      }
    }
  }
  
  if(ligado2==0){
    tempoDesligado2 = tempoAgora-tempoInicioDes2;
    tempoDesligadoMin2 = (tempoDesligado2/1000/60);    
    lcd.setCursor(10,1);
    if(tempoDesligadoMin2 < 10){
      lcd.print("2D   ");
      lcd.print(tempoDesligadoMin2);
    }else{
      if(tempoDesligadoMin2<100 && tempoDesligadoMin2>=10){
        lcd.print("2D  ");
        lcd.print(tempoDesligadoMin2);
      }else{
        lcd.print("2D ");
        lcd.print(tempoDesligadoMin2);
      }
    }
  }else{
    tempoLigado2 = tempoAgora-tempoInicioLig2;
    tempoLigadoMin2 = (tempoLigado2/1000/60);
    lcd.setCursor(10,1);
    if(tempoLigadoMin2 < 10){
      lcd.print("2L   ");
      lcd.print(tempoLigadoMin2);
    }else{
      if(tempoLigadoMin2 < 100 && tempoLigadoMin2 >= 10){
        lcd.print("2L  ");
        lcd.print(tempoLigadoMin2);
      }else{
        lcd.print("2L  ");
        lcd.print(tempoLigadoMin2);
      }
    }
  }
  
  esperarDesligadoMin = (esperarDesligado/1000/60);
  esperarLigadoMin = (esperarLigado/1000/60);
  
  if ((millis() - tempoAnt)>1000){
    lcd.setCursor(0,0); 
    lcd.print("Tmp ");
    lcd.print(temperatura);
    tempoAnt = millis();
    lcd.setCursor(0,1);
    if(esperarLigadoMin <10){
      lcd.print("L 0");
      lcd.print(esperarLigadoMin); 
    }else{
      lcd.print("L ");
      lcd.print(esperarLigadoMin); 
    }
    if(esperarDesligadoMin <10){
      lcd.print(" D 0");
      lcd.print(esperarDesligadoMin); 
    }else{
      if (esperarDesligadoMin>=100){
        lcd.print(" D");
        lcd.print(esperarDesligadoMin);
      }else{
        lcd.print(" D ");
        lcd.print(esperarDesligadoMin);
      } 
    }
  }
  

  
  if(tempoDesligado1 >= esperarDesligado && ligado1 == 0){
    digitalWrite(ledPin3, HIGH);
    ligado1 = 1;
    tempoInicioLig1 =millis();
    Serial.print("Ligou 1 em ");
    Serial.println(tempoDesligadoMin1);
    Serial.println(tempoDesligado1);
    Serial.println(temperatura);
    tempoDesligado1 = 0;
    ligouUltimo = 1;
  }
  else{
    if(tempoLigado1 >= esperarLigado && ligado1 ==1){
      digitalWrite(ledPin3, LOW);
      ligado1 = 0;
      tempoInicioDes1 =millis();
      Serial.print("Desligou 1 em ");
      Serial.println(tempoLigadoMin1);
      Serial.println(tempoLigado1);
      Serial.println(temperatura);      
      tempoLigado1 = 0;
    }
  }
  
  if(tempoDesligado2 >= esperarDesligado && ligado2 == 0){
    digitalWrite(ledPin1, HIGH);
    ligado2 = 1;
    tempoInicioLig2 =millis();
    Serial.print("Ligou 2 em ");
    Serial.println(tempoDesligadoMin2);
    Serial.println(tempoDesligado2);
    Serial.println(temperatura);
    tempoDesligado2 = 0;
    ligouUltimo = 2;
  }
  else{
    if(tempoLigado2 >= esperarLigado && ligado2 ==1){
      digitalWrite(ledPin1, LOW);
      ligado2 = 0;
      tempoInicioDes2 =millis();
      Serial.print("Desligou 2 em ");
      Serial.println(tempoLigadoMin2);
      Serial.println(tempoLigado2);
      Serial.println(temperatura);
      tempoLigado2 = 0;      
    }
  }
}
