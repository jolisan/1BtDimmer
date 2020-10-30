#include "mbed.h"

InterruptIn botao(p5);
DigitalOut ledvermelho(p6);
DigitalOut ledazul(p7);
PwmOut intensidade(p8);

Timeout timeout;
Ticker ticker;

enum STATE {
   BAIXO,
   CIMA,
   FALL,
   MIN,
   RISE,
   MAX
};

int estadoatual;
bool estadobotao;
float frequencia;

void iniciar();
void pressionarBotao();
void soltarBotao();
void transicaoEstado();
void piscarLed();

int main() {
   iniciar();
   botao.rise(&pressionarBotao);
   botao.fall(&soltarBotao);
}

void iniciar(){
   ledazul = 1.0;
   ledvermelho = 0.0;
   intensidade = 1.0;
   estadoatual = CIMA;
   printf("CIMA\n");
}

void transicaoEstado(){
   timeout.attach( &piscarLed, 0.1);
   if(estadobotao){
      switch(estadoatual){
      case CIMA:
         estadoatual = RISE;
         printf("RISE\n");
         break;
      case BAIXO:
         estadoatual = FALL;
         printf("FALL\n");
         break;
      case FALL:
         if(intensidade == 0.0){
            estadoatual = MIN;
            printf("MIN\n");
         }
		 else{
            intensidade = intensidade - 0.05;
            printf("Intensidade: %.2f\n", intensidade.read());
         }
         break;
      case RISE:
         if(intensidade == 1.0){
            estadoatual = MAX;
            printf("MAX\n");
         }
		 else{
            intensidade = intensidade + 0.05;
            printf("Intensidade: %.2f\n", intensidade.read());
         }
         break;
      default:
         if(intensidade >= 1.0){
            estadoatual = MAX;
            printf("MAX\n");
         }
		 else if(intensidade <= 0.0){
            estadoatual = MIN;
            printf("MIN\n");
         }
         break;
      }
   }
}


void pressionarBotao(){
   printf("Pressione o botao\n");
   estadobotao = true;
   ticker.attach(&transicaoEstado, 1.0);
}

void soltarBotao(){
   printf("Liberar o botao\n");
   estadobotao = false;
   ticker.detach();
   if(estadoatual == MAX || estadoatual == CIMA || estadoatual == FALL){
      if(intensidade > 0.0){
         ledazul = 0.0;
         ledvermelho = 1.0;
         estadoatual = BAIXO;
         printf("BAIXO\n");
      }
   }
   else if(estadoatual == MIN || estadoatual == BAIXO || estadoatual == RISE){
      if(intensidade < 1.0){
         ledazul = 1.0;
         ledvermelho = 0.0;
         estadoatual = CIMA;
         printf("CIMA\n");
      }
   }
}

void piscarLed(){
if(estadoatual == FALL){
        frequencia = 1;
        ledvermelho = !ledvermelho;
    }
    else if(estadoatual == RISE){
        frequencia = 1;
        ledazul = !ledazul;
    }
    else if(estadoatual == MIN){
        frequencia = 0.1;
        ledvermelho = !ledvermelho;
    }
    else if(estadoatual == MAX){
        frequencia = 0.1;
        ledazul = !ledazul;
    }
    timeout.attach(&piscarLed, frequencia);
}