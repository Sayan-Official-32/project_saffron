#ifndef CONFIG_H
#define CONFIG_H

// ===== *** Supabase Configuration *** =====
extern const char* supabase_url;
extern const char* supabase_key;

// ===== *** WiFi Setup Configuration *** =====
extern const char* wifi_setup_ssid;
extern const char* wifi_setup_password;

// ===== *** Hardware Pins *** =====
// MQ135 and MOSFET UART
#define RXD2 16
#define TXD2 17

// DHT22
#define DHTTYPE DHT22
#define DHT_PIN 4

// Moisture Sensor
#define MOISTURE_PIN 34
#define MOISTURE_DRY 0
#define MOISTURE_WET 4095

// Relays
#define RELAY_MIST 23
#define RELAY_FAN 19
#define RELAY_UV 18
#define RELAY_EXHAUST 5

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define I2C_ADDRESS 0x3C
#define OLED_RESET -1

// BH1750 I2C Pins
#define I2C_SDA 21
#define I2C_SCL 22

// ===== *** Timing Configuration *** =====
#define SENSOR_UPDATE_INTERVAL 5000 // 5 seconds
#define ACTUATOR_CHECK_INTERVAL 2000 // 2 seconds
#define WIFI_RECONNECT_INTERVAL 15000 // 15 seconds

#endif // CONFIG_H
