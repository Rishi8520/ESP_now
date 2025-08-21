#include <esp_now.h>
#include <WiFi.h>

// Define the MAC address of the receiver
uint8_t broadcastAddress[] = {0x8C, 0x4F, 0x00, 0x2F, 0xAD, 0x94}; // Replace with your receiver's MAC address

// Structure to send and receive data
typedef struct struct_message {
  char message[32]; // Example: A string message
} struct_message;

struct_message myMessage;
struct_message incomingMessage;

// Callback function to confirm message delivery
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Callback function to process received data
void OnDataRecv(const esp_now_recv_info_t *recv_info, const uint8_t *incomingData, int len) {
  memcpy(&incomingMessage, incomingData, sizeof(incomingMessage));
  Serial.print("Received Message from Receiver: ");
  Serial.println(incomingMessage.message);
}

void setup() {
  Serial.begin(115200);

  // Set ESP32 in station mode
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register callbacks
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  // Add the receiver as a peer
  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0; // Use channel 0 (auto-select)
  peerInfo.encrypt = false; // No encryption

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  Serial.println("Peer added successfully");
}

void loop() {
  // Read user input from the serial monitor
  if (Serial.available() > 0) {
    String userInput = Serial.readStringUntil('\n');
    userInput.trim(); // Remove any trailing whitespace or newline characters
    strncpy(myMessage.message, userInput.c_str(), sizeof(myMessage.message));

    // Send the message to the receiver
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&myMessage, sizeof(myMessage));

    if (result == ESP_OK) {
      Serial.println("Message sent successfully");
    } else {
      Serial.println("Message failed to send");
    }
  }
}