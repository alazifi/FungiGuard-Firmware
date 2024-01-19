#include <WiFi.h>
#include <HTTPClient.h>

const char *ssid = "IEG-1";
const char *password = "evergreen999";
const char *thingsboardServer = "iot.abc-server.id";
const String accessToken = "XjsnxxhgwkOcCKIX7yPl";

void setup()
{
  Serial.begin(115200);

  // Koneksi WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

bool sendDataToThingsBoard()
{
  HTTPClient http;

  // Buat URL untuk mengirimkan data ke ThingsBoard
  String url = "https://" + String(thingsboardServer) + "/api/v1/" + String(accessToken) + "/telemetry";

  // Buat payload data (ganti sesuai dengan data yang ingin Anda kirim)
  String payload = "{\"temperature\": 25.5, \"humidity\": 60.0}";

  // Mulai koneksi HTTPS tanpa sidik jari sertifikat
  http.begin(url);

  // Atur header HTTP
  http.addHeader("Content-Type", "application/json");

  // Kirim data
  int httpResponseCode = http.POST(payload);

  // Cek status respon
  if (httpResponseCode > 0)
  {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    http.end();
    return true;
  }
  else
  {
    Serial.print("HTTP Error code: ");
    Serial.println(httpResponseCode);
    http.end();
    return false;
  }
}

void loop()
{

  // Kirim data ke ThingsBoard
  if (sendDataToThingsBoard())
  {
    Serial.println("Data sent successfully");
  }
  else
  {
    Serial.println("Failed to send data");
  }
}
