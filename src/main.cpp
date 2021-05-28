#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <Arduino.h>
#include <String.h>
#include <iostream>
#include <iomanip>

//Preparando os comandos para escrever as informações no monitor serial
  using std::endl;
  using std::cout;
  using std::setprecision;
  using std::fixed;
  using namespace::std;

//Puxa a classe BLEScan
  BLEScan* pBLEScan;

//Variáveis globais
  string banco; // String para armazenar as informações do dispostivo escaneado
  int a; // Numero do dispositivo encontrado
  int x; // Quantidade de scans feitos

// Tipo de beacon e calculo de distância
  // Cálculo de distância
    int RSSI; // Variável para guardar o RSSI
    double N = 2.00; // Variação do RSSI por metro dependendo do fator climático, padrão é 2
    double Dist; //Variável para guardar a distância dos beacons

  //IBeacon
    string IBeacon = "000000bf"; //Final da info de fabricante dos Ibeacons
    int IBMP = -56; //MP dos Ibeacons
  
  //AltBeacon
    string AltBeacon = "0000bb00"; //Final da info de fabricante dos Altbeacons
    int ABMP = -56; //MP dos AltBeacons

  //JoãoBeacon
    string JoaoBeacon = "0001000100";
    int JBMP = -65;
 

  //Variáveis relacionadas a interação com o usuário
    bool AB; //AltBeacon
    bool IB; //IBeacon
    bool ED; //Eddystone
    bool JB = 1; //João Beacon
    int T = 30; //Tempo entre os scans

//Scan dos dispositivos
  class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks { //Procura por novos dispositivos BLE próximos
      void onResult(BLEAdvertisedDevice advertisedDevice) { //Quando a classe encontra um dispositivo novo, inicia esse loop

        RSSI = advertisedDevice.getRSSI();    //Guarda o valor do RSSI
        cout << setprecision(2);     //Setta a precisão dos doubles para 2 cadas decimais
        banco = advertisedDevice.toString();    // Guarda as informações adiquiridas pela classe dentro de um string usável pelas funções abaixo

        if(AB == 1){    //Caso o usuário tenha escolhido procurar por AB, se inicia a função
          size_t foundAB = banco.find(AltBeacon);     //Procura pela terminação dos AB dentro da string Banco
          if(foundAB != string::npos){    //Se ele tiver encontrado a terminação ele inicia essa função
            a=a+1;     //Guarda o numero referente a ordem da qual o beacon foi encontrado
            Dist = pow(10.0, (( ABMP - RSSI) )/(10 * N));     //Calcula a distância do Beacon para o ESP-32
            cout   << a << ": " << "Distância: " << Dist << "m, "  << banco << ", AltBeacon" << endl;    // printa as informações do Beacon na serial
          }
        }

        if (IB ==1){    //Caso o usuário tenha escolhido procurar por IB, se inicia a função
          size_t foundIB = banco.find(IBeacon);    //Procura pela terminação dos IB dentro da string Banco
          if(foundIB != string::npos){     //Se ele tiver encontrado a terminação ele inicia essa função
            a=a+1;      //Guarda o numero referente a ordem da qual o beacon foi encontrado
            Dist = pow(10.0, (( IBMP - RSSI) )/(10 * N));     //Calcula a distância do Beacon para o ESP-32
            cout   << a << ": " << "Distância: " << Dist << "m, "  << banco << ", IBeacon"  << endl;     // printa as informações do Beacon na serial
          }
        }

        if(JB == 1){    //Caso o usuário tenha escolhido procurar por AB, se inicia a função
          size_t foundJB = banco.find(JoaoBeacon);     //Procura pela terminação dos AB dentro da string Banco
          if(foundJB != string::npos){    //Se ele tiver encontrado a terminação ele inicia essa função
            a=a+1;     //Guarda o numero referente a ordem da qual o beacon foi encontrado
            Dist = pow(10.0, (( JBMP - RSSI) )/(10 * N));     //Calcula a distância do Beacon para o ESP-32
            cout   << a << ": " << "Distância: " << Dist << "m, "  << banco  << ", JoãoBeacon" << endl;    // printa as informações do Beacon na serial
          }
        }
      }
  };

void setup() {
  inicio:   //Guarda o lugar de inicio do código
  int J;    //Variável referente a escolha do usuário
  Serial.begin(115200);   //Inicia a Serial

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); // Cria um novo scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());    //Prepara a Classe MyAdvertisedDeviceCallbacks
  pBLEScan->setActiveScan(true); // Scan ativo gasta mais energia mas é mais preciso (Em testes deixar ligado)
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);

  //Printa a apresentação e escolhas para o usuário, além de recolher informações
    cout 
    << "\n\n\n"
    << "####################################"
    << "# Bem vindo ao scan BLE Wize-Scan! #" 
    << "####################################"
    << "\nDigite o numero referente ao tipo de beacon a ser procurado:"
    << "\n     1 - IBeacon, 2 - AltBeacon, 3- Eddystone" << endl;

    cin >> J;
    delay (10000);

    if (J==1){
      IB=1;
    }
    if (J==2){
      AB=1;
    }
    if (J==3){
      ED=1;
    }

    cout << "\nQuanto tempo (Segundos) entre os scans?" << endl;
    cin >> T;
    delay (10000);

  //Caso o usuário não tenha escolhido uma informação valida o código reinicia
    if(J==0 or T==0){
      cout << endl;
      cout <<"Não foram Fornecidas as informações nescessárias. reiniciando...";
      goto inicio;

    }
}
void loop() {

  //Diz a quantidade de scas feitos:
  x=x+1;
  cout << "\nIniciando scan N: " << x << "\n\n";

  if(IB == 1){
    cout << "Scaneando por IBeacons\n" << endl;
  }

  if(AB == 1){
    cout << "Scaneando por AltBeacons\n" << endl;
  }

  //Inicia os scans

  int scanTime = 1;
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);

    //Texto de finalização
    Serial.printf("\n\nScan finalizado!\n");
    cout << "Tempo entre os scans é de " << T << " Segundos\n\n" << endl;
    pBLEScan->clearResults();   // Apaga os resultados do buffer
    a=0; // Reinicia a contagem dos Beacons
    delay(T * 1000); // Setta o tempo entre os Scans
    
}

