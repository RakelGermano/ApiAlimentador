#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <PubSubClient.h>

// Definições
#define TOPICO_SUBSCRIBE "Alimentador_TccRecebe"
#define TOPICO_PUBLISH   "Alimentador_TccEnvio"
#define ID_MQTT  "Rakel"

// Pinagem do NodeMCU
#define LED_PIN    14 // D5 (GPIO14)

// Configuração da rede WiFi
const char* SSID = "RENATA";
const char* PASSWORD = "010507ggr!";

// URL do servidor Django para enviar dados
const char* SERVER_URL = "http://192.168.15.114:8000/update_button/";

// Configurações MQTT
const char* BROKER_MQTT = "mqtt.eclipseprojects.io";
int BROKER_PORT = 1883;

// Objetos globais
WiFiClient espClient;
PubSubClient MQTT(espClient);

// Protótipos das funções
void initWiFi();
void initMQTT();
void reconnectWiFi();
void reconnectMQTT();
void VerificaConexoesWiFIEMQTT();
void mqtt_callback(char* topic, byte* payload, unsigned int length);
String getCSRFToken();
void enviaEstadoBotao();

void setup() {
    pinMode(D1, INPUT);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    Serial.begin(115200);
    initWiFi();
    initMQTT();
}

void loop() {
    VerificaConexoesWiFIEMQTT();
    MQTT.loop();
    enviaEstadoBotao();
    delay(1000);
}

void initWiFi() {
    delay(10);
    Serial.println("Conectando-se na rede WiFi...");
    WiFi.begin(SSID, PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("Conectado na rede WiFi");
    Serial.println("Endereço IP:");
    Serial.println(WiFi.localIP());
}

void initMQTT() {
    MQTT.setServer(BROKER_MQTT, BROKER_PORT);
    MQTT.setCallback(mqtt_callback);
}

void reconnectMQTT() {
    while (!MQTT.connected()) {
        Serial.print("* Tentando se conectar ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT)) {
            Serial.println("Conectado com sucesso ao broker MQTT!");
            MQTT.subscribe(TOPICO_SUBSCRIBE);
        } else {
            Serial.println("Falha ao reconectar no broker.");
            Serial.println("Nova tentativa de conexão em 2s");
            delay(2000);
        }
    }
}

void reconnectWiFi() {
    if (WiFi.status() == WL_CONNECTED)
        return;

    WiFi.begin(SSID, PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("Reconectado na rede WiFi");
    Serial.println("Endereço IP:");
    Serial.println(WiFi.localIP());
}

void VerificaConexoesWiFIEMQTT() {
    if (!MQTT.connected())
        reconnectMQTT();
    reconnectWiFi();
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    String msg;
    for (int i = 0; i < length; i++) {
        char c = (char)payload[i];
        msg += c;
    }
    Serial.println(msg);

    if (msg == "ligar") {
        Serial.println("Recebeu Comando: Ligar");
        digitalWrite(LED_PIN, HIGH); 
    } else if (msg == "desligar") {
        Serial.println("Recebeu Comando: Desligar"); 
        digitalWrite(LED_PIN, LOW);
    }
}

String getCSRFToken() {
    HTTPClient http;
    WiFiClient client;

    String url = "http://192.168.15.114:8000/get_csrf_token/";
    http.begin(client, url);

    int httpResponseCode = http.GET();
    String csrfToken = "";

    if (httpResponseCode > 0) {
        csrfToken = http.getString();
        csrfToken.trim();

        if (csrfToken.startsWith("\"") && csrfToken.endsWith("\"")) {
            csrfToken = csrfToken.substring(1, csrfToken.length() - 1);
        }
    } else {
        Serial.print("Erro ao obter CSRF Token: ");
        Serial.println(httpResponseCode);
    }

    http.end();
    return csrfToken;
}

void enviaEstadoBotao() {
    static bool estadoBotaoAnt = HIGH;
    static unsigned long debounceBotao;
    
    bool estadoBotao = digitalRead(D1);
    Serial.print("Valor da entrada: ");
    Serial.println(estadoBotao);
    if ((millis() - debounceBotao) > 50) {
      Serial.println(estadoBotao);
        if (estadoBotao != estadoBotaoAnt) {
            debounceBotao = millis();
            if (estadoBotao) {
                MQTT.publish(TOPICO_PUBLISH, "1");
                Serial.println("Botao Apertado Resposta: 1");
            } else {
                MQTT.publish(TOPICO_PUBLISH, "0");
                Serial.println("Botao Solto Resposta: 0");
            }
        }
        estadoBotaoAnt = estadoBotao;
    }

    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        WiFiClient client;
        String url = SERVER_URL;
        http.begin(client, url);

        // Adiciona o token CSRF no cabeçalho
        http.addHeader("Content-Type", "application/json");
        String csrfToken = getCSRFToken();
        http.addHeader("X-CSRFToken", csrfToken);

        String payload = "{\"estado\": \"" + String(estadoBotao ? "1" : "0") + "\"}";
        int httpResponseCode = http.POST(payload);

        if (httpResponseCode > 0) {
            String response = http.getString();
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
            Serial.print("Response: ");
            Serial.println(response);
        } else {
            Serial.print("Erro ao enviar POST: ");
            Serial.println(httpResponseCode);
        }
        http.end();
    } else {
        Serial.println("WiFi desconectado");
    }
}