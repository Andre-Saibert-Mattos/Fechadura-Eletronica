#include <ArduinoJson.h>                  // Biblioteca usada para manipulação de dados em formato Json

#define CONFIG_ESP_INT_WDT_TIMEOUT_MS 600
/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
#include <WiFiClientSecure.h>
#define ssid "XXXXX"     // Nome da sua rede WIFI
#define password "XXXXX" // Senha da sua rede WIFI

bool WifiConectado = false;

// Inicia Certificado
const char* test_root_ca= \
     "-----BEGIN CERTIFICATE-----\n" \
     "MIIDSjCCAjKgAwIBAgIQRK+wgNajJ7qJMDmGLvhAazANBgkqhkiG9w0BAQUFADA/\n" \
     "MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n" \
     "DkRTVCBSb290IENBIFgzMB4XDTAwMDkzMDIxMTIxOVoXDTIxMDkzMDE0MDExNVow\n" \
     "PzEkMCIGA1UEChMbRGlnaXRhbCBTaWduYXR1cmUgVHJ1c3QgQ28uMRcwFQYDVQQD\n" \
     "Ew5EU1QgUm9vdCBDQSBYMzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n" \
     "AN+v6ZdQCINXtMxiZfaQguzH0yxrMMpb7NnDfcdAwRgUi+DoM3ZJKuM/IUmTrE4O\n" \
     "rz5Iy2Xu/NMhD2XSKtkyj4zl93ewEnu1lcCJo6m67XMuegwGMoOifooUMM0RoOEq\n" \
     "OLl5CjH9UL2AZd+3UWODyOKIYepLYYHsUmu5ouJLGiifSKOeDNoJjj4XLh7dIN9b\n" \
     "xiqKqy69cK3FCxolkHRyxXtqqzTWMIn/5WgTe1QLyNau7Fqckh49ZLOMxt+/yUFw\n" \
     "7BZy1SbsOFU5Q9D8/RhcQPGX69Wam40dutolucbY38EVAjqr2m7xPi71XAicPNaD\n" \
     "aeQQmxkqtilX4+U9m5/wAl0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNV\n" \
     "HQ8BAf8EBAMCAQYwHQYDVR0OBBYEFMSnsaR7LHH62+FLkHX/xBVghYkQMA0GCSqG\n" \
     "SIb3DQEBBQUAA4IBAQCjGiybFwBcqR7uKGY3Or+Dxz9LwwmglSBd49lZRNI+DT69\n" \
     "ikugdB/OEIKcdBodfpga3csTS7MgROSR6cz8faXbauX+5v3gTt23ADq1cEmv8uXr\n" \
     "AvHRAosZy5Q6XkjEGB5YGV8eAlrwDPGxrancWYaLbumR9YbK+rlmM6pZW87ipxZz\n" \
     "R8srzJmwN0jP41ZL9c8PDHIyh8bwRLtTcm1D9SZImlJnt1ir/md2cXjbDaJWFBM5\n" \
     "JDGFoqgCWjBH4d1QB7wCCZAA62RjYJsWvIjJEubSfZGL+T0yjWW06XyxV3bqxbYo\n" \
     "Ob8VZRzI9neWagqNdwvYkQsEjgfbKbYK7p2CNTUQ\n" \
     "-----END CERTIFICATE-----\n";

WiFiClientSecure client;  // Cria um cliente seguro para a conexão Web

/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
#include <LiquidCrystal_I2C.h>    // Bibliioteca para o uso do LCD com I2C
LiquidCrystal_I2C lcd(0x27,20,4); // Defina o endereço LCD para 0x27 para um display de 16 caracteres e 2 linhas

uint8_t setaCima[8] =  {0b00000, 0b00100, 0b01110, 0b11111, 0b00000, 0b00000, 0b00000, 0b00000};
uint8_t setaBaixo[8] = {0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b01110, 0b00100, 0b00000};
uint8_t aComTio[8] = {0b01001, 0b10110, 0b00000, 0b01110, 0b00001, 0b01111, 0b10001, 0b01111};

/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
#include <Wire.h>           // Biblioteca usado para a comunicação com a memoria EEPROM
#define disk 0x51           // Endereço do chip 24C16 EEPROM

/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
#include <Servo.h>
#define pin_Servo 15
Servo servo;

/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
// Pinos LEDs sinalização
#define led_Verde 2
#define led_Vermelho 4

/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
// Pinos das linha, ESP32 pull up interno GPIOs: 14, 16, 17, 18, 19, 21, 22 e 23.
#define L1 14 // GPIO 13
#define L2 16 // GPIO 12
#define L3 17 // GPIO 11
#define L4 18 // GPIO 10
#define L5 19 // GPIO 9
#define L6 23 // GPIO 8

// Pinos das colunas.
#define C1 32 // GPIO 3
#define C2 33 // GPIO 4
#define C3 25 // GPIO 5
#define C4 26 // GPIO 6
#define C5 27 // GPIO 7

/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
// Objeto usado para manipular o banco de dados, leitura e escrita.
struct EstruturaBancoDados {  // 8byte
  uint16_t ID;    // ID unico para cada usuario, constituido por 2 bytes de inteiro sem sinal, valores possiveis de 0 a 65535.
  char Senha[4];  // Senha pode existir iguais para diferentes usuarios, constituida por 4 caracteres, XXXX.
  char Tipo;      // Indica se o ID é de um Usuario ou Administrador, "U" ou "A".
};
EstruturaBancoDados DadosBanco;   // Variavel que recebe o objeto do banco de dados.
EstruturaBancoDados structVazia;  // Usado para limpar o DadosBanco.
EstruturaBancoDados Velho;

// Ajustes de entrada de dados
int maxCaracterID = 5;    // Ajuste de quantos caracteres sera composto o ID.
int maxCaracterSenha = 4; // Ajuste de quantos caracteres sera composto a Senha.

/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
// Declaração de funções
String MatrizTeclado(int linha, int coluna);
String teclado();
void primeiraInicializacao();
void redirecionar();
void checarParametros();
String checarCredenciais();
void Escrever_EEPROM(int device, unsigned int cadastro, EstruturaBancoDados dado);
byte Ler_Byte(int device, unsigned int Posicao);
EstruturaBancoDados Ler_EEPROM(int device, EstruturaBancoDados dado, unsigned int cadastro);
void LiberarEntrada();
void menuADM();
void cadastrar();
void apagar();
void editar();
void mudarID();
void mudarSenha();
void mudarTipo();
void conectarWIFI();
void exportarBanco();
void importarBanco();

String txtOpcMenuADM(uint8_t opcao){
  switch(opcao){
    case 1: return "1.Libera Entrada";
    case 2: return "2.Editar";
    case 3: return "3.Cadastrar";
    case 4: return "4.Apagar";
    case 5: return "5.Exporta Banco";
    case 6: return "6.Importar Banco";
    case 7: return "7.Sair";
  }
}

