#include <Arduino.h>
#include <ESP8266WiFi.h>
const char* ssid = "";        // Replace with your hotspot name
const char* password = ""; // Replace with your hotspot password
// === Thresholds for Safe Drinking Water ===
#define PH_MIN 6.5
#define PH_MAX 8.5

#define TDS_MAX 300.0          // ppm
#define TURBIDITY_MAX 5.0      // NTU
#define ENDOTOXIN_MAX 10.0     // simulated units

// === Hardware pins (NodeMCU / Wemos D1 Mini) ===
// Built-in LED is usually on GPIO2 (D4)
#define LED_PIN 2     // D4 on NodeMCU
#define BUZZER_PIN 5  // D1 on NodeMCU (optional buzzer)

// Data buffer
String incomingData = "";

void setup() {
  Serial.begin(9600);  // Serial communication with Python simulation
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  WiFi.begin(ssid, password);  // Start connecting
  Serial.println("Connecting to Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to Wi-Fi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());  
  Serial.println("ESP8266 Water Quality Monitor Started...");
  Serial.println("Waiting for data: pH, TDS, Turbidity, Endotoxin, Ecoli");
}

void loop() {
  // Read data from Serial
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      processIncomingData(incomingData);
      incomingData = "";  // Reset buffer
    } else {
      incomingData += c;
    }
  }
}

void processIncomingData(String data) {
  // Example incoming data: "7.3,120.5,2.1,5.4,0"
  float pH, tds, turbidity, endotoxin;
  int ecoli;

  // Split CSV data
  int index1 = data.indexOf(',');
  int index2 = data.indexOf(',', index1 + 1);
  int index3 = data.indexOf(',', index2 + 1);
  int index4 = data.indexOf(',', index3 + 1);

  if (index1 == -1 || index2 == -1 || index3 == -1 || index4 == -1) {
    Serial.println("Invalid data received!");
    return;
  }

  // Convert to float/int
  pH = data.substring(0, index1).toFloat();
  tds = data.substring(index1 + 1, index2).toFloat();
  turbidity = data.substring(index2 + 1, index3).toFloat();
  endotoxin = data.substring(index3 + 1, index4).toFloat();
  ecoli = data.substring(index4 + 1).toInt();

  // Print received data
  Serial.print("Received -> pH: "); Serial.print(pH);
  Serial.print(" | TDS: "); Serial.print(tds);
  Serial.print(" ppm | Turbidity: "); Serial.print(turbidity);
  Serial.print(" NTU | Endotoxin: "); Serial.print(endotoxin);
  Serial.print(" | Ecoli: "); Serial.println(ecoli);

  // Evaluate water safety
  if (isWaterSafe(pH, tds, turbidity, endotoxin, ecoli)) {
    Serial.println("✅ WATER IS SAFE TO DRINK\n");
    digitalWrite(LED_PIN, HIGH);    // LED ON
    digitalWrite(BUZZER_PIN, LOW);  // Buzzer OFF
  } else {
    Serial.println("❌ WATER NOT SAFE TO DRINK\n");
    digitalWrite(LED_PIN, LOW);     // LED OFF
    digitalWrite(BUZZER_PIN, HIGH); // Buzzer ON
  }
}

bool isWaterSafe(float pH, float tds, float turbidity, float endotoxin, int ecoli) {
  if (pH < PH_MIN || pH > PH_MAX) return false;
  if (tds > TDS_MAX) return false;
  if (turbidity > TURBIDITY_MAX) return false;
  if (endotoxin > ENDOTOXIN_MAX) return false;
  if (ecoli != 0) return false; // must be 0
  return true;
}
