#include <Servo.h>// biblioteca do servo

const int pino_sensor = 2; // sensor infra vermelho
const int pino_buzzer = 11; // saída do buzzer
char array_de_comando[10]; // Array de comandos que guarda todos os comandos bluetooth
int Posicao_no_array = 0; // posição de inserção no Array
bool sistemaAtivo = false; // variavel booleana que determina se o sistema está ativo
bool alarmeAtivo = true; // variavel booleana que determina se o alarme está tocando
unsigned long inicioAlarme; // variavel de controle de tempo 
unsigned long ultimaTrocaBuzzer = 0;//variavel de controle de troca de estado
bool estadoBuzzer = false; // variavel booleana que determina se o buzzer está tocando
const int intervaloBuzzer = 500; // variavel que determina o tempo até a troca de estado do buzzer

const int pino_led_alarme = 4; // led do alarme
const int LEDGARAGEM = A0; // led da garagem
const int LEDSALA = A1; // led da sala
const int LEDCOZINHA = A2; // led da cozinha
const int LEDQUARTO = A3; // led do quarto
const int LEDBANHEIRO = A4; // led do banheiro

bool ledgaragemState = false; // variavel booleana que determina o estado do led (aceso ou apagado)
bool ledsalaState = false; // variavel booleana que determina o estado do led (aceso ou apagado)
bool ledcozinhaState = false; // variavel booleana que determina o estado do led (aceso ou apagado)
bool ledquartoState = false; // variavel booleana que determina o estado do led (aceso ou apagado)
bool ledbanheiroState = false; // variavel booleana que determina o estado do led (aceso ou apagado)
Servo myServo;
const int pinServo = 5; // servo motor
unsigned long tempoServoInicio = 0; //variavel de controle de tempo
bool servoMovendo = false; // variavel booleana que verifica o estado do servo (em movimento ou não)
int anguloAtual = 60; // Ângulo atual do servo
int anguloAlvo = 60; // Ângulo alvo do servo







void setup() {
    myServo.attach(pinServo);
    pinMode(pino_sensor, INPUT); // sensor infravermelho
    pinMode(pino_led_alarme, OUTPUT); // led do alarme
    pinMode(pino_buzzer, OUTPUT); // buzzer
    pinMode(LEDGARAGEM, OUTPUT); // led da garagem
    pinMode(LEDSALA, OUTPUT); // led da sala
    pinMode(LEDCOZINHA, OUTPUT); // led da cozinha
    pinMode(LEDQUARTO, OUTPUT); // led do quarto
    pinMode(LEDBANHEIRO, OUTPUT); // led do banheiro
    Serial.begin(9600);
}

void loop() {
    verificaComandosBluetooth(); // Recebe os comandos e os coloca no array
    processa_comando_array(); // Processa os comandos armazenados no array
    
    if (sistemaAtivo) {
        verificaSensor();
        tocaBuzzerIntermitente();
    }
    
    moverServoComMillis(); // Move o servo gradualmente se necessário
}

void verificaComandosBluetooth() {
    if (Serial.available() > 0) {
   char comandoRecebido = Serial.read(); //comando bluetooth é armazenado na string
        
        // Armazena o comando no array se houver espaço
        if (Posicao_no_array < sizeof(array_de_comando)) // retorna o tamanho do array_de_comando e compara para ver se ja está na ultima posição do array.
        {
            array_de_comando[Posicao_no_array++] = comandoRecebido; // guarda o comando recebido na próxima posição
        }
    }
}

void processa_comando_array() { // percorre o array executando os comandos recebidos
    for (int i = 0; i < Posicao_no_array; i++) {
        char comando = array_de_comando[i];
        
        switch (comando) {
          //COMANDOS DOS LEDS
            case 'g':
                ledgaragemState = !ledgaragemState;
                analogWrite(LEDGARAGEM, ledgaragemState ? 255 : 0);
                break;
            case 'b':
   ledsalaState = !ledsalaState;
                analogWrite(LEDSALA, ledsalaState ? 255 : 0);
                break;
            case 'c':
                ledcozinhaState = !ledcozinhaState;
                analogWrite(LEDCOZINHA, ledcozinhaState ? 255 : 0);
                break;
            case 'q':
                ledquartoState = !ledquartoState;
                analogWrite(LEDQUARTO, ledquartoState ? 255 : 0);
                break;
            case 's':
                ledbanheiroState = !ledbanheiroState;
                analogWrite(LEDBANHEIRO, ledbanheiroState ? 255 : 0);
                break;
                //COMANDOS DO PORTAO
            case 'A':
                abrir_portao();
                break;
            case 'F':
                fechar_portao();
                break;
                //COMANDOS DO ALARME
            case 'L':
                sistemaAtivo = true;
                break;
            case 'D':
                sistemaAtivo = false;
                desligaAlarme();
                break;
        }
    }
    // Limpa o array após processamento
    Posicao_no_array = 0;
}

void verificaSensor() { //verifica se o sensor está ativo
    if (digitalRead(pino_sensor) == LOW && !alarmeAtivo) {
        alarmeAtivo = true;
        inicioAlarme = millis();
        digitalWrite(pino_led_alarme, HIGH);
    }
    
    if (alarmeAtivo && (millis() - inicioAlarme >= 10000)) {
        alarmeAtivo = false;
        desligaAlarme();
    }
}

void desligaAlarme() { // desliga o alarme
    alarmeAtivo = false;
    digitalWrite(pino_buzzer, LOW);
    digitalWrite(pino_led_alarme, LOW);
    estadoBuzzer = false;
}

void tocaBuzzerIntermitente() { // faz o buzzer do alarme tocar intermitentemente
    if (alarmeAtivo) {
        unsigned long tempoAtual = millis();
        if (tempoAtual - ultimaTrocaBuzzer >= intervaloBuzzer) { //se o tempo for menor que 10 segundos e o sistema de alarme estiver ativo alterna-se o estado do buzzer a cada 500ms
            estadoBuzzer = !estadoBuzzer;
            digitalWrite(pino_buzzer, estadoBuzzer);
            ultimaTrocaBuzzer = tempoAtual;
        }
    } else {
        digitalWrite(pino_buzzer, LOW);
    }
}

void moverServoComMillis() { // controla o movimento do servo utilizando millis
    if (servoMovendo) {
        if (millis() - tempoServoInicio >= 10) { // o contador conta 10 ms 
            if (anguloAtual < anguloAlvo) { // vê se o angulo atual é menor que o angulo alvo
                anguloAtual++; // anda de um em um a cada 10 ms até chegar no angulo pretendido
            } else if (anguloAtual > anguloAlvo) {// vê se o angulo atual é maior que o angulo alvo
                anguloAtual--; // anda de um em um a cada 10 ms até chegar no angulo pretendido
            } else {
                servoMovendo = false; // Movimento concluído
            }
            myServo.write(anguloAtual);
            tempoServoInicio = millis();
        }
    }
}

void abrir_portao() {
    anguloAlvo = 120;
    servoMovendo = true; // em movimento
}

void fechar_portao() {
    anguloAlvo = 178;
    servoMovendo = true; // em movimento
}
