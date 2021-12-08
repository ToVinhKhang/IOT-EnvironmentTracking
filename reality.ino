#include <DHT.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <LiquidCrystal_I2C.h>

int PIRPIN = 14;
int LEDPIN_RED = 2;
int LEDPIN_GREEN = 4;
int BUZZERPIN = 27;
int DHTPIN = 13;
int DHTTYPE = DHT11;
char wifiName[] = "Asus";
char wifiPass[] = "87654321";
char server[] = "api.thingspeak.com";
String THINGSPEAK_APIKEY = "18425AZU8BBP3R98";

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27,16,2);
WiFiClient client;

void setup(){
	Serial.begin(9600);
	dht.begin();
	lcd.init();  
	lcd.backlight();
	pinMode(LEDPIN_RED, OUTPUT);
	pinMode(LEDPIN_GREEN, OUTPUT);
	pinMode(PIRPIN, INPUT);
	pinMode(BUZZERPIN, OUTPUT);
	connectToWifi();
}

void loop(){
	float h = dht.readHumidity();
	float t = dht.readTemperature();
	int statusPIR = digitalRead(PIRPIN);
	
	if(isnan(t) || isnan(h)) {} 
	else{
		printToLCD(t,h);
	}
	
	if(statusPIR == HIGH){
		playBuzzer(1);
		Serial.println("Phat hien chuyen dong!");
	}
	else{
		playBuzzer(0);
		Serial.println("Khong phat hien chuyen dong!");
	}
	delay(1000);
	
	if (client.connect(server, 80)) {
		printToThingSpeak(t,h);
		Serial.printf("Nhiet do %s - Do am %s\r\n", String(t, 2).c_str(), String(h, 2).c_str());
	}
	client.stop();
}

void playBuzzer(int statusPIR){
	if(statusPIR == 1){
		digitalWrite(LEDPIN_GREEN, LOW);
		digitalWrite(LEDPIN_RED, HIGH);
		digitalWrite(BUZZERPIN, HIGH);
		delay(3000);
	}
	else{
		digitalWrite(LEDPIN_GREEN, HIGH);
		digitalWrite(LEDPIN_RED, LOW);
		digitalWrite(BUZZERPIN, LOW);
	}
}

void connectToWifi(){
	Serial.print("Connecting");
	WiFi.begin(wifiName, wifiPass);
	while(WiFi.status() != WL_CONNECTED){
		Serial.print(".");
		delay(500);
	}
	Serial.println("\r\nWiFi connected");
}

void printToLCD(float t, float h){
	lcd.setCursor(0,0);
	lcd.print("Nhiet do: ");
	lcd.setCursor(0,1);
	lcd.print("Do am:    ");
	lcd.setCursor(10,0);
	lcd.print(String(t, 2).c_str());
	lcd.print(" ");
	lcd.setCursor(10,1);
	lcd.print(String(h, 2).c_str());
}

void printToThingSpeak(float t,float h){
	String body = "field1=" + String(t, 1) + "&field2=" + String(h, 1);
	client.print("POST /update HTTP/1.1\n");
	client.print("Host: api.thingspeak.com\n");
	client.print("Connection: close\n");
	client.print("X-THINGSPEAKAPIKEY: " + THINGSPEAK_APIKEY + "\n");
	client.print("Content-Type: application/x-www-form-urlencoded\n");
	client.print("Content-Length: ");
	client.print(body.length());
	client.print("\n\n");
	client.print(body);
	client.print("\n\n");
}
