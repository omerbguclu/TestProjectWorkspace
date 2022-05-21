//**********************************************************//
//****** SERİAL DEBUG - VERSİYON - İP DEĞİŞİMİ UNUTMA ******//
//**********************************************************//
  String inoCodeVersiyon = "1.00.60";
  String PCBCodeVersiyon = "HC-E3DM-S";
  String MusteriCode    = "AK";

  //#define DEBUG 0

const char* serverName = "http://api.kazankarti.com/app/cihaz.asp";
String host = "api.kazankarti.com"; // Update server
//**********************************************************//
int port = 80;
String SeriNo;
int CihazDurum;

/*
#if DEBUG
  #define PRINT(s)  Serial.print(s); //Serial.println(F(s));
  #define PRINTA(s) Serial.println(); Serial.print(s);
#else
  #define PRINT(s)
  #define PRINTA(s)
#endif
*/

#include <MD5.h>
String apiKey;

TaskHandle_t HandGorevEkran;
TaskHandle_t HandGorevWeb;

#include <WiFi.h>
const char* ssid;
const char* password;
#include <HTTPClient.h>
#include <Update.h>
WiFiClient client;
long contentLength = 0;
bool isValidContentType = false;
int UpdaWifiConTime = 180;
String getHeaderValue(String header, String headerName) {
  return header.substring(strlen(headerName.c_str()));
}

#include <NTPClient.h>
#include <WiFiUdp.h>
  WiFiUDP ntpUDP;
  NTPClient timeClient(ntpUDP, "127.0.0.1", 10800);
int BaslangicZamani, Uptime;
int EkranKapaliPostTime;
int EkranKapaliSiraNo = 0;
int FanKalkisTime = 0;
int TaskWebTime = 0;

#include "BluetoothSerial.h"
String BLveri;
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
BluetoothSerial SerialBT;

#include <ArduinoJson.h>
StaticJsonBuffer<800> jsonBuffer;
String JsYaz, JsGon, JsOku, JsWGon, JsWYaz;
String JsAC, JsON, JsPS, JsFK, JsFH, JsSK, JsTM, JsFA, JsYU, JsSG, JsSC, JsSD, JsHD, JsYA, JsYD, JsYS, JsWA, JsWP, JsURL;

#include <EEPROM.h>
int ePSicaklik, eSirkuKontrol, eSirkuGiris, eSirkuCikis, eSirkuDon, eFanKontrol, eFanHizAyari, eFanHizi;
int eHisterezis, eYakitBitti, eTermik, eUpdate;
String eSSID, ePASS, eUrl;

int UykuValue = 0;
int UykuGirdiMi = 0;
int YakitBittiUyarisi = 0;
int HataSensor = 0;
int HataAsiriSicak = 0;

#include <OneWire.h>
#include <DallasTemperature.h>
const int oneWireBus = 4;
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);
float TempOkunan = 0;
int TempOkuTime = 0;
float temperatureC = 0;
int temperatureE = 0;
int SensorTime = 0; //Sensörü okuma zamanı

#include <RBDdimmer.h>
#define outputPin 2 //MOC PIN
#define zerocross 35 // 220v optocuplor ölçüm ucu pini
dimmerLamp dimmer(outputPin, zerocross);

#define LED_ON    HIGH
#define LED_OFF   LOW

#define SEG_A 12
#define SEG_B 27
#define SEG_C 15
#define SEG_D 16
#define SEG_E 17
#define SEG_F 14
#define SEG_G 13
#define SEG_P 33

#define DIG_1 5
#define DIG_2 18
#define DIG_3 19

String YaziYaz;
String YaziNokta;
char c_digit;

int buzzerPin = 21;
int Termik = 3;
  int TermikValue = 0;
  int TermikTime = 0;

//INPUT
int bOn = 34;
  int bOnValue;//Buton On durumu
  int bOnTime = 0; //Buton On en son basılma zamanı
  unsigned long bOnTimeMilis = 0;
int bArti = 26;
int bEksi = 25;
int bSet = 32;
  int SetModu = 0;
  int SetModuTime = 0; //Set modunda iken en son ne zaman tuşa basıldı, 10sn tuşlama olmaz ise setmodundan çık
int bMan = 0; //33

//OUTPUT
int RFan = 23;
  int RFanValue = 0; // Fan açılışta kapalı
int RPompa = 22;
  int RPompaValue = 0; // Sirkülasyon pompası açılışta kapalı
int WifiValue = 0; //Wifi bağlantı durumu