String txtOpcSubMenuEditar(uint8_t opcao){
  switch(opcao){
    case 1: return "1.Mudar ID";
    case 2: return "2.Mudar Senha";
    case 3: return "3.Mudar Tipo";
    case 4: return "4.Sair";
  }
}
/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
char passador = 1;  // Variavel que garante que o codigo analize todas as colunas do teclado.

String btn_precionado="";       // Variavel que recebe o botão precionado no teclado.
String sequenciaCaracteres="";  // Variavel que recebo o conjunto de numes (0-9) que até que seja precionado o botão "ligar".
String ID_Inserido="";          // Variavel que recebe a sequencia de caracteres que compoe o ID inserido.
String Senha_Inserida="";       // Variavel que recebe a sequencia de caracteres que compoe a Senha inserida.
String Tipo_Inserido="";

String Novo_ID="";
String Novo_Senha="";
String Novo_Tipo="";

bool attTexto=true;             // Trava para exibir o texto no LCD 16x2 apenas uma vez.

/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
// --- Interrupção Timer2 ---
void interrupcao_timer(){
  btn_precionado = teclado();
}

/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
void setup(){
  Serial.begin(115200);
  Wire.begin();
  servo.attach(pin_Servo);
  servo.write(0);
  
  lcd.init();         // Inicia o LCD 
  lcd.backlight();    // Liga a Luz de fundo
  lcd.createChar(0, setaCima);
  lcd.createChar(1, setaBaixo);
  lcd.createChar(2, aComTio);
  lcd.home();
  
  pinMode(led_Verde, OUTPUT); digitalWrite(led_Verde, LOW);
  pinMode(led_Vermelho, OUTPUT); digitalWrite(led_Vermelho, LOW);
  
  pinMode(C1, OUTPUT); digitalWrite(C1, HIGH);
  pinMode(C2, OUTPUT); digitalWrite(C2, HIGH);
  pinMode(C3, OUTPUT); digitalWrite(C3, HIGH);
  pinMode(C4, OUTPUT); digitalWrite(C4, HIGH);
  pinMode(C5, OUTPUT); digitalWrite(C5, HIGH);
  pinMode(L1, INPUT_PULLUP);
  pinMode(L2, INPUT_PULLUP);
  pinMode(L3, INPUT_PULLUP);
  pinMode(L4, INPUT_PULLUP);
  pinMode(L5, INPUT_PULLUP);
  pinMode(L6, INPUT_PULLUP);

  // Configurando o Timer2
  hw_timer_t * timer = NULL;        /* Ponteiro usado para a configuração o cronômetro do Timer2    */
  timer = timerBegin(2, 80, true);  /* Inicializando o cronômetro, timerBegin(X, Y, Z)
                                     * X: indica o numero do temporizador que sera usado (de 0 a 3).
                                     * Y: valor do prescaler, sabendo que a frequencia do sinal de base usado pelo ESP32 é de 80MHz, iremos definir um prescaler de 80, para que obtenhamos um sinal com uma frequência de 1MHz que incrementará o contador do temporizador 1000000 vezes por segundo.
                                     * Z: bandeira indicando se o contador deve aumentar (true) ou regressar (false).
                                     */
  timerAttachInterrupt(timer, &interrupcao_timer, true);  /* Vincula ao timer uma função de manipulação, que será executada quando a interrupção for gerada
                                                           * timerAttachInterrupt(X, &Y, Z)
                                                           * X: ponteiro para o temporizador iniciado
                                                           * Y: endereço da função que sera executaa quando houver a interrupção
                                                           * Z: bandeira indicando se a interrupção a ser gerada é borda (true) ou nível (false)
                                                           */
  timerAlarmWrite(timer, 80000, true);  /* Especifica o valor do contador no qual a interrupção do timer será gerada.
                                         * timerAlarmWrite(X, Y, Z)
                                         * X: ponteiro para o temporizador iniciado
                                         * Y: o valor do contador em que a interrupção deve ser gerada, como definimos o prescaler para 80, teremos o contador em microsegundos, ou seja 1000000us = 1s
                                         * Z: bandeira indicando se o temporizador deve recarregar automaticamente ao gerar a interrupção.
                                         */
  timerAlarmEnable(timer);  /* Habilitamos o Timer que acabamos de configurar  */
  
  delay(1000);

  primeiraInicializacao();
}

/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
void loop(){
  checarParametros();
  
  // Alterna o texto no LCD entre qual o seu ID e Qual a sua senha.
  if((ID_Inserido=="") && (attTexto)){
    lcd.clear();
    lcd.setCursor(0,0);  
    lcd.print("Insira o ID:");
    attTexto=false;
  }else if((Senha_Inserida=="") && (attTexto)){
    lcd.clear();
    lcd.setCursor(0,0);  
    lcd.print("Insira a Senha:");
    attTexto=false;
  }
  
  if((ID_Inserido!="") && (Senha_Inserida!=""))
    checarCredenciais();
  
  // Ao ser precionado qualquer tecla entra neste if.
  if(btn_precionado!=""){
    if((btn_precionado=="ligar") || (btn_precionado=="cancelar") || (btn_precionado=="0") || (btn_precionado=="1") || (btn_precionado=="2") || (btn_precionado=="3") || (btn_precionado=="4") || (btn_precionado=="5") || (btn_precionado=="6") || (btn_precionado=="7") || (btn_precionado=="8") || (btn_precionado=="9"))
      redirecionar();
  }
}

/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
void Escrever_EEPROM(int device, unsigned int cadastro, EstruturaBancoDados dado) {
  Wire.beginTransmission(device);
  Wire.write(sizeof(dado)*cadastro);
  Wire.write((uint8_t *) &dado, sizeof(dado));
  Wire.endTransmission();
  
  delay(5);
}

/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
byte Ler_Byte(int device, unsigned int Posicao){
  byte Dado_Leitura_EEPROM = 0x00;
  Wire.beginTransmission(device);
  Wire.write(Posicao);
  Wire.endTransmission(); 
  Wire.requestFrom(device, 1);
  if (Wire.available())
    Dado_Leitura_EEPROM = Wire.read();
    
  return Dado_Leitura_EEPROM;
}

