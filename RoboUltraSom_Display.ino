/*
     
     Software de controle do robô do Curso de Arduino WR Kits Channel
     usando LCD Keypad Shield, Controladora Motores, Servo Motor e Sensor Ultrasom
     
*/

// --- Bibliotecas Auxiliares ---
#include <Servo.h>                            //Biblioteca para controle de servos
#include <LiquidCrystal.h>                    //Biblioteca para apresentação de dados no ShieldDisplay

// --- Mapeamento de Hardware ---
//#define EN1     ???0                             //Enable driver
//#define EN2     ???1                             //Enable driver
#define IN1     0                             //Driver L293D
#define IN2     1                             //Driver L293D
#define IN3     2                             //Driver L293D
#define IN4     3                             //Driver L293D
#define trig   12                             //Saída para o pino de trigger do sensor
#define echo   13                             //Entrada para o pino de echo do sensor
#define serv   11                             //Saída para o servo motor 
#define pwm    A1                             //Entrada analógica para ajuste de velocidade


// --- Protótipo das Funções Auxiliares ---
float measureDistance();                      //Função para medir, calcular e retornar a distância em cm
void trigPulse();                             //Função que gera o pulso de trigger de 10µs
void speedAdjust();                           //Função para ajustar velocidade dos motores
void decision();                              //Função para tomada de decisão. Qual melhor caminho?
void robotAhead();                            //Função para mover o robô para frente
void robotBack();                             //Função para mover o robô para trás
void robotRight();                            //Função para mover o robô para direita
void robotLeft();                             //Função para mover o robô para esquerda
void robotStop();                             //Função para parar o robô


// --- Objetos ---
Servo servo1;                                 //Cria objeto para o servo motor
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);  


// --- Variáveis Globais ---
int dist_cm;                                //Armazena a distância em centímetros entre o robô e o obstáculo
int dist_right;                             //Armazena a distância em centímetros da direita
int dist_left;                              //Armazena a distância em centímetros da esquerda
int   adc = 0x00;                           //Armazena o valor lido na entrada analógica


// --- Configurações Iniciais ---
void setup() 
{
 for(char i=0; i<3; i++) pinMode(i, OUTPUT); //Saídas para o driver L293D
 //pinMode(EN1, OUTPUT);                        //Saída para controle de PWM do driver
 //pinMode(EN2, OUTPUT);                        //Saída para controle de PWM do driver
 pinMode(trig, OUTPUT);                       //Saída para o pulso de trigger
 pinMode(serv, OUTPUT);                       //Saída para o servo motor
 pinMode(echo, INPUT);                        //Entrada para o pulso de echo
 
 servo1.attach(serv);                         //Objeto servo1 no pino de saída do servo
   
 digitalWrite(trig, LOW);                     //Pino de trigger inicia em low

 //for(int i=0; i<180; i++){
 // delay(30);
 // servo1.write(i);
 // }
   
 delay(2000);                                  //Aguarda meio segundo antes de iniciar
 servo1.write(80);                             //Centraliza servo
 delay(2000);
 
 // for(int i=180; i>0; i--){
 // delay(30);
 // servo1.write(i);
 // }

 lcd.begin(16, 2);                            //Inicializa o tamanho do Display
 
} //end setup


// --- Loop Infinito ---
void loop() 
{
  speedAdjust();
  
  robotAhead();
  
  dist_cm = measureDistance();
  
  lcd.clear();
  lcd.setCursor(0,0);                         //Inicia o texto na primeira coluna e primeira linha do Display
  lcd.print("Distancia");                     //Escreve o texto da primeira linha
  lcd.setCursor(10,0);                        //Continua o texto na décima primeira coluna da primeira linha do Display
  lcd.print(dist_cm);                         //Escreve o valor da distancia no display
  lcd.setCursor(14,0);                        //Continua o texto 
  lcd.print("cm");                            //Escreve o resto do teste no Display
 
  KeypadPressionado();
  
  if(dist_cm < 20)                            //distância menor que 20 cm?
  {
      decision();
  }

   
  delay(100);
  

} //end loop


// --- Desenvolvimento das Funções Auxiliares ---

float measureDistance()                       //Função que retorna a distância em centímetros
{
  float pulse;                                //Armazena o valor de tempo em µs que o pino echo fica em nível alto
        
  trigPulse();                                //Envia pulso de 10µs para o pino de trigger do sensor
  
  pulse = pulseIn(echo, HIGH);                //Mede o tempo em que echo fica em nível alto e armazena na variável pulse
    
  /*
    >>> Cálculo da Conversão de µs para cm:
    
   Velocidade do som = 340 m/s = 34000 cm/s
   
   1 segundo = 1000000 micro segundos
   
      1000000 µs - 34000 cm/s
            X µs - 1 cm
            
                  1E6
            X = ------- = 29.41
                 34000
                 
    Para compensar o ECHO (ida e volta do ultrassom) multiplica-se por 2
    
    X' = 29.41 x 2 = 58.82
 
  */
  
  return (pulse/58.82);                      //Calcula distância em centímetros e retorna o valor
  
  
} //end measureDistante


