//ESP8266 ile thingspeak'e veri gönderme

#include <SoftwareSerial.h>                                   //Gerekli kütüphaneleri ekledik.                                          

String agAdi = "GIBIRNET_2.4GHz";                     //Ağ adı.    
String agSifresi = ".ttnet321,";                              //Ağ şifresi.
String apiKey = "4A2HPLG8ILYXZXZQ";

int rxPin = 2;                                               //ESP8266 TX pinine
int txPin = 3;                                               //ESP8266 RX pinine
int dht11Pin = 2;

String ip = "184.106.153.149";                                //Thingspeak ip adresi
float nem, sicaklik;

SoftwareSerial esp(rxPin, txPin);                             //Seri haberleşme pin ayarları

void setup() {  
  
  Serial.begin(9600);                                         //Seri port ile haberleşme başlatılıyor.
  Serial.println("Bağlantı Başlatılıyor");
  esp.begin(9600);                                            //ESP8266 ile seri haberleşme başlatılıyor.
  esp.println("AT");                                          //AT komutu ile modül kontrolünü yapıyoruz.
  Serial.println("AT Komutu Yollandı");
  while(!esp.find("OK")){                                     //Modülün hazır olmasını bekliyoruz.
    esp.println("AT");
    Serial.println("ESP8266 ile Bağlantı Kurulamadı.");
  }
  Serial.println("OK Cevabı Alındı");
  esp.println("AT+CWMODE=1");                                 //ESP8266 modülünü client olarak ayarlıyoruz.
  delay(500);
  while(!esp.find("no change")){                                     //Ayar yapılana kadar bekliyoruz.
    esp.println("AT+CWMODE=1");
    Serial.println("Modül Türü Ayarlanıyor.");
  }
  Serial.println("Client olarak ayarlandı");
  Serial.println("Ağa Bağlanılıyor...");
  esp.println("AT+CWJAP=\""+agAdi+"\",\""+agSifresi+"\"");    //Ağa bağlanıyoruz.
  while(!esp.find("OK"));                                     //Ağa bağlanana kadar bekliyoruz.
  Serial.println("Ağa Bağlanıldı.");
  delay(1000);
}
void loop() {
  esp.println("AT+CIPSTART=\"TCP\",\""+ip+"\",80");           //Thingspeak'e bağlanıyoruz.
  
  if(esp.find("Error")){                                      //Bağlantı hatası kontrolü yapıyoruz.
    Serial.println("AT+CIPSTART Error");
  }
  
  sicaklik = 0;                        //sıcaklık bilgisini aldık
  nem = 1;                                //nem bilgisini aldık
  
  String veri = "GET https://api.thingspeak.com/update?api_key=";   //Thingspeak komutu. Key kısmına kendi api keyimizi yazıcaz.                                  
  veri += apiKey;
  veri += "&field1=";
  veri += String(sicaklik);                                   //Gönderilecek sıcaklık değişkeni
  veri += "&field2=";
  veri += String(nem);                                        //Gönderilecek nem değişkeni
  veri += "\r\n\r\n"; 
  esp.print("AT+CIPSEND=");                                   //ESP'ye gönderilecek veri uzunluğu.
  esp.println(veri.length()+2);
  delay(2000);
  if(esp.find(">")){                                          //ESP8266 hazır olduğunda içindeki komutlar çalışıyor.
    esp.print(veri);                                          //Veriyi gönderiyoruz.
    Serial.println(veri);
    Serial.println("Veri gonderildi.");
    delay(1000);
  }
  Serial.println("Baglantı Kapatildi.");
  esp.println("AT+CIPCLOSE");                                //Bağlantıyı kapatıyoruz
  delay(1000);                                               //Yeni veri gönderimi için 1 saniye bekliyoruz.
}