/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
EstruturaBancoDados Ler_EEPROM(int device, EstruturaBancoDados dado, unsigned int cadastro) {
  byte aux[sizeof(dado.ID)];  // Variavel axiliar para captação do ID.
  String somByte="";          // Variavel axiliar para captação do ID.
  bool entra = true;          // Bandeira para executar a codificação do ID.

  // Loop que percorre a memoria.
  for(int i=0; i < sizeof(dado); i++){
    // IF define os parametros para cada tipo de dado (ID, Senha, Tipo).
    if(i < sizeof(dado.ID))
      aux[(sizeof(dado.ID)-1)-i] = Ler_Byte(device, (i+(sizeof(dado)*cadastro)));             // Captura os bytes do ID invertendo eles dentro de uma Array, de traz para frente.
    else if(((i-sizeof(dado.ID)) < sizeof(dado.Senha)) && (i >= sizeof(dado.ID)))
      dado.Senha[(i-sizeof(dado.ID))] = (char)Ler_Byte(device, (i+(sizeof(dado)*cadastro))); // Captura os bytes da senha inserindo um a um na Array senha do obsejo.
    else if(((i-sizeof(dado.ID)-sizeof(dado.Senha)) < sizeof(dado.Tipo)) && (i >= (sizeof(dado.ID)+sizeof(dado.Senha))))
      dado.Tipo = (char)Ler_Byte(device, (i+(sizeof(dado)*cadastro)));                       // Captura o byte tipo e insere na variavel char.

    // IF para a codificação do ID.
    if((i > sizeof(dado.ID)) && (entra)){
      for(int j=0; j < sizeof(dado.ID); j++){          // Loop para a transposição dos bytes de dentro da Array para uma String, concatenando-os e transformando em HEX.
        if(String(aux[j], HEX).length()<2)
          somByte+="0";
        somByte += String(aux[j], HEX);
      }
      char aux2[somByte.length()];                    // Variavel axiliar para codificação do ID.
      somByte.toCharArray(aux2, somByte.length()+1);  // Passa caracter a caracter da String para cada uma das posições da Array auxiliar.
      dado.ID = strtoul(aux2, 0, 16);                // Converte o texto que esta dentro da Array de HEX para DEC.
      entra = false;
    }
  }
  return dado;
}

/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
String checarCredenciais(){
  uint16_t cont=0;
  do{
    DadosBanco = structVazia;
    DadosBanco = Ler_EEPROM(disk, DadosBanco, (cont/sizeof(DadosBanco)));
    cont += sizeof(DadosBanco);
  }while(((cont-sizeof(DadosBanco)) < 255) && (DadosBanco.ID != ID_Inserido.toInt()) && DadosBanco.Tipo);
  
  if((cont >= 255) || !DadosBanco.Tipo){
    lcd.clear();
    lcd.setCursor(0,0);  
    lcd.print("ID n");
    lcd.setCursor(4,0);
    lcd.write((byte)2);
    lcd.setCursor(5,0);
    lcd.print("o existe");
    digitalWrite(led_Vermelho, HIGH);
    delay(3000);
    ID_Inserido="";
    Senha_Inserida="";
  }else if(DadosBanco.ID == ID_Inserido.toInt()){
    String auxSenha="";
    for(int i=0; i<sizeof(DadosBanco.Senha); i++){auxSenha += DadosBanco.Senha[i];}
    if(auxSenha != Senha_Inserida){
      lcd.clear();
      lcd.setCursor(0,0);  
      lcd.print("Senha Incorreta");
      digitalWrite(led_Vermelho, HIGH);
      delay(3000);
      Senha_Inserida="";
    }else{
      if(DadosBanco.Tipo == '0'){
        LiberarEntrada();
      }else if(DadosBanco.Tipo == '1'){
        menuADM();
      }
    }
  }
  digitalWrite(led_Vermelho, LOW);
  DadosBanco = structVazia;
}

/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
void LiberarEntrada(){
  lcd.clear();
  lcd.setCursor(0,0);  
  lcd.print("** Bem Vindo **");
  digitalWrite(led_Verde, HIGH);
  servo.write(90);
  delay(1000);
  servo.write(0);
  delay(1000);
  digitalWrite(led_Verde, LOW);
  ID_Inserido="";
  Senha_Inserida="";
}

/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
void cadastrar(){
  lcd.setCursor(0,0);
  lcd.print("                "); 
  lcd.setCursor(0,1);  
  lcd.print("                ");
  ID_Inserido="";
  Senha_Inserida="";
  attTexto=true;
  String Tipo_Digitado="";
  bool checkID=true;
  bool auxCheck=true;
  uint8_t posicaoVazia=0;
  uint8_t posicaoApagada=0;
  
  while(Tipo_Inserido==""){
    if((ID_Inserido=="") && (attTexto)){
      lcd.clear();
      lcd.setCursor(0,0);  
      lcd.print("Insira Um ID:");
      attTexto=false;
    }else if((Senha_Inserida=="") && (attTexto)){
      lcd.clear();
      lcd.setCursor(0,0);  
      lcd.print("Insira a Senha:");
      attTexto=false;
    }else if((Tipo_Inserido=="") && (attTexto)){
      lcd.clear();
      lcd.setCursor(0,0);  
      lcd.print("Insira o Tipo:");
      attTexto=false;
    }
    
    // Ao ser precionado qualquer tecla entra neste if.
    if(btn_precionado!=""){
      if((ID_Inserido=="") || (Senha_Inserida=="")){
        if(ID_Inserido=="")
          auxCheck=true;
        redirecionar();
        if((ID_Inserido!="") && (Senha_Inserida=="") && auxCheck)
          checkID=true;
      }else if((ID_Inserido!="") && (Senha_Inserida!="") && (Tipo_Inserido=="")){
        if(btn_precionado=="ligar"){
          if((Tipo_Inserido=="") && (!attTexto))
            Tipo_Inserido = Tipo_Digitado;
          lcd.setCursor(0,1);  
          lcd.print("                ");
          Tipo_Digitado="";
          attTexto=true;
        }else if(btn_precionado=="cancelar"){
          Tipo_Digitado="";
          lcd.setCursor(0,1);  
          lcd.print("                ");
        }else if((btn_precionado=="0") || (btn_precionado=="1") || (btn_precionado=="2") || (btn_precionado=="3") || (btn_precionado=="4") || (btn_precionado=="5") || (btn_precionado=="6") || (btn_precionado=="7") || (btn_precionado=="8") || (btn_precionado=="9")){
          if(Tipo_Digitado.length() < sizeof(DadosBanco.Tipo))
            Tipo_Digitado = btn_precionado;

          lcd.setCursor(0,1);  
          lcd.print(Tipo_Digitado);
        }
      }
      btn_precionado = "";
    }
    checarParametros();

    // Checa se o ID existe na memoria
    if((ID_Inserido!="") && (Senha_Inserida=="") && checkID){
      bool aux=true;
      int i=0;
      do{
        DadosBanco = Ler_EEPROM(disk, DadosBanco, (i/sizeof(DadosBanco)));
        if(ID_Inserido.toInt() == DadosBanco.ID){
          lcd.clear();
          lcd.setCursor(0,0);  
          lcd.print("ID ");
          lcd.setCursor(3,0);  
          lcd.print(ID_Inserido);
          lcd.setCursor(0,1); 
          lcd.print("ja cadastrado");

          ID_Inserido="";
          delay(3000);
        }
        if(!DadosBanco.Tipo){
          aux=false;
        }else if((DadosBanco.Tipo == 'x') && (posicaoApagada==0)){
          posicaoApagada=i;
        }
        i+=sizeof(DadosBanco);
      }while((i<255) && aux && (ID_Inserido!=""));
      if(ID_Inserido!="")
        auxCheck=false;
      if(((i-sizeof(DadosBanco))<255) && (!aux)){
        posicaoVazia=(i-sizeof(DadosBanco));
      }
      
      checkID=false;
      attTexto=true;
    }
  }
  
  if((posicaoVazia==0) && (posicaoApagada==0)){
    lcd.clear();
    lcd.setCursor(0,0);  
    lcd.print("*Memoria Cheia!*");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0,0);  
    lcd.print("Apague 1 User P/");
    lcd.setCursor(0,1);  
    lcd.print("Cadastrar 1 Novo");
    delay(3000);
  }else{
    DadosBanco.ID = ID_Inserido.toInt();
    Senha_Inserida.toCharArray(DadosBanco.Senha, Senha_Inserida.length()+1);
    DadosBanco.Tipo = Tipo_Inserido.charAt(0);
      
    if(posicaoApagada!=0)
      Escrever_EEPROM(disk, (posicaoApagada/sizeof(DadosBanco)), DadosBanco);
    else if(posicaoVazia!=0)
      Escrever_EEPROM(disk, (posicaoVazia/sizeof(DadosBanco)), DadosBanco);
    
    lcd.clear();
    lcd.setCursor(0,0);  
    lcd.print("*Cadastro Feito*");
    lcd.setCursor(0,1); 
    lcd.print("* Com Sucesso  *");
    delay(3000);
  }

  DadosBanco = structVazia;   // Clear no objeto DadosBanco
  ID_Inserido="";
  Senha_Inserida="";
  Tipo_Inserido="";
}