void trigPulse()                             //Função para gerar o pulso de trigger para o sensor HC-SR04
{
   digitalWrite(trig,HIGH);                  //Saída de trigger em nível alto
   delayMicroseconds(10);                    //Por 10µs ...
   digitalWrite(trig,LOW);                   //Saída de trigger volta a nível baixo

} //end trigPulse


void speedAdjust()                           //Função para ajustar velocidade dos motores
{
    adc = analogRead(pwm);                   //Lê entrada analógica e armazena valor em adc
    
    adc = map(adc, 0, 1023, 0, 255);         //Ajusta valor para variar entre 0 e 255
    
//    analogWrite(EN1,adc);                    //PWM de acordo com ajuste do potenciômetro em EN1
//    analogWrite(EN2,adc);                    //PWM de acordo com ajuste do potenciômetro em EN2

} //end speedAdjust


void decision()                              //Compara as distâncias e decide qual melhor caminho a seguir
{
   robotStop();                              //Para o robô
   delay(500);                               //Aguarda 500ms
   servo1.write(0);                          //Move sensor para direita através do servo
   delay(500);                               //Aguarda 500ms
   dist_right = measureDistance();           //Mede distância e armazena em dist_right
   delay(2000);                              //Aguarda 2000ms
   servo1.write(180);                        //Move sensor para esquerda através do servo
   delay(500);                               //Aguarda 500ms
   dist_left = measureDistance();            //Mede distância e armazena em dis_left
   delay(2000);                              //Aguarda 2000ms
   servo1.write(80);                         //Centraliza servo
   delay(500);
   
   if(dist_right > dist_left)                //Distância da direita maior que da esquerda?
   {                                         //Sim...
   
      robotBack();                           //Move o robô para trás
      delay(600);                            //Por 600ms
      robotRight();                          //Move o robô para direita
      delay(2000);                           //Por 2000ms
      robotAhead();                          //Move o robô para frente
   
   } //end if
   
   else                                      //Não...
   {
   
      robotBack();                           //Move o robô para trás
      delay(600);                            //Por 600ms
      robotLeft();                           //Move o robô para esquerda
      delay(2000);                            //Por 2000ms
      robotAhead();                          //Move o robô para frente
   
   } //end else
   

} //end decision


void robotAhead()                            //Move o robô para frente
{
   digitalWrite(IN1,  LOW);
   digitalWrite(IN2, HIGH);
   digitalWrite(IN3,  LOW);
   digitalWrite(IN4, HIGH);

} //end robotAhead


void robotBack()                            //Move o robô para trás
{
//   digitalWrite(EN1, HIGH);
//   digitalWrite(EN2, HIGH);
   digitalWrite(IN1, HIGH);
   digitalWrite(IN2,  LOW);
   digitalWrite(IN3, HIGH);
   digitalWrite(IN4,  LOW);

} //end robotBack


void robotRight()                            //Move o robô para direita
{
//   digitalWrite(EN1, HIGH);
//   digitalWrite(EN2, HIGH);
   digitalWrite(IN1,  LOW);
   digitalWrite(IN2, HIGH);
   digitalWrite(IN3, HIGH);
   digitalWrite(IN4,  LOW);

} //end robotRight


void robotLeft()                            //Move o robô para esquerda
{
//   digitalWrite(EN1, HIGH);
//   digitalWrite(EN2, HIGH);
   digitalWrite(IN1, HIGH);
   digitalWrite(IN2,  LOW);
   digitalWrite(IN3,  LOW);
   digitalWrite(IN4, HIGH);

} //end robotLeft


void robotStop()                            //Para o robô
{
   digitalWrite(IN1,  LOW);
   digitalWrite(IN2,  LOW);
   digitalWrite(IN3,  LOW);
   digitalWrite(IN4,  LOW);

} //end robotStop

void KeypadPressionado()  

{  
 int botao;  
 botao = analogRead (0);  //Leitura do valor da porta analógica A0
// botao = map(botao, 0, 1023, 0, 255);         //Ajusta o valor entre 0 e 255
 
 lcd.setCursor(0,1);  
 if (botao < 80) {  
  lcd.print ("Direita -> 0   ");
  servo1.write(0);                            //Move servo a 0 graus
 }  
 else if (botao < 200) {  
  lcd.print ("Cima -> 60     ");
  servo1.write(60);                            //Move servo a 60 graus
 }  
 else if (botao < 400){  
  lcd.print ("Baixo -> 120   ");  
  servo1.write(120);                           //Move servo a 120 graus
 }  
 else if (botao < 600){  
  lcd.print ("Esquerda -> 180");  
  servo1.write(180);                           //Move servo a 180 graus
 }  
 else if (botao < 800){  
  lcd.print ("Select  ");
  servo1.write(80);                            //Centraliza servo
 }  
} //End KeypadPressionado()
