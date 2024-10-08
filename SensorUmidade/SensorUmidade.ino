/*Projeto: medicao local de temperatura e umidade*/
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/*Defines do projeto*/
/*GPIO do modulo WiFi LoRa 32 que o pino de comunicao do sensor esta ligado*/
#define DHTPIN 13 /*(GPIO 13)*/

/*Endereço I2C do display*/
#define OLED_ADDR 0x3c

/*Distancia, em pixels, de cada linha em relacao ao topo do display*/
#define OLED_LINE1 0
#define OLED_LINE2 10
#define OLED_LINE3 20
#define OLED_LINE4 30
#define OLED_LINE5 40
#define OLED_LINE6 50

/*Configuracao da resolucao do display(128x64)*/
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

/*A biblioteca serve para os sensores DHT11, 22 e 21*/
#define DHTTYPE DHT22

/*Variaveis e objetos globais*/
/*Objetos para comunicacao com sensor DHT22*/
DHT dht(DHTPIN, DHTTYPE);

/*Objeto do display*/
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, 16);

/*Variaveis que armazenam os valores maximo e minimo de temperatura registrados.*/
float temperatura_max;
float temperatura_min;

/*Prototypes*/
void atualiza_temperatura_max_e_min(float temp_lida);
void envia_medicoes_para_serial(float temp_lida, float umid_lida);
void escreve_temp_umid_display(float temp_lida, float umid_lida);

/*Implementacoes*/
/*Funcao: Verifica se os valores de temp max e min devem ser atualizados*/
/*Parametros: temp lida - Retorno: nenhum*/
void atualiza_temperatura_max_e_min(float temp_lida){
  if(temp_lida > temperatura_max){
    temperatura_max = temp_lida;
  }
  if(temp_lida < temperatura_min){
    temperatura_min = temp_lida;
  }
}

/*Funcao: envia, na forma de mensagens textuais, as medicoes para a serial*/
/*Parametros: - temp lida*/
/*- Umid relativa do ar lida*/
/*- Max temp registrada*/
/*- Min temp registrada*/
/*Retorno: nenhum*/
void envia_medicoes_para_serial(float temp_lida, float umid_lida){
  char mensagem[200];
  char i;
  
  /*Pula 80 linhas, de forma que no monitor serial seja exibida somente as mensagens
  atuais (impressao de refresh de tela)*/
  for(i=0; i<80; i++)
  Serial.println(" ");

  /*Constroi mensagens e as envia*/
  /*- Temp atual*/
  memset(mensagem,0,sizeof(mensagem));
  sprintf(mensagem,"- Temperatura: %.2fC", temp_lida);
  Serial.println(mensagem);

  /*- Umidade relativa do ar atual*/
  memset(mensagem,0,sizeof(mensagem));
  sprintf(mensagem,"- Umidade atual: %.2f\\%", umid_lida);/*/100*/
  Serial.println(mensagem);

  /*- Temperatura maxima*/
  memset(mensagem,0,sizeof(mensagem));
  sprintf(mensagem,"- Temperatura maxima: %.2fC", temperatura_max);
  Serial.println(mensagem);

  /*- Temperatura minima*/
  memset(mensagem,0,sizeof(mensagem));
  sprintf(mensagem,"- Temperatura minina: %.2fC", temperatura_min);
  Serial.println(mensagem);
}

/*Funcao: escreve no display OLED a temperatura e umidade lidas, 
assim como temperaturas maxima e minima*/
/*Parametros: - Temperatura lida*/
/*- Umidade relativa do ar lida*/
/*Retorno: nenhum*/
void escreve_temp_umid_display(float temp_lida, float umid_lida){
  char str_temp[10] = {0};
  char str_umid[10] = {0};
  char str_temp_max_min[20] = {0};

  /*Formata para o display as strings de temperatura e umidade*/
  sprintf(str_temp, "%.2fC", temp_lida);
  sprintf(str_umid, "%.2f/100", umid_lida);
  sprintf(str_temp_max_min, "%.2fC / %.2fC", temperatura_min, temperatura_max);
  display.clearDisplay();
  display.setCursor(0, OLED_LINE1);
  display.println("Temperatura: ");
  display.setCursor(0, OLED_LINE2);
  display.println(str_temp);
  display.setCursor(0, OLED_LINE3);
  display.println("Umidade: ");
  display.setCursor(0, OLED_LINE4);
  display.println(str_umid);
  display.setCursor(0, OLED_LINE5);
  display.println("Temp min max: ");
  display.setCursor(0, OLED_LINE6);
  display.println(str_temp_max_min);
  display.display();
}

void setup() {
  /*Configura comunicacao serial e inicializa cominicacao com o sensor*/
  Serial.begin(115200);
  dht.begin();
  
  /*inicializa temp max e min com a leitura inicial do sensor*/
  temperatura_max = dht.readTemperature();
  temperatura_min = temperatura_max;

  /*inicializa display OLED*/
  Wire.begin(4, 15);
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)){
    Serial.println("Display OLED: falha ao inicializar");
  }else{
    Serial.println("Display OLED: inicializacao ok");

    /*Limpa display e configura tamanho de fonte*/
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
  }
}

/*Programa principal*/
void loop() {
  float temperatura_lida;
  float umidade_lida;

  /*Faz a leitura de temperatura e umidade do sensor*/
  temperatura_lida = dht.readTemperature();
  umidade_lida = dht.readHumidity();

  /*Se houver falha na leitura do sensor, escreve mensagem de erro no serial*/
  if( isnan(temperatura_lida) || isnan(umidade_lida)){
    Serial.println("Erro ao ler sensor DHT22!");
  }else{

    /*Se a leitura foi bem sucedida, ocorre o seguinte:*/
    /*Os valores min e max sao verificados e comparados a medicao atual de 
    temperatura se a temperatura atual for menor que mini ou maior que a
    max ate entao registrada, os limites max ou min sao atualizados*/
    /*As medicoes(temp, umid, max temp e min temp) sao enviadas pela serial
    na forma de mensagem textual. Tais mensagens podem ser vista no monitor 
    serial*/
    /*As medicoes(temp, umid, max temp e min temp) sao escritas no display OLED*/
    atualiza_temperatura_max_e_min(temperatura_lida);
    envia_medicoes_para_serial(temperatura_lida, umidade_lida);
    escreve_temp_umid_display(temperatura_lida, umidade_lida);
  }

  /*espera 5 seg ate proxima leitura*/
  delay(5000);
}