/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
void apagar(){
  lcd.setCursor(0,0);
  lcd.print("                "); 
  lcd.setCursor(0,1);  
  lcd.print("                ");
  ID_Inserido="";
  btn_precionado="";
  attTexto=true;
  do{
    if(attTexto){
      lcd.clear();
      lcd.setCursor(0,0);  
      lcd.print("Insira um ID:");
      attTexto=false;
    }
    // Ao ser precionado qualquer tecla entra neste if.
    if(btn_precionado!="")
      redirecionar();

    checarParametros();
  }while(ID_Inserido=="");

  int i = 0;
  bool aux = true;
  bool confirmar=false;
  do{
    DadosBanco=structVazia;
    DadosBanco = Ler_EEPROM(disk, DadosBanco, (i/sizeof(DadosBanco)));
    if(ID_Inserido.toInt() == DadosBanco.ID){
      attTexto=true;
      while(!confirmar){
        if(attTexto){
          lcd.clear();
          lcd.setCursor(0,0);  
          lcd.print("Deseja realmente");
          lcd.setCursor(0,1);  
          lcd.print("apagar o User?");
          attTexto=false;
        }
        if(btn_precionado=="cancelar")
          confirmar=true;
        else if(btn_precionado=="ligar"){
          DadosBanco.ID=65535;
          DadosBanco.Tipo='x';
          Escrever_EEPROM(disk, (i/sizeof(DadosBanco)), DadosBanco);
          lcd.clear();
          lcd.setCursor(0,0);  
          lcd.print("* User Apagado *");
          lcd.setCursor(0,1); 
          lcd.print("* Com Sucesso  *");
          delay(3000);

          confirmar=true;
        }
      }
    }
    if(!DadosBanco.Tipo){
      aux=false;
    }
    i+=sizeof(DadosBanco);
  }while((i<255) && aux && (!confirmar));

  if((i>255) || (!aux)){
    lcd.clear();
    lcd.setCursor(0,0);  
    lcd.print("ID ");
    lcd.setCursor(3,0);  
    lcd.print(ID_Inserido);
    lcd.setCursor(0,1); 
    lcd.print("N");
    lcd.setCursor(1,0);
    lcd.write((byte)2);
    lcd.setCursor(2,0);
    lcd.print("o Encontrado");
    ID_Inserido="";
    delay(3000);
  }
  btn_precionado="";
  DadosBanco=structVazia;
  ID_Inserido="";
  attTexto=true;
}

/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
void mudarTipo(){
  lcd.setCursor(0,0);
  lcd.print("                "); 
  lcd.setCursor(0,1);  
  lcd.print("                ");
  String Tipo_Digitado="";
  attTexto=true;
  Tipo_Inserido="";
  while(Tipo_Inserido==""){
    if(attTexto){
      lcd.clear();
      lcd.setCursor(0,0);  
      lcd.print("Insira Nv Tipo:");
      attTexto=false;
    }
    if(btn_precionado=="ligar"){
      if((Tipo_Inserido=="") && (!attTexto))
        Tipo_Inserido = Tipo_Digitado;
      lcd.setCursor(0,1);  
      lcd.print("                ");
      Tipo_Digitado="";
      attTexto=true;
      checarParametros();
      btn_precionado = "";
    }else if(btn_precionado=="cancelar"){
      Tipo_Digitado="";
      lcd.setCursor(0,1);  
      lcd.print("                ");
      btn_precionado = "";
    }else if((btn_precionado=="0") || (btn_precionado=="1") || (btn_precionado=="2") || (btn_precionado=="3") || (btn_precionado=="4") || (btn_precionado=="5") || (btn_precionado=="6") || (btn_precionado=="7") || (btn_precionado=="8") || (btn_precionado=="9")){
      if(Tipo_Digitado.length() < sizeof(DadosBanco.Tipo))
        Tipo_Digitado = btn_precionado;

      lcd.setCursor(0,1);  
      lcd.print(Tipo_Digitado);
      btn_precionado = "";
    }
  }

  lcd.clear();
  lcd.setCursor(0,0);  
  lcd.print("Tipo Atualizado");
  delay(2000);
  
  Novo_Tipo = Tipo_Inserido;
  btn_precionado="";
  Tipo_Digitado="";
  attTexto=true;
}

/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
void mudarSenha(){
  lcd.setCursor(0,0);
  lcd.print("                "); 
  lcd.setCursor(0,1);  
  lcd.print("                ");
  Senha_Inserida="";
  attTexto=true;
  ID_Inserido = 888;
  do{
    if(attTexto){
      lcd.clear();
      lcd.setCursor(0,0);  
      lcd.print("Insira Nv Senha:");
      attTexto=false;
    }
    // Ao ser precionado qualquer tecla entra neste if.
    if(btn_precionado!="")
      redirecionar();
    checarParametros();
  }while(Senha_Inserida=="");
  Novo_Senha = Senha_Inserida;
  lcd.clear();
  lcd.setCursor(0,0);  
  lcd.print("Senha Atualizada");
  delay(2000);
  
  btn_precionado="";
  ID_Inserido = "";
  Senha_Inserida="";
  attTexto=true;
}

