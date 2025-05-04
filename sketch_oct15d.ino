#include <WiFi.h>

const char* ssid = "savani";
const char* password = "12345678";

WiFiServer server(80);

// IR sensor pins
const int irSensor1 = 2; // Box 1
const int irSensor2 = 4; // Box 2

int inventoryBox1 = 0;
int inventoryBox2 = 0;

void setup() {
    Serial.begin(115200);
    Serial.println("\nStarting Wi-Fi Connection...");

    WiFi.begin(ssid, password);

    // Wait for connection and debug
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }

    Serial.println("\nConnected to Wi-Fi");
    Serial.println("====================");
    Serial.println("ESP32 IP Address:");
    Serial.println(WiFi.localIP());
    Serial.println("====================");

    server.begin();
    
    pinMode(irSensor1, INPUT);
    pinMode(irSensor2, INPUT);
}

void loop() {
    WiFiClient client = server.available();
    if (client) {
        String request = client.readStringUntil('\r');
        Serial.println(request);  // Print the incoming request

        // Check for the fill command
        if (request.indexOf("GET /fill") != -1) {
            if (digitalRead(irSensor1) == LOW) { // Object detected in Box 1
                inventoryBox1++;
                Serial.println("Fill command received for Box 1");
            } else if (digitalRead(irSensor2) == LOW) { // Object detected in Box 2
                inventoryBox2++;
                Serial.println("Fill command received for Box 2");
            }
        }

        // Check for the remove command
        else if (request.indexOf("GET /remove") != -1) {
            if (digitalRead(irSensor1) == LOW && inventoryBox1 > 0) {
                inventoryBox1--;
                Serial.println("Remove command received for Box 1");
            } else if (digitalRead(irSensor2) == LOW && inventoryBox2 > 0) {
                inventoryBox2--;
                Serial.println("Remove command received for Box 2");
            }
        }

        // Check for the status command 
        else if (request.indexOf("GET /status") != -1) {
            String response = "HTTP/1.1 200 OK\nContent-type:text/html\n\n";
            response += "<html><body>";
            response += "<p>Box 1: " + String(inventoryBox1) + "</p><p>Box 2: " + String(inventoryBox2) + "</p>";
            response += "</body></html>";
            client.print(response);
            return;
        }

        // Send a response back to the client
        client.println("HTTP/1.1 200 OK");
        client.println("Content-type:text/html");
        client.println();
        client.println("<html><body><h1>Command received</h1></body></html>");
        client.stop();
    }
}
