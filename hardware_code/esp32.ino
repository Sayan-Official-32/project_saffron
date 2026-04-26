#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Wire.h>
#include <DHT.h>
#include <BH1750.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

#include "config.h"

// ===== *** Configuration Variables *** =====
const char* supabase_url = "https://jhgnrbujsggsllzkgsfb.supabase.co"; 
const char* supabase_key = "sb_publishable_RnCtzZEfz-BYwDZ7_X73iw_Q_hdk44_"; 
const char* wifi_setup_ssid = "ESP32_Setup";
const char* wifi_setup_password = "saffron2026";

// ===== *** Global Instances *** =====
BH1750 lightMeter;
WiFiManager wm;
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
DHT dht(DHT_PIN, DHTTYPE);
WiFiClientSecure secureClient;

// ===== *** Global State *** =====
unsigned long lastSensorUpdate = 0;
unsigned long lastActuatorCheck = 0;
unsigned long lastReconnect = 0;

int CO2 = 0;
int pwmValue = 120;
float humidity = 0.0;
float temperature = 0.0;
int moisturePercent = 0;
int lux = 0;

int wifiFailCount = 0;
bool wifiShown = false;

// ===== *** Function Prototypes *** =====
void setupHardware();
void setupWiFi();
void checkWiFiConnection();
void readSensors();
void syncSupabaseSensors();
void fetchSupabaseActuators();
void updateDisplay();

void setup() {
    Serial.begin(115200);
    setupHardware();
    setupWiFi();
    
    // Set up secure client trust
    secureClient.setInsecure();
}

void loop() {
    checkWiFiConnection();
    readSensors();

    if (WiFi.status() == WL_CONNECTED) {
        syncSupabaseSensors();
        fetchSupabaseActuators();
    }

    updateDisplay();
    delay(1000);
}

// ===== *** Function Implementations *** =====

void setupHardware() {
    // 1. OLED Setup
    if(!display.begin(I2C_ADDRESS, true)){
        Serial.println("OLED Not Found.");
        while(1);
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);

    // 2. DHT Setup
    dht.begin(); 

    // 3. UART for MQ135 (Arduino UNO)
    Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
    Serial2.setTimeout(50);

    // 4. I2C for BH1750
    Wire.begin(I2C_SDA, I2C_SCL);
    if (lightMeter.begin()) {
        Serial.println("BH1750 Initialized");
    } else {
        Serial.println("Error Initializing BH1750");
    }

    // 5. Relays Setup
    pinMode(RELAY_MIST, OUTPUT);
    pinMode(RELAY_FAN, OUTPUT);
    pinMode(RELAY_UV, OUTPUT);
    pinMode(RELAY_EXHAUST, OUTPUT);

    // Turn OFF all relays initially (Active LOW)
    digitalWrite(RELAY_MIST, HIGH);
    digitalWrite(RELAY_FAN, HIGH);
    digitalWrite(RELAY_UV, HIGH);
    digitalWrite(RELAY_EXHAUST, HIGH);
}

void setupWiFi() {
    Serial.println("\nStarting Smart WiFi System...");
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
    WiFi.setSleep(false);

    wm.setDebugOutput(true);
    wm.setConfigPortalTimeout(180);

    display.clearDisplay();
    display.setCursor(0,10);
    display.println("WiFi Starting...");
    display.display();
    delay(1000);

    bool res = wm.autoConnect(wifi_setup_ssid, wifi_setup_password);

    if (res) {
        Serial.println("WiFi Connected!");
        display.clearDisplay();
        display.setCursor(0,10);
        display.println("WiFi Connected!\n");
        display.print("IP : ");
        display.println(WiFi.localIP());
        display.print("RSSI : ");
        display.print(WiFi.RSSI());
        display.println(" dBm");
        display.display();
        delay(4000);
    } else {
        Serial.println("Opening Config Portal...");
        display.clearDisplay();
        display.setCursor(0,10);
        display.println("Open WiFi Setup");
        display.print("SSID: ");
        display.println(wifi_setup_ssid);
        display.display();
        wm.startConfigPortal(wifi_setup_ssid, wifi_setup_password);
    }
}

void checkWiFiConnection() {
    wl_status_t status = WiFi.status();

    if (status == WL_CONNECTED) {
        if (!wifiShown) {
            Serial.println("WiFi Connected");
            wifiFailCount = 0;
            wifiShown = true;
        }
    } else {
        wifiShown = false;
        if (millis() - lastReconnect > WIFI_RECONNECT_INTERVAL) {
            Serial.println("🔄 Reconnecting...");
            WiFi.reconnect();
            lastReconnect = millis();
            wifiFailCount++;
        }

        if (wifiFailCount > 5) {
            display.clearDisplay();
            display.setCursor(0,0);
            display.println("Reconnect Failed");
            display.println("Setup WiFi");
            display.display();

            wm.startConfigPortal(wifi_setup_ssid, wifi_setup_password);
            delay(2000);
            wifiFailCount = 0;
        }
    }
}