/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
void mudarID(){
  lcd.setCursor(0,0);
  lcd.print("                "); 
  lcd.setCursor(0,1);  
  lcd.print("                ");
  ID_Inserido="";
  attTexto=true;
  do{
    if(attTexto){
      lcd.clear();
      lcd.setCursor(0,0);  
      lcd.print("Insira Novo ID:");
      attTexto=false;
    }
    // Ao ser precionado qualquer tecla entra neste if.
    if(btn_precionado!="")
      redirecionar();
    
    checarParametros();

    // Checa se o ID existe na memoria
    if((ID_Inserido!="")){
      bool aux=true;
      int i=0;
      do{
        DadosBanco = Ler_EEPROM(disk, DadosBanco, (i/sizeof(DadosBanco)));
        if(ID_Inserido.toInt() == DadosBanco.ID){
          lcd.clear();
          lcd.setCursor(0,0);  
          lcd.print("ID ");
          lcd.setCursor(3,0);  
          lcd.print(ID_Inserido);
          lcd.setCursor(0,1); 
          lcd.print("ja cadastrado");
  
          ID_Inserido="";
          delay(3000);
        }
        if(!DadosBanco.Tipo){
          aux=false;
        }
        i+=sizeof(DadosBanco);
      }while((i<255) && aux && (ID_Inserido!=""));
      attTexto=true;
    }
  }while(ID_Inserido=="");

  lcd.clear();
  lcd.setCursor(0,0);  
  lcd.print("ID Atualizado");
  delay(2000);
  
  Novo_ID = ID_Inserido;
  btn_precionado="";
  ID_Inserido="";
  attTexto=true;
}

/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
void editar(){
  lcd.setCursor(0,0);
  lcd.print("                "); 
  lcd.setCursor(0,1);  
  lcd.print("                ");
  ID_Inserido="";
  btn_precionado="";
  Tipo_Inserido="";
  attTexto=true;
  uint8_t pTxt = 1;
  
  do{
    if(attTexto){
      lcd.clear();
      lcd.setCursor(0,0);  
      lcd.print("Insira um ID:");
      attTexto=false;
    }
    // Ao ser precionado qualquer tecla entra neste if.
    if(btn_precionado!="")
      redirecionar();
    checarParametros();
  }while(ID_Inserido=="");

  int i = 0;
  bool aux = true;
  bool confirmar=false;
  do{
    DadosBanco=structVazia;
    DadosBanco = Ler_EEPROM(disk, DadosBanco, (i/sizeof(DadosBanco)));
    if(ID_Inserido.toInt() == DadosBanco.ID){
      Velho = DadosBanco;
      attTexto=true;
      if(attTexto){
        lcd.clear();
        lcd.setCursor(0,0);  
        lcd.print("ID encontrado");
        delay(1000);
        attTexto=false;
      }
      attTexto=true;
      while(!confirmar){
        if(attTexto){
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(txtOpcSubMenuEditar(pTxt));
          lcd.setCursor(0,1);
          lcd.print(txtOpcSubMenuEditar(pTxt+1));
          lcd.setCursor(14,1);
          lcd.write((byte)0);
          lcd.setCursor(15,1);
          lcd.write((byte)1);  
          attTexto=false;
        }
        if(btn_precionado!=""){
          if((btn_precionado=="bovina") || (btn_precionado=="frango")){   // "Rolagem" opções do menu
            if((btn_precionado=="frango") && (pTxt<3)){
              pTxt++;
              attTexto=true;
            }else if((btn_precionado=="bovina") && (pTxt>1)){
              pTxt--;
              attTexto=true;
            }
          }else if((btn_precionado=="1") || (btn_precionado=="2") || (btn_precionado=="3") || (btn_precionado=="4")){
            if(btn_precionado=="1"){
              mudarID();
            }else if(btn_precionado=="2"){
              mudarSenha();
            }else if(btn_precionado=="3"){
              mudarTipo();
            }else if(btn_precionado=="4"){
              if(Novo_ID!="")
                Velho.ID = Novo_ID.toInt();
              
              if(Novo_Senha!="")
                Novo_Senha.toCharArray(Velho.Senha, Novo_Senha.length()+1);
              
              if(Novo_Tipo!="")
                Velho.Tipo = Novo_Tipo.charAt(0);
              
              if((Novo_ID!="") || (Novo_Senha!="") || (Novo_Tipo!=""))
                Escrever_EEPROM(disk, (i/sizeof(DadosBanco)), Velho);
              
              confirmar=true;
            }
          }else{
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Entrada Esperada");
            lcd.setCursor(0,1);
            lcd.print("1 a 4, ");
            lcd.setCursor(7,1);
            lcd.write((byte)0);
            lcd.setCursor(8,1);
            lcd.print(" ou ");
            lcd.setCursor(12,1);
            lcd.write((byte)1);
            attTexto=true;
            delay(3000);
          }
          btn_precionado = "";
        }
      }
    }
    if(!DadosBanco.Tipo){
      aux=false;
    }
    i+=sizeof(DadosBanco);
  }while((i<255) && aux && !confirmar);

  if(((i>255) || (!aux)) && (ID_Inserido!="")){
    lcd.clear();
    lcd.setCursor(0,0);  
    lcd.print("ID ");
    lcd.setCursor(3,0);  
    lcd.print(ID_Inserido);
    lcd.setCursor(0,1); 
    lcd.print("N");
    lcd.setCursor(1,0);
    lcd.write((byte)2);
    lcd.setCursor(2,0);
    lcd.print("o Encontrado");
    ID_Inserido="";
    delay(3000);
  }
  btn_precionado="";
  DadosBanco=structVazia;
  ID_Inserido="";
  Senha_Inserida="";
  Tipo_Inserido="";
  
  Novo_ID="";
  Novo_Senha="";
  Novo_Tipo="";
  attTexto=true;
}

/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
void conectarWIFI(){
  // Solicita a conexão com o WIFI
  lcd.setCursor(0,0);
  lcd.print("                ");
  lcd.setCursor(0,1);
  lcd.print("                ");
  lcd.setCursor(0,0);  
  lcd.print("Conectando WIFI");
  delay(500);
  WiFi.begin(ssid, password);

  // Espera até a conexão ser bem sucedida
  uint8_t i=0;
  while (WiFi.status() != WL_CONNECTED) {
    lcd.setCursor(i,1);  
    lcd.print(".");
    i++;
    if(i==16){
      lcd.setCursor(0,1);
      lcd.print("                ");
      i=0;
    }
    delay(1000);
  }
  // Setar para o cliete o certificado
  client.setCACert(test_root_ca);
  delay(500);

  lcd.setCursor(0,0);
  lcd.print("                ");
  lcd.setCursor(0,1);
  lcd.print("                ");
  lcd.setCursor(0,0);  
  lcd.print("*WIFI Conectado*");
  delay (1000);
}

