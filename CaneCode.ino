#include <Wire.h>
#include "Adafruit_VL53L0X.h"
#include <esp_now.h>
#include <WiFi.h>
int Location;


#define SENSOR_COUNT 5
#define BUTTON_PIN 4
#define GSM_RX_PIN 16
#define GSM_TX_PIN 17


const uint8_t newAddresses[SENSOR_COUNT] = {0x30, 0x31, 0x32, 0x33, 0x34};
const uint8_t xshutPins[SENSOR_COUNT] = {27, 26, 25, 33, 32};


Adafruit_VL53L0X sensors[SENSOR_COUNT];
VL53L0X_RangingMeasurementData_t measure;


uint8_t receiverMAC[] = {0x54, 0x32, 0x04, 0x33, 0x15, 0xfc};


typedef struct struct_message {
  uint16_t distances[SENSOR_COUNT];
} struct_message;


struct_message sensorData;
esp_now_peer_info_t peerInfo;


HardwareSerial GSM(1); // Use UART1


bool smsSent = false;
unsigned long lastButtonPressTime = 0;
const unsigned long debounceDelay = 300;


const char phoneNumber[] = "‪+919152422654‬";
//const char phoneNumber[] = "‪+917506196815‬";
// === Flush GSM Buffer ===
void flushGSM() {
  while (GSM.available()) {
    Serial.write(GSM.read());
  }
}


// === Set unique I2C address for each sensor ===
void setSensorAddresses() {
  for (int i = 0; i < SENSOR_COUNT; i++) {
    pinMode(xshutPins[i], OUTPUT);
    digitalWrite(xshutPins[i], LOW);
  }
  delay(10);


  for (int i = 0; i < SENSOR_COUNT; i++) {
    digitalWrite(xshutPins[i], HIGH);
    delay(50);


    if (!sensors[i].begin(0x29, &Wire)) {
      Serial.printf("Failed to start sensor %d\n", i + 1);
      while (1);
    }


    if (!sensors[i].setAddress(newAddresses[i])) {
      Serial.printf("Failed to set address for sensor %d\n", i + 1);
      while (1);
    }


    Serial.printf("Sensor %d at 0x%X\n", i + 1, newAddresses[i]);
  }
}


// === ESP-NOW Send Callback ===
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}


// === Send SMS ===
void sendSMS() {
  Serial.println("Sending SMS...");


  GSM.println("AT");
  delay(500);
  flushGSM();


  GSM.println("AT+CMGF=1");  // Text mode
  delay(500);
  flushGSM();


  GSM.println("AT+CSCS=\"GSM\"");  // Character set
  delay(500);
  flushGSM();


  GSM.print("AT+CMGS=\"");
  GSM.print(phoneNumber);
  GSM.println("\"");
  delay(1000);  // Wait for '>' prompt


  GSM.print("Hello, The Blind Person is in Danger Here's the location :",Location)
  delay(5000);    // Wait for message to be processed


  Serial.println("SMS sent successfully.");
}


// === Setup ===
void setup() {
  Serial.begin(115200);
  Wire.begin();
  pinMode(BUTTON_PIN, INPUT_PULLUP);


  // Init GSM
  GSM.begin(9600, SERIAL_8N1, GSM_RX_PIN, GSM_TX_PIN);
  delay(2000);
  GSM.println("AT");
  delay(500);
  flushGSM();
  GSM.println("AT+CMGF=1");
  delay(500);
  flushGSM();
  GSM.println("AT+CSCS=\"GSM\"");
  delay(500);
  flushGSM();
  Serial.println("GSM Module initialized.");


  // Init ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }


  esp_now_register_send_cb(OnDataSent);
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }


  setSensorAddresses();
}


// === Loop ===
void loop() {
  // Sensor reading
  for (int i = 0; i < SENSOR_COUNT; i++) {
    sensors[i].rangingTest(&measure, false);
    if (measure.RangeStatus != 4) {
      sensorData.distances[i] = measure.RangeMilliMeter;
    } else {
      sensorData.distances[i] = 0xFFFF;
    }
  }


  // Send via ESP-NOW
  esp_err_t result = esp_now_send(receiverMAC, (uint8_t *) &sensorData, sizeof(sensorData));
  if (result == ESP_OK) {
    Serial.println("Distance data sent.");
  } else {
    Serial.println("ESP-NOW send failed.");
  }


  // Button press triggers SMS
  if (digitalRead(BUTTON_PIN) == LOW) {
    unsigned long currentTime = millis();
    if (!smsSent && (currentTime - lastButtonPressTime > debounceDelay)) {
      sendSMS();
      smsSent = true;
      lastButtonPressTime = currentTime;
    }
  } else {
    smsSent = false;
  }


  // Print GSM responses (optional)
  flushGSM();


  delay(500);
}
