#include <WiFi.h>
#include <SoftwareSerial.h>

#include <HTTPClient.h>

#define SSID_NAME "ESQUINA"
#define SSID_PASSWORD "ledis123"

#define SENSOR_USER "test_user"
#define SENSOR_PASSWORD "test_password"

#define RX_PIN 21
#define TX_PIN 22

static const char *sensor_server_host = "http://54.198.117.76:65000/data";

SoftwareSerial esp32Serial(RX_PIN, TX_PIN);

#define NOISE_TOLERANCE 32
#define MAX_VALUE 60
#define INCREASE_VALUE 0.02
#define DECREASE_VALUE -0.1
#define SLEEP_TIME 10

bool increase_time = true;
float mod_time = 0;
static const double noise_mirror = 255 / NOISE_TOLERANCE;

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

    pinMode(12, OUTPUT);
    pinMode(13, OUTPUT);

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void loop()
{
    if ((WiFi.status() == WL_CONNECTED))
    {
        byte noise = 0;

        float temperature = ((float(sin(mod_time) + 1) * MAX_VALUE / 2) + (noise / NOISE_TOLERANCE) - noise_mirror);

        float humidity = ((temperature) / 8 + 30);

        mod_time += increase_time ? INCREASE_VALUE : DECREASE_VALUE;
        if (mod_time >= 3.14)
        {
            increase_time = false;
        }
        if (mod_time <= 0)
        {
            increase_time = true;
        }

        if (temperature > 50)
        {
            digitalWrite(12, HIGH);
        }
        else
        {
            digitalWrite(12, LOW);
        }

        if (humidity > 45)
        {
            digitalWrite(13, HIGH);
        }
        else
        {
            digitalWrite(13, LOW);
        }

        HTTPClient http;

        Serial.print("[HTTP] begin...\n");

        String url = sensor_server_host;

        url.concat("?t=");
        url.concat(String(temperature, 3));
        url.concat("&h=");
        url.concat(String(humidity, 3));

        Serial.println(url);

        http.begin(url);
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

        delay(5000);
    }
}