/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
void exportarBanco(){
  conectarWIFI();

  lcd.clear();
  lcd.setCursor(0,0);  
  lcd.print(" Conectando ao");
  lcd.setCursor(0,1); 
  lcd.print("   Servidor");
  delay(500);
  client.setInsecure();
  if (!client.connect("script.google.com", 443)){
    lcd.clear();
    lcd.setCursor(0,0);  
    lcd.print(" ** Falha ao **");
    lcd.setCursor(0,1); 
    lcd.print("  * Conectar *");
  }else {
    lcd.clear();
    lcd.setCursor(0,0);  
    lcd.print(" * Conectado *");
    
    // Constroi o HTTP
    String sendDados = "GET https://script.google.com/macros/s/AKfycbwLWVeYzSRPjNA5Wv9H6Huywa1uSe5ocLjZ-J0fRtULLr7tp_iL/exec?"; 
    bool aux=true;
    int i=0;
    int k=0;
    do{
      DadosBanco = Ler_EEPROM(disk, DadosBanco, (i/sizeof(DadosBanco)));

      if(!DadosBanco.Tipo){
        aux=false;
      }
      
      if((DadosBanco.Tipo != 'x') && aux){
        sendDados += (String)k+"=";
        sendDados += DadosBanco.ID;
        sendDados += "&"+(String)(k+1)+"=";
        for(int i=0; i<sizeof(DadosBanco.Senha); i++)
          sendDados += DadosBanco.Senha[i];
        sendDados += "&"+(String)(k+2)+"=";
        sendDados += DadosBanco.Tipo;
        k+=3;
      }
      i+=sizeof(DadosBanco);
      
      if((i<255) && aux){
        sendDados += "&";
      }
    }while((i<255) && aux);
    
    if(!aux){
      for(int j=i; j<258; (j+=sizeof(DadosBanco))){
        sendDados += (String)k+"=";
        sendDados += "";
        sendDados += "&";
        sendDados += (String)(k+1);
        sendDados += "=";
        sendDados += "";
        sendDados += "&";
        sendDados += (String)(k+2);
        sendDados += "=";
        sendDados += "";
        if((j+sizeof(DadosBanco))<258)
          sendDados += "&";
        k+=3;
      }
    }
    sendDados += " HTTP/1.0";
    
    // Envia o HTTP para o dominio
    client.println(sendDados);
    client.println("Host: script.google.com");
    client.println("Connection: close");
    client.println();
    client.stop();  // Finaliza a conexão com o dominio

    lcd.clear();
    lcd.setCursor(0,0);  
    lcd.print("Dados Enviados!");
    lcd.setCursor(0,1); 
    lcd.print(" *Desconectando*");
  }
  WiFi.disconnect();
  delay(3000);

  DadosBanco=structVazia;
  attTexto=true;
}

/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
void importarBanco(){
  conectarWIFI();

  String IdPlanilha = "1MxFd5vjCzjW4IjmMbt7zDEIIJT3lzt8HqTcRtD0CAhQ";
  String ChaveAPI = "AIzaSyBxFIT3RP0rJFZwNZUug5499ZuWUjc3cHk";

  lcd.clear();
  lcd.setCursor(0,0);  
  lcd.print(" Conectando ao");
  lcd.setCursor(0,1); 
  lcd.print("   Servidor");
  delay(500);
  client.setInsecure();
  if (!client.connect("https://sheets.googleapis.com", 443)){
    lcd.clear();
    lcd.setCursor(0,0);  
    lcd.print(" ** Falha ao **");
    lcd.setCursor(0,1); 
    lcd.print("  * Conectar *");
  }else{
    lcd.clear();
    lcd.setCursor(0,0);  
    lcd.print(" * Conectado *");
    
    // Envia o HTTP para o dominio
    client.println("GET https://sheets.googleapis.com/v4/spreadsheets/"+IdPlanilha+"/values/A2:C33/?key="+ChaveAPI+" HTTP/1.0");
    client.println("Host: sheets.googleapis.com");
    client.println("Connection: close");
    client.println();

    // Le o cabeçalho, e assim que termina sai do loop
    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        Serial.println("cabeçalhos recebido");
        break;
      }
    }

    // Faz a leitura do retorno da API, JSON
    String retorno="";
    while (client.available()) {
      char c = client.read();
      retorno += c;
    }
    client.stop();  // Finaliza a conexão com o dominio

    DynamicJsonDocument doc(1024);
    deserializeJson(doc, retorno);
    JsonObject obj = doc.as<JsonObject>();

    int i=0; 
    do{
      for(int j=0; j<3; j++){
        if(j==0){
          DadosBanco.ID = (int)doc["values"][i][j];
        }else if(j==1){
          String aux = doc["values"][i][j];
          aux.toCharArray(DadosBanco.Senha, aux.length()+1);
        }else if(j==2){
          String aux = doc["values"][i][j];
          DadosBanco.Tipo = aux.charAt(0);
        }
      }
      if(DadosBanco.ID!=NULL)
        Escrever_EEPROM(disk, i,DadosBanco);
      i++;
    }while(DadosBanco.ID!=NULL);

    // Preenche com nulo nos demais bytes da memoria
    byte msgf[8] = {
       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    
    for(int j = ((i-1)*sizeof(DadosBanco)); j <= 255; j+=8){
      byte devaddr = disk | ((j >> 8) & 0x07);
      byte addr    = j;
      Wire.beginTransmission(devaddr);
      Wire.write(int(addr));
      for (int k = 0; k < 8; k++) {
         Wire.write(msgf[k]);
      }
      Wire.endTransmission();
      delay(10);
    }

    lcd.clear();
    lcd.setCursor(0,0);  
    lcd.print("Dados Recebidos!");
    lcd.setCursor(0,1); 
    lcd.print("*Desconectando*");
  }
  
  WiFi.disconnect();
  delay(3000);

  DadosBanco=structVazia;
  attTexto=true;
}

