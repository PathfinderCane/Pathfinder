#include <esp_now.h>
#include <WiFi.h>
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

#define SENSOR_COUNT 5  // Match sender's sensor count

// DFPlayer Mini pins
static const uint8_t PIN_MP3_TX = D2; // to module RX
static const uint8_t PIN_MP3_RX = D1; // to module TX
SoftwareSerial softwareSerial(PIN_MP3_RX, PIN_MP3_TX);
DFRobotDFPlayerMini player;

// Distance data structure
typedef struct struct_message {
  uint16_t distances[SENSOR_COUNT];
} struct_message;

struct_message incomingData;

bool audioPlaying = false;
unsigned long lastPlayTime = 0;
const unsigned long playDelay = 5000; // delay between messages (ms)

void playAudio(int fileNumber) {
  if (!audioPlaying || (millis() - lastPlayTime > playDelay)) {
    player.play(fileNumber);
    Serial.print("Playing audio file: ");
    Serial.println(fileNumber);
    audioPlaying = true;
    lastPlayTime = millis();
  }
}

// ESP-NOW receive callback
void OnDataRecv(const esp_now_recv_info_t *recv_info, const uint8_t *incomingDataRaw, int len) {
  memcpy(&incomingData, incomingDataRaw, sizeof(incomingData));

  Serial.print("Data received from MAC: ");
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
           recv_info->src_addr[0], recv_info->src_addr[1], recv_info->src_addr[2],
           recv_info->src_addr[3], recv_info->src_addr[4], recv_info->src_addr[5]);
  Serial.println(macStr);

  for (int i = 0; i < SENSOR_COUNT; i++) {
    Serial.print("Sensor ");
    Serial.print(i + 1);
    Serial.print(": ");
    if (incomingData.distances[i] != 0xFFFF) {
      Serial.print(incomingData.distances[i]);
      Serial.println(" mm");
    } else {
      Serial.println("Out of Range");
    }
  }

  Serial.println("--------------------------");

  // Apply condition checks
  if (incomingData.distances[0] < 500) playAudio(1);     // Sensor 1
  else if (incomingData.distances[1] < 500) playAudio(5); // Sensor 2
  else if (incomingData.distances[2] < 500) playAudio(6); // Sensor 3
  else if (incomingData.distances[4] < 500) playAudio(5); // Sensor 5
  else if (incomingData.distances[3] > 500) playAudio(2);  // Sensor 4
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW initialization failed!");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
  Serial.println("ESP-NOW Receiver Initialized");

  softwareSerial.begin(9600);

  if (player.begin(softwareSerial)) {
    Serial.println("DFPlayer Mini connected.");
    player.volume(12);  // Set initial volume
  } else {
    Serial.println("Failed to connect to DFPlayer Mini. Restarting ESP...");
    // delay(1000);  // Let message print before restarting
    // ESP.restart(); // Restart once
  }
}


void loop() {
  // Reset audioPlaying flag after play delay
  if (audioPlaying && (millis() - lastPlayTime > playDelay)) {
    audioPlaying = false;
  }
}
