#include <WiFi.h>
#include <SoftwareSerial.h>

#include <HTTPClient.h>

#define SSID_NAME "ESQUINA"
#define SSID_PASSWORD "ledis123"

#define SENSOR_USER "test_user"
#define SENSOR_PASSWORD "test_password"

#define RX_PIN 21
#define TX_PIN 22

static const char *sensor_server_host = "http://google.com";

SoftwareSerial esp32Serial(RX_PIN, TX_PIN);

void setup()
{

    Serial.begin(115200);
    esp32Serial.begin(4800);
    esp32Serial.listen();
    WiFi.begin(SSID_NAME, SSID_PASSWORD);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void loop()
{
    if ((WiFi.status() == WL_CONNECTED))
    {
        float temperature;
        float humidity;
        Serial.println("Requesting data from Arduino...");
        esp32Serial.write((byte)255);
        uint8_t read_data_count = 0;
        byte data[8];
        while (read_data_count < 8)
        {
            if (esp32Serial.available())
            {
                data[read_data_count] = esp32Serial.read();
                read_data_count++;
                Serial.println("1 byte");
            }
        };

        ((uint8_t *)&temperature)[0] = data[0];
        ((uint8_t *)&temperature)[1] = data[1];
        ((uint8_t *)&temperature)[2] = data[2];
        ((uint8_t *)&temperature)[3] = data[3];
        ((uint8_t *)&humidity)[0] = data[4];
        ((uint8_t *)&humidity)[1] = data[5];
        ((uint8_t *)&humidity)[2] = data[6];
        ((uint8_t *)&humidity)[3] = data[7];

        HTTPClient http;

        Serial.print("[HTTP] begin...\n");

        http.begin(sensor_server_host);
        http.setAuthorization(SENSOR_USER, SENSOR_PASSWORD);

        Serial.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        int httpCode = http.GET();

        if (httpCode > 0)
        {
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);

            if (httpCode == HTTP_CODE_OK)
            {
                Serial.print("Data report successful");
            }
        }
        else
        {
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
    }
}