/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
void menuADM(){
  uint8_t pTxt = 1;
  bool sair=false;
  do{
    if(attTexto){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(txtOpcMenuADM(pTxt));
      lcd.setCursor(0,1);
      lcd.print(txtOpcMenuADM(pTxt+1));
      lcd.setCursor(14,1);
      lcd.write((byte)0);
      lcd.setCursor(15,1);
      lcd.write((byte)1);  
      attTexto=false;
    }
    if(btn_precionado!=""){
      if((btn_precionado=="bovina") || (btn_precionado=="frango")){   // "Rolagem" opções do menu
        if((btn_precionado=="frango") && (pTxt<6)){
          pTxt++;
          attTexto=true;
        }else if((btn_precionado=="bovina") && (pTxt>1)){
          pTxt--;
          attTexto=true;
        }
      }else if((btn_precionado=="1") || (btn_precionado=="2") || (btn_precionado=="3") || (btn_precionado=="4") || (btn_precionado=="5") || (btn_precionado=="6") || (btn_precionado=="7") || (btn_precionado=="brigadeiro") || (btn_precionado=="memória")){
        if(btn_precionado=="1"){
          LiberarEntrada();
          sair=true;
        }else if(btn_precionado=="2"){
          editar();
        }else if(btn_precionado=="3"){
          cadastrar();
        }else if(btn_precionado=="4"){
          apagar();
        }else if(btn_precionado=="5"){
          exportarBanco();
        }else if(btn_precionado=="6"){
          importarBanco();
        }else if(btn_precionado=="7"){
          sair=true;
        }else{
          redirecionar();
        }
      }else{
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Entrada Esperada");
        lcd.setCursor(0,1);
        lcd.print("1 a 5, ");
        lcd.setCursor(7,1);
        lcd.write((byte)0);
        lcd.setCursor(8,1);
        lcd.print(" ou ");
        lcd.setCursor(12,1);
        lcd.write((byte)1);
        attTexto=true;
        delay(3000);
      }
      btn_precionado = "";
    }
  }while(sair==false);
  btn_precionado = "";
  sair=false;
  ID_Inserido="";
  Senha_Inserida="";
  attTexto=true;
}

/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
void checarParametros(){
  // Força o usuario a cumprir os parametros de ID e Senha, senho ID variando de 0 a 65534 e senha possuindo 4 caracteres
  if((ID_Inserido != "") && (ID_Inserido.toInt() >= (pow(2, (8*sizeof(DadosBanco.ID))))-1)){
    lcd.clear();
    lcd.setCursor(0,0);  
    lcd.print("O ID deve ser um");
    lcd.setCursor(0,1);  
    lcd.print("valor de 0-");
    lcd.setCursor(11,1);  
    lcd.print((pow(2, (8*sizeof(DadosBanco.ID))))-2);

    ID_Inserido="";
    delay(3000);      // Tempo para exibir a informação ao usuario
  }else if((Senha_Inserida != "") && (Senha_Inserida.length() < maxCaracterSenha)){
    lcd.clear();
    lcd.setCursor(0,0);  
    lcd.print("A Senha deve");
    lcd.setCursor(0,1);  
    lcd.print("conter ");
    lcd.setCursor(7,1);  
    lcd.print(sizeof(DadosBanco.Senha));
    lcd.setCursor(8,1);  
    lcd.print(" digitos");

    Senha_Inserida="";
    delay(3000);      // Tempo para exibir a informação ao usuario
  }else if((Tipo_Inserido != "") && (Tipo_Inserido != "0") && (Tipo_Inserido != "1")){
    lcd.clear();
    lcd.setCursor(0,0);  
    lcd.print("Digite 0 P/ USER");
    lcd.setCursor(0,1);
    lcd.print(" e 1 P/ ADM");
    Tipo_Inserido="";
    delay(3000);      // Tempo para exibir a informação ao usuario
  }
}