void readSensors() {
    // DHT22
    humidity = dht.readHumidity();
    temperature = dht.readTemperature();
    if (isnan(humidity) || isnan(temperature)) {
        Serial.println("DHT22 Sensor Error!");
    }

    // Moisture
    int moistureSensorValue = analogRead(MOISTURE_PIN);
    moisturePercent = map(moistureSensorValue, MOISTURE_WET, MOISTURE_DRY, 0, 100);
    moisturePercent = constrain(moisturePercent, 0, 100);

    // Send PWM to Arduino UNO
    Serial2.println(pwmValue);

    // Read CO2 from Arduino UNO
    if (Serial2.available()) {
        String data = Serial2.readStringUntil('\n');
        data.trim();
        if (data.startsWith("CO2:")) { 
            CO2 = data.substring(4).toInt();
        }
    }

    // BH1750
    lux = lightMeter.readLightLevel();

    // Log to Serial
    float feel_like = dht.computeHeatIndex(temperature, humidity, false);
    Serial.printf("Temp: %.2f C | Hum: %.2f %% => Feels Like: %.2f C\n", temperature, humidity, feel_like);
    Serial.printf("Moisture: %d %%\n", moisturePercent);
    Serial.printf("CO2: %d ppm\n", CO2);
    Serial.printf("Light: %d lx\n", lux);
}

void syncSupabaseSensors() {
    if (millis() - lastSensorUpdate > SENSOR_UPDATE_INTERVAL) {
        HTTPClient http;
        String url = String(supabase_url) + "/rest/v1/sensor_data";
        
        http.begin(secureClient, url);
        http.addHeader("apikey", supabase_key);
        http.addHeader("Authorization", String("Bearer ") + supabase_key);
        http.addHeader("Content-Type", "application/json");
        http.addHeader("Prefer", "return=minimal");

        String payload = "{\"temperature\":" + String(temperature) + 
                         ",\"humidity\":" + String(humidity) + 
                         ",\"moisture\":" + String(moisturePercent) + 
                         ",\"co2\":" + String(CO2) + 
                         ",\"light\":" + String(lux) + "}";
        
        int httpResponseCode = http.POST(payload);
        Serial.printf("Supabase Sensor POST Response: %d\n", httpResponseCode);
        http.end();
        
        lastSensorUpdate = millis();
    }
}

void fetchSupabaseActuators() {
    if (millis() - lastActuatorCheck > ACTUATOR_CHECK_INTERVAL) {
        HTTPClient http;
        String url = String(supabase_url) + "/rest/v1/actuators?id=eq.1&select=*";
        
        http.begin(secureClient, url);
        http.addHeader("apikey", supabase_key);
        http.addHeader("Authorization", String("Bearer ") + supabase_key);

        int httpResponseCode = http.GET();
        if (httpResponseCode == 200) {
            String response = http.getString();
            StaticJsonDocument<512> doc;
            DeserializationError error = deserializeJson(doc, response);

            if (!error && doc.size() > 0) {
                bool mist = doc[0]["mist_maker"];
                bool fan = doc[0]["cooling_fan"];
                bool r3 = doc[0]["relay3"];
                bool r4 = doc[0]["relay4"];
                pwmValue = doc[0]["grow_light_pwm"];

                // Relays are active LOW
                digitalWrite(RELAY_MIST, mist ? LOW : HIGH);
                digitalWrite(RELAY_FAN, fan ? LOW : HIGH);
                digitalWrite(RELAY_UV, r3 ? LOW : HIGH);
                digitalWrite(RELAY_EXHAUST, r4 ? LOW : HIGH);
                
                Serial.println("Actuators Updated from Supabase!");
            }
        }
        http.end();
        lastActuatorCheck = millis();
    }
}

void updateDisplay() {
    display.clearDisplay();
    
    display.setCursor(0,10);
    display.printf("Temp : %.1f ", temperature);
    display.drawCircle(116, 12, 2, SH110X_WHITE);
    display.println(" C");

    display.setCursor(0,20);
    display.printf("Hum  : %.2f %%\n", humidity);

    display.setCursor(0,30);
    display.printf("Moist: %d %%\n", moisturePercent);

    display.setCursor(0,40);
    display.printf("CO2  : %d ppm\n", CO2);

    display.setCursor(0,50);
    display.printf("Light: %d lx\n", lux);

    display.display();
}