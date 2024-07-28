#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <PubSubClient.h>

// Definições
#define TOPICO_SUBSCRIBE "Alimentador_TccRecebe"
#define TOPICO_PUBLISH   "Alimentador_TccEnvio"
#define ID_MQTT  "Rakel"

// Pinagem do NodeMCU
#define LED_PIN    14 // D5 (GPIO14)
#define LED_PIN2    12 // D6 (GPIO12)
#define BUTTON1    05 // D1 (GPIO05)
#define BUTTON2    04 // D2 (GPIO04)

// Configuração da rede WiFi
const char* SSID = "LUDU";
const char* PASSWORD = "12111977D@m";

// URL do servidor Django para enviar dados
const char* SERVER_URL = "http://192.168.15.30:8000/update_button/";

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
void enviaEstadoBotao(int button, const String& estado);

void setup() {
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    pinMode(LED_PIN2, OUTPUT);
    digitalWrite(LED_PIN2, LOW);

    pinMode(BUTTON1, INPUT);
    pinMode(BUTTON2, INPUT);

    Serial.begin(115200);
    initWiFi();
    initMQTT();
}

void loop() {
    VerificaConexoesWiFIEMQTT();
    MQTT.loop();
    
    static bool estadoBotaoAnt1 = HIGH;
    static bool estadoBotaoAnt2 = HIGH;
    static unsigned long debounceBotao1;
    static unsigned long debounceBotao2;
    
    bool estadoBotao1 = digitalRead(BUTTON1);
    bool estadoBotao2 = digitalRead(BUTTON2);

    if ((millis() - debounceBotao1) > 50) {
        if (estadoBotao1 != estadoBotaoAnt1) {
            debounceBotao1 = millis();
            if (estadoBotao1) {
                enviaEstadoBotao(1, "botao1Pressionado");
            } else {
                enviaEstadoBotao(1, "botao1Solto");
            }
        }
        estadoBotaoAnt1 = estadoBotao1;
    }

    if ((millis() - debounceBotao2) > 50) {
        if (estadoBotao2 != estadoBotaoAnt2) {
            debounceBotao2 = millis();
            if (estadoBotao2) {
                enviaEstadoBotao(2, "botao2Pressionado");
            } else {
                enviaEstadoBotao(2, "botao2Solto");
            }
        }
        estadoBotaoAnt2 = estadoBotao2;
    }

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
    if (msg == "ligar2") {
        Serial.println("Recebeu Comando: Ligar2");
        digitalWrite(LED_PIN2, HIGH); 
    } else if (msg == "desligar2") {
        Serial.println("Recebeu Comando: Desligar2"); 
        digitalWrite(LED_PIN2, LOW);
    }
}

String getCSRFToken() {
    HTTPClient http;
    WiFiClient client;

    String url = "http://192.168.15.30:8000/get_csrf_token/";
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

void enviaEstadoBotao(int button, const String& estado) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        WiFiClient client;
        String url = SERVER_URL;
        http.begin(client, url);

        // Adiciona o token CSRF no cabeçalho
        http.addHeader("Content-Type", "application/json");
        String csrfToken = getCSRFToken();
        http.addHeader("X-CSRFToken", csrfToken);

        // Cria o payload JSON corretamente formatado
        String payload = "{\"botao\": " + String(button) + ", \"estado\": \"" + estado + "\"}";
        Serial.println("Payload enviado:");
        Serial.println(payload);

        // Envia a requisição POST
        int httpResponseCode = http.POST(payload);
        String response = http.getString(); // Captura a resposta

        // Exibe o código de resposta HTTP e a resposta
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        Serial.print("Response: ");
        Serial.println(response);

        if (httpResponseCode == HTTP_CODE_OK) {
            Serial.println("Requisição POST bem-sucedida");
        } else if (httpResponseCode == HTTP_CODE_MOVED_PERMANENTLY || httpResponseCode == HTTP_CODE_FOUND) {
            Serial.println("Redirecionamento detectado.");
        } else {
            Serial.print("Erro ao enviar POST: ");
            Serial.println(httpResponseCode);
        }
        http.end();
    } else {
        Serial.println("WiFi desconectado");
    }
}