/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
void redirecionar(){
  if(btn_precionado=="ligar"){
    // Insere a sequencia de caracteres do ID e da Senha para as suas respectivas variaveis.
    if((ID_Inserido=="") && (!attTexto))
      ID_Inserido = sequenciaCaracteres;
    else if((Senha_Inserida=="") && (!attTexto))
      Senha_Inserida = sequenciaCaracteres;

    lcd.setCursor(0,1);  
    lcd.print("                ");
    
    sequenciaCaracteres="";               // Limpa a variavel que comnta a sequencia de caracteres apos ser capitada.
    attTexto=true;                        // Libera o LCD exiber o novo texto.
  }else if(btn_precionado=="cancelar"){
    sequenciaCaracteres="";
    
    lcd.setCursor(0,1);  
    lcd.print("                ");
  }else if(btn_precionado=="brigadeiro"){    // Apaga a memoria completa
    byte msgf[16] = {
       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    
    for(int i = 15; i <= 255; i+=16){
      byte devaddr = disk | ((i >> 8) & 0x07);
      byte addr    = i;
      Wire.beginTransmission(devaddr);
      Wire.write(int(addr));
      for (int i = 0; i < 16; i++) {
         Wire.write(msgf[i]);
      }
      Wire.endTransmission();
      delay(10);
    }
    lcd.clear();
    lcd.setCursor(0,0);  
    lcd.print("Limpou Memoria");
    delay(2000);
    primeiraInicializacao();
    
  }else if(btn_precionado=="memória"){ // Apresenta os bytes da memora em HEX no Serial Monitor
    for (int i = 0; i <= 255; i++) {
      Serial.print(Ler_Byte(disk, i), HEX);
      Serial.print(' ');
      if((i+1)%16 == 0){
        Serial.println();
      }
    }
    Serial.println();
  }else if((btn_precionado=="0") || (btn_precionado=="1") || (btn_precionado=="2") || (btn_precionado=="3") || (btn_precionado=="4") || (btn_precionado=="5") || (btn_precionado=="6") || (btn_precionado=="7") || (btn_precionado=="8") || (btn_precionado=="9")){
    if((ID_Inserido=="") && (sequenciaCaracteres.length() < maxCaracterID))                                 // Permite que insira apenas o 'maxCaracterID' definido anteriormente para o ID.
      sequenciaCaracteres += btn_precionado;
    else if((ID_Inserido!="") && (Senha_Inserida=="") && (sequenciaCaracteres.length() < maxCaracterSenha)) // Permite que insira apenas o 'maxCaracterSenha' definido anteriormente para a Senha.
      sequenciaCaracteres += btn_precionado;

    lcd.setCursor(0,1);  
    lcd.print(sequenciaCaracteres);
  }
  btn_precionado = "";
}

/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
void primeiraInicializacao(){
  ID_Inserido="";
  Senha_Inserida="";
  attTexto=true;
  DadosBanco = Ler_EEPROM(disk, DadosBanco, 0);
  if(DadosBanco.ID == 0){
    lcd.clear();
    lcd.setCursor(0,0);  
    lcd.print("Bem Vindo!!");
    lcd.setCursor(0,1);  
    lcd.print("Cadastre um ADM!");
    delay(4000);
    
    while(Senha_Inserida==""){
      if((ID_Inserido=="") && (attTexto)){
        lcd.clear();
        lcd.setCursor(0,0);  
        lcd.print("Insira ID do ADM");
        attTexto=false;
      }else if((Senha_Inserida=="") && (attTexto)){
        lcd.clear();
        lcd.setCursor(0,0);  
        lcd.print("Insira a Senha:");
        attTexto=false;
      }
      // Ao ser precionado qualquer tecla entra neste if.
      if(btn_precionado!="")
        redirecionar();
    
      checarParametros();
    }
    DadosBanco.ID = ID_Inserido.toInt();
    Senha_Inserida.toCharArray(DadosBanco.Senha, Senha_Inserida.length()+1);
    DadosBanco.Tipo = '1';

    Escrever_EEPROM(disk, 0, DadosBanco);
    DadosBanco = structVazia;   // Clear no objeto DadosBanco
    ID_Inserido="";
    Senha_Inserida="";
  }
}

/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
String MatrizTeclado(int linha, int coluna){
  String tecla;
  switch(linha){
    case 1:switch(coluna){
             case 1: tecla = "cancelar"; break;
             case 2: tecla = " ";        break;
             case 3: tecla = "relógio";  break;
             case 4: tecla = "memória";  break;
             case 5: tecla = "ligar";    break;
           }break;
    case 2:switch(coluna){
             case 1: tecla = "frango"; break;
             case 2: tecla = " ";      break;
             case 3: tecla = "0";      break;
             case 4: tecla = "7";      break;
             case 5: tecla = "2^";     break;
           }break;
    case 3:switch(coluna){
             case 1: tecla = "brigadeiro"; break;
             case 2: tecla = "carne^";     break;
             case 3: tecla = "9";          break;
             case 4: tecla = "pipoca";     break;
             case 5: tecla = "3^";         break;
           }break;
    case 4:switch(coluna){
             case 1: tecla = "potência";  break;
             case 2: tecla = "macarrão^"; break;
             case 3: tecla = "8";         break;
             case 4: tecla = "4";         break;
             case 5: tecla = "bovina";    break;
           }break;
    case 5:switch(coluna){
             case 1: tecla = "prato";  break;
             case 2: tecla = "arroz^"; break;
             case 3: tecla = "5";      break;
             case 4: tecla = "2";      break;
             case 5: tecla = "1";      break;
           }break;
    case 6:switch(coluna){
             case 1: tecla = "4^";       break;
             case 2: tecla = "legumes^"; break;
             case 3: tecla = "6";        break;
             case 4: tecla = "3";        break;
             case 5: tecla = "1^";       break;
           }break;
  }
  return tecla;
}

/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
String teclado(){
  String tecla;
  if(digitalRead(C1) && passador == 1){
    passador = 2;
    digitalWrite(C1,  LOW); digitalWrite(C2, HIGH); digitalWrite(C3, HIGH); digitalWrite(C4, HIGH); digitalWrite(C5, HIGH);
    
    // -- Testa qual tecla foi pressionada e armazena o valor --
    if     (!digitalRead(L1))  tecla = MatrizTeclado(1,1);
    else if(!digitalRead(L2))  tecla = MatrizTeclado(2,1);
    else if(!digitalRead(L3))  tecla = MatrizTeclado(3,1);
    else if(!digitalRead(L4))  tecla = MatrizTeclado(4,1);
    else if(!digitalRead(L5))  tecla = MatrizTeclado(5,1);
    else if(!digitalRead(L6))  tecla = MatrizTeclado(6,1);
    
  }else if(digitalRead(C2) && passador == 2){
    passador = 3;
    digitalWrite(C1, HIGH); digitalWrite(C2,  LOW); digitalWrite(C3, HIGH); digitalWrite(C4, HIGH); digitalWrite(C5, HIGH);
    
    // -- Testa qual tecla foi pressionada e armazena o valor --
    if     (!digitalRead(L1))  tecla = MatrizTeclado(1,2);
    else if(!digitalRead(L2))  tecla = MatrizTeclado(2,2);
    else if(!digitalRead(L3))  tecla = MatrizTeclado(3,2);
    else if(!digitalRead(L4))  tecla = MatrizTeclado(4,2);
    else if(!digitalRead(L5))  tecla = MatrizTeclado(5,2);
    else if(!digitalRead(L6))  tecla = MatrizTeclado(6,2);
    
  }else if(digitalRead(C3) && passador == 3){
    passador = 4;  
    digitalWrite(C1, HIGH); digitalWrite(C2, HIGH); digitalWrite(C3,  LOW); digitalWrite(C4, HIGH); digitalWrite(C5, HIGH);
    
    // -- Testa qual tecla foi pressionada e armazena o valor --
    if     (!digitalRead(L1))  tecla = MatrizTeclado(1,3);
    else if(!digitalRead(L2))  tecla = MatrizTeclado(2,3);
    else if(!digitalRead(L3))  tecla = MatrizTeclado(3,3);
    else if(!digitalRead(L4))  tecla = MatrizTeclado(4,3);
    else if(!digitalRead(L5))  tecla = MatrizTeclado(5,3);
    else if(!digitalRead(L6))  tecla = MatrizTeclado(6,3);
    
  }else if(digitalRead(C4) && passador == 4){
    passador = 5; 
    digitalWrite(C1, HIGH); digitalWrite(C2, HIGH); digitalWrite(C3, HIGH); digitalWrite(C4,  LOW); digitalWrite(C5, HIGH);
    
    // -- Testa qual tecla foi pressionada e armazena o valor --
    if     (!digitalRead(L1))  tecla = MatrizTeclado(1,4);
    else if(!digitalRead(L2))  tecla = MatrizTeclado(2,4);
    else if(!digitalRead(L3))  tecla = MatrizTeclado(3,4);
    else if(!digitalRead(L4))  tecla = MatrizTeclado(4,4);
    else if(!digitalRead(L5))  tecla = MatrizTeclado(5,4);
    else if(!digitalRead(L6))  tecla = MatrizTeclado(6,4);
    
  }else if(digitalRead(C5) && passador == 5){
    passador = 1;
    digitalWrite(C1, HIGH); digitalWrite(C2, HIGH); digitalWrite(C3, HIGH); digitalWrite(C4, HIGH); digitalWrite(C5, LOW);
    
    // -- Testa qual tecla foi pressionada e armazena o valor --
    if     (!digitalRead(L1))  tecla = MatrizTeclado(1,5);
    else if(!digitalRead(L2))  tecla = MatrizTeclado(2,5);
    else if(!digitalRead(L3))  tecla = MatrizTeclado(3,5);
    else if(!digitalRead(L4))  tecla = MatrizTeclado(4,5);
    else if(!digitalRead(L5))  tecla = MatrizTeclado(5,5);
    else if(!digitalRead(L6))  tecla = MatrizTeclado(6,5);
  }
  return tecla;
}