void execOTA(String UpdateURL){
  //EkranYaz(DilSunucuBaglaUst[eDil],DilSunucuBaglaAlt[eDil]);
  //PRINTA("Connecting to: " + String(host));
  if (client.connect(host.c_str(), port)) {
    //EkranYaz(DilSunucuBagliUst[eDil],DilSunucuBagliAlt[eDil]);
    //PRINTA("Fetching Bin: " + String(UpdateURL));

    client.print(String("GET ") + UpdateURL + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Cache-Control: no-cache\r\n" +
                 "Connection: close\r\n\r\n");

    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        //EkranYaz(DilBagTimeOutUST[eDil],DilBagTimeOutAlt[eDil]);
        //PRINTA("Client Timeout !");
        client.stop();
        return;
      }
    }

    while (client.available()) {
      String line = client.readStringUntil('\n');
      line.trim();

      if (!line.length()) {
        break;
      }

      if (line.startsWith("HTTP/1.1")) {
        if (line.indexOf("200") < 0) {
          //PRINTA("Got a non 200 status code from server. Exiting OTA Update.");
          break;
        }
      }

      if (line.startsWith("Content-Length: ")) {
        contentLength = atol((getHeaderValue(line, "Content-Length: ")).c_str());
        //PRINTA("Got " + String(contentLength) + " bytes from server");
      }

      if (line.startsWith("Content-Type: ")) {
        String contentType = getHeaderValue(line, "Content-Type: ");
        //PRINTA("Got " + contentType + " payload.");
        if (contentType == "application/octet-stream") {
          isValidContentType = true;
        }
      }
    }
  } else {
    //PRINTA("Connection to " + String(host) + " failed. Please check your setup");
  }

  //PRINTA("contentLength : " + String(contentLength) + ", isValidContentType : " + String(isValidContentType));

  if (contentLength && isValidContentType) {
    //EkranYaz(DilGuncindiriUst[eDil],DilGuncindiriAlt[eDil]);
    bool canBegin = Update.begin(contentLength);

    if (canBegin) {
      //PRINTA("Begin OTA. This may take 2 - 5 mins to complete. Things might be quite for a while.. Patience!");
      //EkranYaz(DilGuncYukleUst[eDil],DilGuncYukleALT[eDil]);
      size_t written = Update.writeStream(client);

      if (written == contentLength) {
        //PRINTA("Written : " + String(written) + " successfully");
      } else {
        //PRINTA("Written only : " + String(written) + "/" + String(contentLength) + ". Retry?" );
      }

      if (Update.end()) {
        //PRINTA("OTA done!");
        if (Update.isFinished()) {
          //EkranYaz(DilGuncTamamUst[eDil],DilGuncTamamALT[eDil]);
          delay(2000);
          //PRINTA("Update successfully completed. Rebooting.");
          YenidenBasla();
        } else {
          //EkranYaz(DilGuncHataUst[eDil],DilGuncHataALT[eDil] + "1          ");
          //PRINTA("Update not finished? Something went wrong!");
        delay(2000);
        YenidenBasla();
        }
      } else {
        //EkranYaz(DilGuncHataUst[eDil],DilGuncHataALT[eDil] + "2          ");
        //PRINTA("Error Occurred. Error #: " + String(Update.getError()));
        delay(2000);
        YenidenBasla();
      }
    } else {
      //EkranYaz(DilGuncHataUst[eDil],DilGuncHataALT[eDil] + "3          ");
      //PRINTA("Not enough space to begin OTA");
      delay(2000);
      YenidenBasla(); //client.flush();
    }
  } else {
    //EkranYaz(DilGuncHataUst[eDil],DilGuncHataALT[eDil] + "4          ");
    //PRINTA("There was no content in the response");
    delay(2000);
    YenidenBasla(); //client.flush();
  }
}
void setup(){
  //#if DEBUG
    //Serial.begin(115200);
  //#endif

  SeriNo = WiFi.macAddress();
  SeriNo.replace(":", "");
  String hashKeyGen = SeriNo + PCBCodeVersiyon + MusteriCode; //apikey
  char *hashKEY = &hashKeyGen[0];
  unsigned char* hash=MD5::make_hash(hashKEY);
  char *md5str = MD5::make_digest(hash, 16);
  free(hash);
    apiKey = md5str;
  free(md5str);

  SerialBT.begin("HC-" + SeriNo.substring(8,12));
    //PRINTA("BLUETOOTH OK");

  // Display pinmodu
  pinMode(SEG_A, OUTPUT);
  pinMode(SEG_B, OUTPUT);
  pinMode(SEG_C, OUTPUT);
  pinMode(SEG_D, OUTPUT);
  pinMode(SEG_E, OUTPUT);
  pinMode(SEG_F, OUTPUT);
  pinMode(SEG_G, OUTPUT);
  pinMode(SEG_P, OUTPUT);
      pinMode(DIG_1, OUTPUT);
      pinMode(DIG_2, OUTPUT);
      pinMode(DIG_3, OUTPUT);

  delay(5);

    digitalWrite(SEG_A, LED_OFF);  
    digitalWrite(SEG_B, LED_OFF);  
    digitalWrite(SEG_C, LED_OFF);  
    digitalWrite(SEG_D, LED_OFF);  
    digitalWrite(SEG_E, LED_OFF);  
    digitalWrite(SEG_F, LED_OFF);  
    digitalWrite(SEG_G, LED_ON);
    digitalWrite(SEG_P, LED_OFF);

    digitalWrite(DIG_1, LED_ON);
    digitalWrite(DIG_2, LED_ON);
    digitalWrite(DIG_3, LED_ON);      

  pinMode(buzzerPin, OUTPUT);
    digitalWrite (buzzerPin, LOW);
  pinMode(Termik, INPUT_PULLUP);
  pinMode(bOn, INPUT_PULLUP);
  pinMode(bArti, INPUT_PULLUP);
  pinMode(bEksi, INPUT_PULLUP);
  pinMode(bSet, INPUT_PULLUP);
  pinMode(bMan, INPUT_PULLUP);
    //PRINTA("INPUT OK");

  pinMode(RFan, OUTPUT);
  pinMode(RPompa, OUTPUT);
    //PRINTA("OUTPUT OK");

  EEPROM.begin(512);
    //PRINTA("EEPROM BEGİN OK");

  bOnValue      = EEPROM.read(0);
      if(bOnValue == 1){bOnValue = 1;}else{bOnValue = 0;}
  ePSicaklik    = EEPROM.read(1);
      if(ePSicaklik >= 32 && ePSicaklik <= 80){ePSicaklik = ePSicaklik;}else{ePSicaklik = 45;}
  eSirkuKontrol = EEPROM.read(2);
      if(eSirkuKontrol == 1){eSirkuKontrol = 1;}else{eSirkuKontrol = 0;}
  eSirkuGiris   = EEPROM.read(3);
      if(eSirkuGiris >= 20 && eSirkuGiris <= 65){eSirkuGiris = eSirkuGiris;}else{eSirkuGiris = 34;}
  eSirkuCikis   = EEPROM.read(4);
      if(eSirkuCikis >= 20 && eSirkuCikis <= 60){eSirkuCikis = eSirkuCikis;}else{eSirkuCikis = 30;}
  eSirkuDon     = EEPROM.read(5);
      if(eSirkuDon >= 1 && eSirkuDon <= 10){eSirkuDon = eSirkuDon;}else{eSirkuDon = 5;}
  eFanKontrol   = EEPROM.read(6);
      if(eFanKontrol == 0){eFanKontrol = 0;}else{eFanKontrol = 1;}
  eFanHizAyari  = EEPROM.read(7);
      if(eFanHizAyari == 0){eFanHizAyari = 0;}else{eFanHizAyari = 1;}
  eFanHizi      = EEPROM.read(8);
      if(eFanHizi >= 1 && eFanHizi <= 4){eFanHizi = eFanHizi;}else{eFanHizi = 4;}
  eHisterezis   = EEPROM.read(9);
      if(eHisterezis >= 2 && eHisterezis <= 8){eHisterezis = eHisterezis;}else{eHisterezis = 4;}
  eYakitBitti   = EEPROM.read(10);
      if(eYakitBitti >= 20 && eYakitBitti <= 60){eYakitBitti = eYakitBitti;}else{eYakitBitti = 29;}
  eUpdate       = EEPROM.read(18);
      if(eUpdate == 1){eUpdate = 1;}else{eUpdate = 0;}
  eTermik       = EEPROM.read(19);
      if(eTermik == 1){eTermik = 1;}else{eTermik = 0;}
  eUrl = EEPROM.readString(50);
      eUrl = &eUrl[0];      
  eSSID = EEPROM.readString(100);
      ssid = &eSSID[0];
  ePASS = EEPROM.readString(132);
      password = &ePASS[0];

      if(strlen(ssid) < 3){
        ssid = "KazanKarti";
        password = "kazankarti.com";
      }

  //PRINTA("eUpdate:"); PRINT(eUpdate);
  //PRINTA("eUrl:");    PRINT(eUrl);
      
    //PRINTA("EEPROM READ OK");

    delay(150);

  WiFi.begin(ssid, password); //WİFİ başlat
  WiFi.setHostname("Dijilite");
    //PRINTA("WİFİ BEGİN OK");

    delay(150);

// UPDATE +++
  if(eUpdate == 1){
    digitalWrite(RFan, LOW);
    digitalWrite(RPompa, HIGH);
    EEPROM.write(18, 0);
    EEPROM.commit();

    while (WiFi.status() != WL_CONNECTED){
      //EkranYaz(DilGuncWifiUst[eDil],DilGuncWifiALT[eDil] + String(UpdaWifiConTime));
      //PRINT(".");
        if(UpdaWifiConTime == 0){break;}
        UpdaWifiConTime = UpdaWifiConTime - 1;
      delay(1000);
    }
      if(WiFi.status() == WL_CONNECTED){
          //PRINTA("");
          //PRINTA("Connected to " + String(ssid));
          //PRINTA("Update başlayacak");
          delay(2000);
          //PRINTA("Update başlıyor");
          execOTA(eUrl);
      }else{
          //EkranYaz(DilGuncWifiHatUst[eDil],DilGuncWifiHatALT[eDil]);
          delay(2000);
      }
  }
// UPDATE ---

  timeClient.begin();
  BaslangicZamani = timeClient.getEpochTime();
  FanKalkisTime   = (timeClient.getEpochTime() + 4);
  //PRINTA("TİME BEGİN OK");

    delay(500);

  sensors.begin();
    sensors.requestTemperatures();
      temperatureC = sensors.getTempCByIndex(0);
      temperatureE = temperatureC;

  YaziYaz = " " + String(temperatureE); // Açılışda ekran boş beklemesin diye ısıyı okutup ekrana gönderiyoruz

  dimmer.begin(NORMAL_MODE, ON); //DİMMER BAŞLATILIYOR ÖRNEK: dimmer.setPower(25);

    delay(500);

  xTaskCreatePinnedToCore(TaskEkran,"GorevEkran",10000,NULL,1,&HandGorevEkran,0);
  xTaskCreatePinnedToCore(TaskWeb,"GorevWeb",10000,NULL,1,&HandGorevWeb,1);
/*
  PRINTA("void setup CPU : "); PRINT(xPortGetCoreID());
  PRINTA("---");
  PRINTA("bOnValue        = "); PRINT(bOnValue);
  PRINTA("ePSicaklik      = "); PRINT(ePSicaklik);
  PRINTA("eSirkuKontrol   = "); PRINT(eSirkuKontrol);
  PRINTA("eSirkuGiris     = "); PRINT(eSirkuGiris);
  PRINTA("eSirkuCikis     = "); PRINT(eSirkuCikis);
  PRINTA("eSirkuDon       = "); PRINT(eSirkuDon);
  PRINTA("eFanKontrol     = "); PRINT(eFanKontrol);
  PRINTA("eFanHizAyari    = "); PRINT(eFanHizAyari);
  PRINTA("eFanHizi        = "); PRINT(eFanHizi);
  PRINTA("eHisterezis     = "); PRINT(eHisterezis);
  PRINTA("eYakitBitti     = "); PRINT(eYakitBitti);
  PRINTA("eTermik         = "); PRINT(eTermik);
  PRINTA("---");
  PRINTA("Versiyon        = "); PRINT(inoCodeVersiyon);
  PRINTA("SeriNo          = "); PRINT(SeriNo);
  PRINTA("apikey          = "); PRINT(apiKey);
  PRINTA("SSID            = "); PRINT("'"); PRINT(ssid); PRINT("'");
  PRINTA("PASS            = "); PRINT("'"); PRINT(password); PRINT("'");
  PRINTA("---");
*/
    delay(1250);

  int ResetMilis = millis();
  while(digitalRead(bSet) == 0){
      if (millis() - ResetMilis > 15000){
          //PRINTA("Resetleniyor");
            for (int i = 0; i < 512; i++) { // EEPROM SİLİNİYOR
              EEPROM.write(i, -1);
            }
          EEPROM.commit();
          //PRINTA("EEPROM Silindi");
          digitalWrite (buzzerPin, HIGH); delay (500); digitalWrite (buzzerPin, LOW);
          YenidenBasla();
      }
  }

    delay(250);
}
void loop(){
  Uptime = timeClient.getEpochTime() - BaslangicZamani;
  //PRINT(Uptime);
  //PRINT(" - ");
  //PRINT(temperatureC); PRINT(" ºC ");

  //ISI SENSÖR OKUMA
  if((SensorTime < timeClient.getEpochTime()) && SetModu == 0){
      sensors.requestTemperatures();
      TempOkunan = sensors.getTempCByIndex(0);

        if(TempOkunan == -127.00 && ((timeClient.getEpochTime()-TempOkuTime) > 10)){ //10sn hatalı gelirse sensör arızasına düş
          temperatureC = TempOkunan;
          temperatureE = temperatureC;
          SensorTime = timeClient.getEpochTime();
        }else if(TempOkunan != -127.00){
          TempOkuTime = timeClient.getEpochTime();
          temperatureC = TempOkunan;
          temperatureE = temperatureC;
          SensorTime = timeClient.getEpochTime() + 3;
        }
  }
//ISI SENSÖR OKUMA SONU

  //BUTON ON
  if ((digitalRead(bOn) == 0) && (bOnTime < (timeClient.getEpochTime() - 1))){
    if (millis() - bOnTimeMilis > 50){
      bOnValue = !(bOnValue);
      bOnTime = timeClient.getEpochTime();
          EEPROM.write(0, bOnValue);
          EEPROM.commit();
          Buzzer();
    }
  }else{
    bOnTimeMilis = millis();
  }

  //TERMİK
  if (digitalRead(Termik) == eTermik){
    TermikValue = 1;
    TermikTime = timeClient.getEpochTime();
  }else if(TermikTime > (timeClient.getEpochTime() - 2)){
    TermikValue = 1;
  }else{
    TermikValue = 0;
  }

  //BLUETOOTH al +++
  if (SerialBT.available()){
    BLveri = (SerialBT.readString());
    BluetoothVeri(BLveri);
      //PRINTA("GELEN VERİ= ");
      //PRINTA(BLveri);
  }
  //BLUETOOTH al ---

  // Cihaz açık +++
    if (bOnValue == 1){
        //PRINT("- AÇIK ");

      if((SetModu != 0) && (SetModuTime <= (timeClient.getEpochTime() - 10))){ //tuşlama yapmazsa set modundan çık
        SetModu = 0;
        YaziYaz = "Pr ";
            EEPROM.write(1, ePSicaklik);
            EEPROM.write(8, eFanHizi);
            EEPROM.commit();          
        digitalWrite (buzzerPin, HIGH);
          delay(400);
        digitalWrite (buzzerPin, LOW);          
      }

        if(SetModu == 1){ // Set Modu
          //PRINT("- SET MODU:1 ");
            if(digitalRead(bSet) == 0 && ePSicaklik < 80){
              SetModuTime = timeClient.getEpochTime();
              ePSicaklik = ePSicaklik + 1;
                Buzzer();
                delay(75);
            }else if(digitalRead(bMan) == 0 && ePSicaklik >32){
              SetModuTime = timeClient.getEpochTime();
              ePSicaklik = ePSicaklik - 1;
                Buzzer();
                delay(75);
            }

              if(SetModuTime <= (timeClient.getEpochTime() - 3)){
                if(EkranKapaliPostTime <= timeClient.getEpochTime()){
                  EkranKapaliPostTime = (timeClient.getEpochTime() + 1);
                    if(EkranKapaliSiraNo == 0){
                        YaziYaz = " " + String(ePSicaklik);
                        EkranKapaliSiraNo = (EkranKapaliSiraNo + 1);
                    }else{
                        YaziYaz = "Pr ";
                        EkranKapaliSiraNo = 0;
                    }
                }                
              }else{
                YaziYaz = " " + String(ePSicaklik);
              }

        }else if(SetModu == 2){
          //PRINT("- SET MODU:2 ");
            if(digitalRead(bArti) == 0 && eFanHizi < 4){
              SetModuTime = timeClient.getEpochTime();
              eFanHizi = eFanHizi + 1;
                Buzzer();
                delay(75);
            }else if(digitalRead(bEksi) == 0 && eFanHizi >1){
              SetModuTime = timeClient.getEpochTime();
              eFanHizi = eFanHizi - 1;
                Buzzer();
                delay(75);
            }

              if(SetModuTime <= (timeClient.getEpochTime() - 3)){
                if(EkranKapaliPostTime <= timeClient.getEpochTime()){
                  EkranKapaliPostTime = (timeClient.getEpochTime() + 1);
                    if(EkranKapaliSiraNo == 0){
                        YaziYaz = "Fn" + String(eFanHizi);
                        EkranKapaliSiraNo = (EkranKapaliSiraNo + 1);
                    }else{
                        YaziYaz = "Fn ";
                        EkranKapaliSiraNo = 0;
                    }
                }                
              }else{
                YaziYaz = "Fn" + String(eFanHizi);
              }

        }else{
          if ((digitalRead(bSet) == 0) || (digitalRead(bMan) == 0)){
              SetModu     = 1;
              SetModuTime = timeClient.getEpochTime();
              Buzzer();
                delay(100);
          }
          if (((digitalRead(bArti) == 0) && eFanHizAyari == 1) || ((digitalRead(bEksi) == 0) && eFanHizAyari == 1)){
              SetModu     = 2;
              SetModuTime = timeClient.getEpochTime();
              Buzzer();
                delay(100);
          }

          if(TermikValue == 1){
            YaziYaz = "tEr"; 
          }else{
            if(String(temperatureE).length() == 1){
              YaziYaz = "  " + String(temperatureE);
            }else if(String(temperatureE).length() == 2){
              YaziYaz = " " + String(temperatureE);
            }else if(String(temperatureE).length() == 3){
              YaziYaz = String(temperatureE);
            }
          }
        }

          // Termik kontrol +++
          if (TermikValue == 1){ // termik açık
              //PRINT(" Termik AKTİF ");
              CihazDurum = 3;
                digitalWrite(RFan, LOW);
                RFanValue = 0;
          }else{ // Termik kapalı
              // Açık FAN kontrol +++
              if (eFanKontrol == 0){ //FAN aktif değil
                  //PRINT("RFan OFF ");
                    digitalWrite(RFan, LOW);
                    RFanValue = 0;
              }else{ //FAN otomatik
                    if(temperatureC <= (ePSicaklik+1) && UykuValue == 0){//Kazan sıcaklığı programlanandan küçük ise fan
                      CihazDurum = 1;

                        if(temperatureC <= eYakitBitti && UykuGirdiMi >= 1){ //Sıcaklık yakıt bitti derecesinin altına iner ve daha önce uykuya girmiş ise= yakıt bitti
                            //PRINTA("Yakıt Bitti");
                              YakitBittiUyarisi = 1;
                        }else{
                            if(eFanHizAyari == 1){ //Fan Hız Ayarı aktif mi
                                if(eFanHizi == 1){ //Fan Hızı 1
                                    //PRINT("RFan 1 ON ");
                                      UykuValue = 0;
                                      digitalWrite(RFan, HIGH); // Fanı açıyoruz
                                      RFanValue = 1;
                                        if (FanKalkisTime > (timeClient.getEpochTime() - 10)){ //Fan Kalkış desteği
                                          dimmer.setPower(85);
                                        }else{
                                          dimmer.setPower(35);
                                        }
                                }else if(eFanHizi == 2){ //Fan Hızı 2
                                    //PRINT("RFan 2 ON ");
                                      UykuValue = 0; //UYKU MODU KAPALI
                                      digitalWrite(RFan, HIGH); // Fanı açıyoru
                                      RFanValue = 1;
                                        if (FanKalkisTime > (timeClient.getEpochTime() - 10)){ //Fan Kalkış desteği
                                          dimmer.setPower(85);
                                        }else{
                                          dimmer.setPower(50);
                                        }
                                }else if(eFanHizi == 3){ //Fan Hızı 3
                                    //PRINT("RFan 3 ON ");
                                      UykuValue = 0; //UYKU MODU KAPALI
                                      digitalWrite(RFan, HIGH); // Fanı açıyoru
                                      RFanValue = 1;
                                        if (FanKalkisTime > (timeClient.getEpochTime() - 10)){ //Fan Kalkış desteği
                                          dimmer.setPower(85);
                                        }else{
                                          dimmer.setPower(65);
                                        }
                                }else{ //Fan Hızı tam hız
                                    //PRINT("RFan 4 ON ");
                                      UykuValue = 0; //UYKU MODU KAPALI
                                      digitalWrite(RFan, HIGH); // Fanı açıyoruz
                                      RFanValue = 1;
                                      dimmer.setPower(85);
                                }
                            }else{ //Fan hız ayarı aktif değil, fan tam hız çalışacak
                                //PRINT("RFan 5 ON ");
                                  UykuValue = 0; //UYKU MODU KAPALI
                                  digitalWrite(RFan, HIGH); // Fanı açıyoruz
                                  RFanValue = 1;
                                  dimmer.setPower(85);
                            }
                        }
                    }else if(temperatureC <= (ePSicaklik-eHisterezis) && UykuValue == 1){
                        //PRINTA("Uyku modundan çık");
                          UykuValue = 0; //UYKU MODU KAPALI
                          CihazDurum = 1;
                    }else{
                        //PRINT("RFan OFF ");
                        CihazDurum = 2;
                          UykuValue = 1; //UYKU MODU BAŞLADI
                          UykuGirdiMi = 1;
                          digitalWrite(RFan, LOW); // Fanı kapatıyoruz
                          RFanValue = 0;
                          dimmer.setPower(0);
                          FanKalkisTime = timeClient.getEpochTime(); //Fan kapalı, kalkış destek süresini son zamana eşitliyoruz
                    }
              }
              // Cihaz açık FAN kontrol bölümü sonu ---------------------------------

          }
          // Cihaz açık termik kontrolü sonu ---

// KAPALI BÖLÜMÜ +++
    }else{
      if(EkranKapaliPostTime <= timeClient.getEpochTime()){
        EkranKapaliPostTime = (timeClient.getEpochTime() + 2);
          if(EkranKapaliSiraNo == 0){
              if(String(temperatureE).length() == 1){
                YaziYaz = "  " + String(temperatureE);
              }else if(String(temperatureE).length() == 2){
                YaziYaz = " " + String(temperatureE);
              }else if(String(temperatureE).length() == 3){
                YaziYaz = String(temperatureE);
              }
              EkranKapaliSiraNo = (EkranKapaliSiraNo + 1);
          }else{
              YaziYaz = "oFF";
              EkranKapaliSiraNo = 0;
          }
      }
      CihazDurum = 0;
      //PRINT("KAPALI ");
        digitalWrite(RFan, LOW);
        RFanValue         = 0;
        SetModu           = 0; //Ayarlar ekranında ise kapat
        UykuValue         = 0; //Uyku modu girdi bilgisi siliniyor
        UykuGirdiMi       = 0;
        YakitBittiUyarisi = 0;
        FanKalkisTime     = timeClient.getEpochTime();
    }
// CİHAZ AÇIK KAPALI BÖLÜM SONU ---

// Sirkülasyon +++
if(eSirkuKontrol == 1 && bOnValue == 1){ //Sirkülasyon Kontrol otomatik=0 / açık=1
    RPompaValue = 1;
    digitalWrite(RPompa, HIGH);
      //PRINT(" Pompa 1 ON");
}else{
  if(temperatureC >= eSirkuGiris ){ //Giriş
      RPompaValue = 1;
      digitalWrite(RPompa, HIGH);
        //PRINT(" Pompa 2 ON ");
  }else if (temperatureC >= eSirkuCikis && RPompaValue == 1){ //düşen
      RPompaValue = 1;
      digitalWrite(RPompa, HIGH);
        //PRINT(" Pompa 3 ON ");
  }else if(temperatureC <= eSirkuDon ){ //DON
      RPompaValue = 1;
      digitalWrite(RPompa, HIGH);
        //PRINT(" Pompa 4 ON ");
  }else{ //Çıkış
      RPompaValue = 0;
      digitalWrite(RPompa, LOW);
        //PRINT(" Pompa OFF ");
  }
}
// Sirkülasyon  ---

// HATA BÖLÜMÜ ---
if(temperatureC == -127 && Uptime >= 5){
  HataSensor = 1;
}else if(temperatureC >= 90){
  HataAsiriSicak = 1;  
}

  while(HataSensor == 1  || HataAsiriSicak == 1 || YakitBittiUyarisi == 1){
    sensors.requestTemperatures();
    TempOkunan = sensors.getTempCByIndex(0);
          temperatureC = TempOkunan;
          temperatureE = temperatureC;

    if(HataSensor == 1){
      CihazDurum = 4;
      //PRINTA("HATA: Sensör");
      YaziYaz = "SEn";
          if(TempOkunan != -127.00){
            HataSensor = 0;
          }
    }else if(HataAsiriSicak == 1){
      CihazDurum = 6;
      //PRINTA("HATA: Aşırı Sıcaklık");
      YaziYaz = "AnS";
          if(temperatureE <= 85){
            HataAsiriSicak = 0;
          }
    }else if(YakitBittiUyarisi == 1){
      CihazDurum = 7;
      //PRINTA("HATA: Yakıt Bitti");
      YaziYaz = "ybt";
    }

    YaziNokta = String(RFanValue) + String(RPompaValue) + String(WifiValue);
    
    digitalWrite(RFan, LOW);
      RFanValue = 0;
    digitalWrite(RPompa, HIGH);
      RPompaValue = 1;

        if ((digitalRead(bOn) == 0) || (digitalRead(bArti) == 0) || (digitalRead(bEksi) == 0) || (digitalRead(bSet) == 0) || (digitalRead(bMan) == 0)){YenidenBasla();}
      digitalWrite(buzzerPin, HIGH);
          delay(200);
      digitalWrite(buzzerPin, LOW);
          delay(200);
  }
// HATA BÖLÜMÜ ---

    //PRINTA("");
    YaziNokta = String(RFanValue) + String(RPompaValue) + String(WifiValue);
}
void EkranYaz(char YaziMetni, char YaziNoktaMetni){
        // AnS  : Anormal sıcaklık
        // SEn  : Semsör Arıza
        // ybt  : Yakıt Bitti
        // oFF  : Kapalı
        // Pr   : Program
        // Fn   : Fan Hızı
        // uP   : Update
        // 1. dot : Fan
        // 2. dot : Sirkülasyon
        // 3. dot : Wifi durumu
        // YaziYaz = "ybt"; YaziNokta = "11-";
  switch (YaziMetni){
    case '0': digitalWrite(SEG_A, LED_ON);  digitalWrite(SEG_B, LED_ON);  digitalWrite(SEG_C, LED_ON);  digitalWrite(SEG_D, LED_ON);  digitalWrite(SEG_E, LED_ON);  digitalWrite(SEG_F, LED_ON);  digitalWrite(SEG_G, LED_OFF); break;      
    case '1': digitalWrite(SEG_A, LED_OFF); digitalWrite(SEG_B, LED_ON);  digitalWrite(SEG_C, LED_ON);  digitalWrite(SEG_D, LED_OFF); digitalWrite(SEG_E, LED_OFF); digitalWrite(SEG_F, LED_OFF); digitalWrite(SEG_G, LED_OFF); break;
    case '2': digitalWrite(SEG_A, LED_ON);  digitalWrite(SEG_B, LED_ON);  digitalWrite(SEG_C, LED_OFF); digitalWrite(SEG_D, LED_ON);  digitalWrite(SEG_E, LED_ON);  digitalWrite(SEG_F, LED_OFF); digitalWrite(SEG_G, LED_ON);  break;
    case '3': digitalWrite(SEG_A, LED_ON);  digitalWrite(SEG_B, LED_ON);  digitalWrite(SEG_C, LED_ON);  digitalWrite(SEG_D, LED_ON);  digitalWrite(SEG_E, LED_OFF); digitalWrite(SEG_F, LED_OFF); digitalWrite(SEG_G, LED_ON);  break;
    case '4': digitalWrite(SEG_A, LED_OFF); digitalWrite(SEG_B, LED_ON);  digitalWrite(SEG_C, LED_ON);  digitalWrite(SEG_D, LED_OFF); digitalWrite(SEG_E, LED_OFF); digitalWrite(SEG_F, LED_ON);  digitalWrite(SEG_G, LED_ON);  break;
    case '5': digitalWrite(SEG_A, LED_ON);  digitalWrite(SEG_B, LED_OFF); digitalWrite(SEG_C, LED_ON);  digitalWrite(SEG_D, LED_ON);  digitalWrite(SEG_E, LED_OFF); digitalWrite(SEG_F, LED_ON);  digitalWrite(SEG_G, LED_ON);  break;
    case '6': digitalWrite(SEG_A, LED_ON);  digitalWrite(SEG_B, LED_OFF); digitalWrite(SEG_C, LED_ON);  digitalWrite(SEG_D, LED_ON);  digitalWrite(SEG_E, LED_ON);  digitalWrite(SEG_F, LED_ON);  digitalWrite(SEG_G, LED_ON);  break;
    case '7': digitalWrite(SEG_A, LED_ON);  digitalWrite(SEG_B, LED_ON);  digitalWrite(SEG_C, LED_ON);  digitalWrite(SEG_D, LED_OFF); digitalWrite(SEG_E, LED_OFF); digitalWrite(SEG_F, LED_OFF); digitalWrite(SEG_G, LED_OFF); break;
    case '8': digitalWrite(SEG_A, LED_ON);  digitalWrite(SEG_B, LED_ON);  digitalWrite(SEG_C, LED_ON);  digitalWrite(SEG_D, LED_ON);  digitalWrite(SEG_E, LED_ON);  digitalWrite(SEG_F, LED_ON);  digitalWrite(SEG_G, LED_ON);  break;
    case '9': digitalWrite(SEG_A, LED_ON);  digitalWrite(SEG_B, LED_ON);  digitalWrite(SEG_C, LED_ON);  digitalWrite(SEG_D, LED_ON);  digitalWrite(SEG_E, LED_OFF); digitalWrite(SEG_F, LED_ON);  digitalWrite(SEG_G, LED_ON);  break;
    case 'A': digitalWrite(SEG_A, LED_ON);  digitalWrite(SEG_B, LED_ON);  digitalWrite(SEG_C, LED_ON);  digitalWrite(SEG_D, LED_OFF); digitalWrite(SEG_E, LED_ON);  digitalWrite(SEG_F, LED_ON);  digitalWrite(SEG_G, LED_ON);  break;
    case 'b': digitalWrite(SEG_A, LED_OFF); digitalWrite(SEG_B, LED_OFF); digitalWrite(SEG_C, LED_ON);  digitalWrite(SEG_D, LED_ON);  digitalWrite(SEG_E, LED_ON);  digitalWrite(SEG_F, LED_ON);  digitalWrite(SEG_G, LED_ON);  break;
    case 'E': digitalWrite(SEG_A, LED_ON);  digitalWrite(SEG_B, LED_OFF); digitalWrite(SEG_C, LED_OFF); digitalWrite(SEG_D, LED_ON);  digitalWrite(SEG_E, LED_ON);  digitalWrite(SEG_F, LED_ON);  digitalWrite(SEG_G, LED_ON);  break;
    case 'F': digitalWrite(SEG_A, LED_ON);  digitalWrite(SEG_B, LED_OFF); digitalWrite(SEG_C, LED_OFF); digitalWrite(SEG_D, LED_OFF); digitalWrite(SEG_E, LED_ON);  digitalWrite(SEG_F, LED_ON);  digitalWrite(SEG_G, LED_ON);  break;
    case 'n': digitalWrite(SEG_A, LED_OFF); digitalWrite(SEG_B, LED_OFF); digitalWrite(SEG_C, LED_ON);  digitalWrite(SEG_D, LED_OFF); digitalWrite(SEG_E, LED_ON);  digitalWrite(SEG_F, LED_OFF); digitalWrite(SEG_G, LED_ON);  break;
    case 'o': digitalWrite(SEG_A, LED_OFF); digitalWrite(SEG_B, LED_OFF); digitalWrite(SEG_C, LED_ON);  digitalWrite(SEG_D, LED_ON);  digitalWrite(SEG_E, LED_ON);  digitalWrite(SEG_F, LED_OFF); digitalWrite(SEG_G, LED_ON);  break;
    case 'P': digitalWrite(SEG_A, LED_ON);  digitalWrite(SEG_B, LED_ON);  digitalWrite(SEG_C, LED_OFF); digitalWrite(SEG_D, LED_OFF); digitalWrite(SEG_E, LED_ON);  digitalWrite(SEG_F, LED_ON);  digitalWrite(SEG_G, LED_ON);  break;
    case 'r': digitalWrite(SEG_A, LED_OFF); digitalWrite(SEG_B, LED_OFF); digitalWrite(SEG_C, LED_OFF); digitalWrite(SEG_D, LED_OFF); digitalWrite(SEG_E, LED_ON);  digitalWrite(SEG_F, LED_OFF); digitalWrite(SEG_G, LED_ON);  break;
    case 'S': digitalWrite(SEG_A, LED_ON);  digitalWrite(SEG_B, LED_OFF); digitalWrite(SEG_C, LED_ON);  digitalWrite(SEG_D, LED_ON);  digitalWrite(SEG_E, LED_OFF); digitalWrite(SEG_F, LED_ON);  digitalWrite(SEG_G, LED_ON);  break;
    case 't': digitalWrite(SEG_A, LED_OFF); digitalWrite(SEG_B, LED_OFF); digitalWrite(SEG_C, LED_OFF); digitalWrite(SEG_D, LED_ON);  digitalWrite(SEG_E, LED_ON);  digitalWrite(SEG_F, LED_ON);  digitalWrite(SEG_G, LED_ON);  break;
    case 'u': digitalWrite(SEG_A, LED_OFF); digitalWrite(SEG_B, LED_ON);  digitalWrite(SEG_C, LED_ON);  digitalWrite(SEG_D, LED_ON);  digitalWrite(SEG_E, LED_ON);  digitalWrite(SEG_F, LED_ON);  digitalWrite(SEG_G, LED_ON);  break;
    case 'y': digitalWrite(SEG_A, LED_OFF); digitalWrite(SEG_B, LED_ON);  digitalWrite(SEG_C, LED_ON);  digitalWrite(SEG_D, LED_ON);  digitalWrite(SEG_E, LED_OFF); digitalWrite(SEG_F, LED_ON);  digitalWrite(SEG_G, LED_ON);  break;
    default:  digitalWrite(SEG_A, LED_OFF); digitalWrite(SEG_B, LED_OFF); digitalWrite(SEG_C, LED_OFF); digitalWrite(SEG_D, LED_OFF); digitalWrite(SEG_E, LED_OFF); digitalWrite(SEG_F, LED_OFF); digitalWrite(SEG_G, LED_OFF); break;
  }
  switch(YaziNoktaMetni){
    case '0': digitalWrite(SEG_P, LED_OFF); break;
    case '1': digitalWrite(SEG_P, LED_ON);  break;
    default:  digitalWrite(SEG_P, LED_OFF); break;
  }
}
void refreshDisplay(){  
  switch (c_digit){
    case 2:
      digitalWrite(DIG_1, LED_OFF);
      digitalWrite(DIG_2, LED_OFF);
        EkranYaz(YaziYaz[2],YaziNokta[2]);
      digitalWrite(DIG_3, LED_ON);
      delay(1);
      break;
    case 1:
      digitalWrite(DIG_1, LED_OFF);
      digitalWrite(DIG_3, LED_OFF);
        EkranYaz(YaziYaz[1],YaziNokta[1]);
      digitalWrite(DIG_2, LED_ON);
      delay(1);
      break;
    case 0:
      digitalWrite(DIG_2, LED_OFF);
      digitalWrite(DIG_3, LED_OFF);
        EkranYaz(YaziYaz[0],YaziNokta[0]);
      digitalWrite(DIG_1, LED_ON);
      delay(1);
      break;
  }
  c_digit++;
  if (c_digit == 3) c_digit = 0;
}
void Buzzer(){
  digitalWrite (buzzerPin, HIGH); delay (75); digitalWrite (buzzerPin, LOW);
}
void BluetoothVeri(String BluetoothData){
    if(BluetoothData != ""){
          JsOku = "";
          JsonObject& JsOku = jsonBuffer.parseObject(BluetoothData);
            JsAC = JsOku["AC"].as<char*>();
            JsON = JsOku["ON"].as<char*>();
            JsPS = JsOku["PS"].as<char*>();
            JsFK = JsOku["FK"].as<char*>();
            JsFH = JsOku["FH"].as<char*>();
            JsSK = JsOku["SK"].as<char*>();
            JsTM = JsOku["TM"].as<char*>();
            JsFA = JsOku["FA"].as<char*>();
            JsYU = JsOku["YU"].as<char*>();
            JsSG = JsOku["SG"].as<char*>();
            JsSC = JsOku["SC"].as<char*>();
            JsSD = JsOku["SD"].as<char*>();
            JsHD = JsOku["HD"].as<char*>();
            JsWA = JsOku["WA"].as<char*>();
            JsWP = JsOku["WP"].as<char*>();
            JsURL = JsOku["URL"].as<char*>();
          jsonBuffer.clear();
          delay(20);

          if(JsAC == "Kaydet"){
            //EkranYaz(DilKayitEdildiUst[eDil], DilKayitEdildiAlt[eDil]);
              if(JsON != ""){
                if(JsON.toInt() == 0){
                  bOnValue = 0;
                  EEPROM.write(0, bOnValue);
                }else if(JsON.toInt() == 1){
                  bOnValue = 1;
                  EEPROM.write(0, bOnValue);
                }
              }
              if(JsPS != ""){
                if(JsPS.toInt() >= 32 && JsPS.toInt() <= 80){
                  ePSicaklik = JsPS.toInt();
                  EEPROM.write(1, ePSicaklik);
                }
              }
              if(JsFK != ""){
                if(JsFK.toInt() == 0){
                  eFanKontrol = 0;
                  EEPROM.write(6, eFanKontrol);
                }else if(JsFK.toInt() == 1){
                  eFanKontrol = 1;
                  EEPROM.write(6, eFanKontrol);
                }
              }
              if(JsFH != ""){
                if(JsFH.toInt() >= 1 && JsFH.toInt() <= 4){
                  eFanHizi = JsFH.toInt();
                  EEPROM.write(8, eFanHizi);
                }
              }
              if(JsSK != ""){
                if(JsSK.toInt() == 0){
                  eSirkuKontrol = 0;
                  EEPROM.write(2, eSirkuKontrol);
                }else if(JsSK.toInt() == 1){
                  eSirkuKontrol = 1;
                  EEPROM.write(2, eSirkuKontrol);
                }
              }
              if(JsTM != ""){
                if(JsTM.toInt() == 0){
                  eTermik = 0;
                  EEPROM.write(19, eTermik);
                }else if(JsTM.toInt() == 1){
                  eTermik = 1;
                  EEPROM.write(19, eTermik);
                }
              }
              if(JsFA != ""){
                if(JsFA.toInt() == 0){
                  eFanHizAyari = 0;
                  EEPROM.write(7, eFanHizAyari);
                }else if(JsFA.toInt() == 1){
                  eFanHizAyari = 1;
                  EEPROM.write(7, eFanHizAyari);
                }
              }
              if(JsYU != ""){
                if(JsYU.toInt() >= 20 && JsYU.toInt() <= 60){
                  eYakitBitti = JsYU.toInt();
                  EEPROM.write(10, eYakitBitti);
                }
              }
              if(JsSG != ""){
                if(JsSG.toInt() >= 20 && JsSG.toInt() <= 65){
                  eSirkuGiris = JsSG.toInt();
                  EEPROM.write(3, eSirkuGiris);
                }
              }
              if(JsSC != ""){
                if(JsSC.toInt() >= 20 && JsSC.toInt() <= 60){
                  eSirkuCikis = JsSC.toInt();
                  EEPROM.write(4, eSirkuCikis);
                }
              }
              if(JsSD != ""){
                if(JsSD.toInt() >= 1 && JsSD.toInt() <= 10){
                  eSirkuDon = JsSD.toInt();
                  EEPROM.write(5, eSirkuDon);
                }
              }
              if(JsHD != ""){
                if(JsHD.toInt() >= 2 && JsHD.toInt() <= 8){
                  eHisterezis = JsHD.toInt();
                  EEPROM.write(9, eHisterezis);
                }
              }           
              if((JsWA != "") && (JsWA != ssid)){
                EEPROM.writeString(100, JsWA);
                  ssid = &JsWA[0];
              }
              if((JsWP != "") && (JsWP != password)){
                EEPROM.writeString(132, JsWP);
                  password = &JsWP[0];
              }
              EEPROM.commit();
                  digitalWrite (buzzerPin, HIGH); delay(300); digitalWrite (buzzerPin, LOW); delay(200);
                  digitalWrite (buzzerPin, HIGH); delay(50); digitalWrite (buzzerPin, LOW); delay(50);
                  digitalWrite (buzzerPin, HIGH); delay(50); digitalWrite (buzzerPin, LOW);
              //PRINTA("Kaydet OK");
          }else if(JsAC == "Update"){
              //PRINTA("UPDATE VAR");
              eUpdate = 1;
                //PRINTA("eUrl:");    PRINT(JsURL);
                  EEPROM.write(18, 1);
                  EEPROM.writeString(50, JsURL);
                  EEPROM.commit();
              //EkranYaz(DilGuncBulunUst[eDil],DilGuncBulunAlt[eDil]);
              digitalWrite (buzzerPin, HIGH); delay(300); digitalWrite (buzzerPin, LOW); delay(50);
              digitalWrite (buzzerPin, HIGH); delay(50); digitalWrite (buzzerPin, LOW); delay(50);
              digitalWrite (buzzerPin, HIGH); delay(50); digitalWrite (buzzerPin, LOW);                          
              delay(3000);
              //EkranYaz(DilGuncModUst[eDil],DilGuncModAlt[eDil]);
              delay(3000);
              YenidenBasla();
          }else if(JsAC == "Oku" + MusteriCode){
              JsGon = "";
              JsYaz = "";
              JsonObject& JsYaz = jsonBuffer.createObject();
                JsYaz["SN"] = SeriNo;
                JsYaz["CV"] = inoCodeVersiyon;
                JsYaz["PV"] = PCBCodeVersiyon + MusteriCode;
                JsYaz["KS"] = temperatureE;
                JsYaz["CD"] = CihazDurum;
                JsYaz["PS"] = ePSicaklik;
                JsYaz["FK"] = eFanKontrol;
                JsYaz["FH"] = eFanHizi;
                JsYaz["SK"] = eSirkuKontrol;
                JsYaz["TM"] = eTermik;
                JsYaz["FA"] = eFanHizAyari;
                JsYaz["YU"] = eYakitBitti;
                JsYaz["SG"] = eSirkuGiris;
                JsYaz["SC"] = eSirkuCikis;
                JsYaz["SD"] = eSirkuDon;
                JsYaz["HD"] = eHisterezis;
                JsYaz["WA"] = ssid;
                JsYaz["WP"] = password;
              JsYaz.printTo(JsGon);
                digitalWrite (buzzerPin, HIGH);
                  delay(40);
                digitalWrite (buzzerPin, LOW);
              BluetoothGonder(JsGon);
                  //PRINTA("Gönderildi =");
                  //PRINT(JsGon);
              jsonBuffer.clear();
          }else if(JsAC == "Reboot"){
              //EkranYaz(DilRebootUst[eDil], DilRebootAlt[eDil]);            
                  digitalWrite (buzzerPin, HIGH); delay(50); digitalWrite (buzzerPin, LOW); delay(50);
                  digitalWrite (buzzerPin, HIGH); delay(50); digitalWrite (buzzerPin, LOW); delay(50);
                  digitalWrite (buzzerPin, HIGH); delay(50); digitalWrite (buzzerPin, LOW); delay(50);
                  digitalWrite (buzzerPin, HIGH); delay(500); digitalWrite (buzzerPin, LOW);
              YenidenBasla();
          }
    }
}
void BluetoothGonder(String line){
    unsigned l = line.length();
    for(int i=0; i<l; i++){
      if(line[i]!='\0')
      SerialBT.write(byte(line[i]));
    }
    SerialBT.write(10); // \n
}
void TaskEkran( void * parameter){
  (void) parameter;
    UBaseType_t uxHighWaterMark;
    uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
    //PRINTA(""); PRINT("TaskEkran Stack:");  PRINT(uxHighWaterMark);
    //PRINTA(""); PRINT("TaskEkran CPU: ");   PRINT(xPortGetCoreID());
  for (;;) {
    refreshDisplay(); // Ekran Tazeleme
  }
}
void TaskWeb( void * parameter){
  (void) parameter;
    delay(4000);  
    UBaseType_t uxHighWaterMark;
    uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
    //PRINTA(""); PRINT("TaskWeb Stack:");  PRINT(uxHighWaterMark);
    //PRINTA(""); PRINT("TaskWeb CPU: ");   PRINT(xPortGetCoreID());
  for (;;) {
    if ((WiFi.status() == WL_CONNECTED)){
      WifiValue = 1;
          if(TaskWebTime <= (timeClient.getEpochTime() - 60)){
            TaskWebTime = timeClient.getEpochTime();
            JsWGon = "";
            JsWYaz = "";
            JsonObject& JsWYaz = jsonBuffer.createObject();
              JsWYaz["SN"] = SeriNo;
              JsWYaz["AK"] = apiKey;
              JsWYaz["UP"] = Uptime;
              JsWYaz["CV"] = inoCodeVersiyon;
              JsWYaz["PV"] = PCBCodeVersiyon + MusteriCode;
              JsWYaz["KS"] = temperatureE;
              JsWYaz["CD"] = CihazDurum;
              JsWYaz["PS"] = ePSicaklik;
              JsWYaz["FK"] = eFanKontrol;
              JsWYaz["FH"] = eFanHizi;
              JsWYaz["SK"] = eSirkuKontrol;
              JsWYaz["TM"] = eTermik;
              JsWYaz["FA"] = eFanHizAyari;
              JsWYaz["YU"] = eYakitBitti;
              JsWYaz["SG"] = eSirkuGiris;
              JsWYaz["SC"] = eSirkuCikis;
              JsWYaz["SD"] = eSirkuDon;
              JsWYaz["HD"] = eHisterezis;
            JsWYaz.printTo(JsWGon);
                HttpPost("js=" + JsWGon);
            jsonBuffer.clear();
          }
    }else{
      WifiValue = 0;
          if(TaskWebTime <= (timeClient.getEpochTime() - 10)){
            TaskWebTime = timeClient.getEpochTime();
            WiFi.begin(ssid, password); //Wifi bağlan
          }
    }
  }
}
void HttpPost(String PostDATA){
    if ((WiFi.status() == WL_CONNECTED)){
          HTTPClient http;
          http.begin(serverName);
          http.addHeader("Content-Type", "application/x-www-form-urlencoded");
            //PRINTA("Data: "); PRINT(PostDATA);
          int httpResponseCode = http.POST(PostDATA);
          String payload = http.getString(); //Webden gelen
              if (httpResponseCode == 200){
                //PRINTA(payload);
                BluetoothVeri(payload);
              }
          http.end();
    }
}
void YenidenBasla(){
  digitalWrite(RFan, LOW);
  digitalWrite(RPompa, LOW);
  digitalWrite(SEG_A, LED_OFF); digitalWrite(SEG_B, LED_OFF); digitalWrite(SEG_C, LED_OFF); digitalWrite(SEG_D, LED_OFF); digitalWrite(SEG_E, LED_OFF); digitalWrite(SEG_F, LED_OFF); digitalWrite(SEG_G, LED_OFF); digitalWrite(SEG_P, LED_OFF);
  digitalWrite(DIG_1, LED_OFF); digitalWrite(DIG_2, LED_OFF); digitalWrite(DIG_3, LED_OFF);    
    delay(250);
      ESP.restart();
}
