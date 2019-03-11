#include <SoftwareSerial.h>

#define pump 12
#define led 13
#define MOS A0
#define LDR A1
int mos_val;
int lig_val;

String AP = "chris";
String PASS = "";
String API = "DPFZR0CGYU5SI8T5";
String HOST = "api.thingspeak.com";
String PORT = "80";
int countTrueCommand = 1;
int countTimeCommand;
boolean found = false;

SoftwareSerial esp8266(10, 11);

void setup() {
  Serial.begin(9600);
  esp8266.begin(115200);
  pinMode(led, OUTPUT);
  pinMode(pump, OUTPUT);
  sendCommand("AT", 5, "OK");
  sendCommand("AT+CWMODE=3", 5, "OK");
  sendCommand("AT+CWJAP=\"" + AP + "\",\"" + PASS + "\"", 20, "OK");
}

void loop() {
  String getData = "GET /update?api_key=" + API + "&field1=" + String(read_mos()) + "&field2=" + String(read_lig());
  sendCommand("AT+CIPMUX=1", 5, "OK");
  sendCommand("AT+CIPSTART=2,\"TCP\",\"" + HOST + "\",80", 15, "OK");
  sendCommand("AT+CIPSEND=2," + String(getData.length() + 4), 10, ">");
  esp8266.println(getData);
  countTrueCommand++;
  delay(10000);
  sendCommand("AT+CIPCLOSE", 5, "OK");
  if (mos_val > 800){
    digitalWrite(pump, HIGH);
    delay(5000);
    digitalWrite(pump, LOW);
  }
  if (lig_val < 780) {
    digitalWrite(led, HIGH);
  }
  else {
    digitalWrite(led, LOW);
  }
  Serial.println();
}

int read_lig() {
  lig_val = analogRead(LDR);
  Serial.print("Light value:  ");
  Serial.println(lig_val);
  return lig_val;}

int read_mos() {
  mos_val = analogRead(MOS);
  Serial.print("Moisture value:  ");
  Serial.println(mos_val);
  return mos_val;}

void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" -> ");
  while (countTimeCommand < (maxTime * 1)) {
    esp8266.println(command);//at+cipsend
    if (esp8266.find(readReplay)) //ok
    {
      found = true;
      break;
    }
    countTimeCommand++;
  }

  if (found == true)
  {
    Serial.println("OK");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  else {
    Serial.println("Fail");
    countTimeCommand = 0;
  }

  found = false;
}
