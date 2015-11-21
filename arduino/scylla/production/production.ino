// temp and humi (blue and white brick)
#include <dht.h>

// waterproof temp
#include <OneWire.h>
#include <DallasTemperature.h>

// Ethernet
#include <SPI.h>
#include <Ethernet.h>

// strings
#include <string.h>

// room temp and humi
#define DHT11_PIN 6
#define DHT22_PIN 7
#define ROOM_WINDOW 2

// waterproof temp
#define OUTSIDE_STREET_TEMP 3

#define ROOM_LIGHT A5

#define BOARD_LED 13

// init libs
dht DHT;
OneWire oneWire(OUTSIDE_STREET_TEMP);

DallasTemperature sensors(&oneWire);

// assign a MAC address for the ethernet controller.
// fill in your address here:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
// fill in an available IP address on your network here,
// for manual configuration:
IPAddress ip(192, 168, 1, 120);

// fill in your Domain Name Server address here:
IPAddress myDns(192, 168, 1, 1);

// initialize the library instance:
EthernetClient client;

char server[] = "192.168.1.100";

unsigned long lastConnectionTime = 0;             // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 10L * 1000L; // delay between updates, in milliseconds
// the "L" is needed to use long type numbers

void setup() {
  Serial.begin(9600);

  pinMode(ROOM_WINDOW, INPUT); //INPUT_PULLUP
  digitalWrite(ROOM_WINDOW, HIGH);

  // internal LED
  pinMode(BOARD_LED, OUTPUT);

  // turn off LED
  digitalWrite(BOARD_LED, LOW);

  sensors.begin();
  
   // give the ethernet module time to boot up:
  delay(1000);
  // start the Ethernet connection using a fixed IP address and DNS server:
  Ethernet.begin(mac, ip, myDns);
}

int ticks;

int chk;
int room_window_state = 0;
double room_light = .0;
double outside_street_temp = .0;
double outside_laundry_temp = .0;
double outside_laundry_hum = .0;
double room_humidity = .0;
double room_temp = .0;


void loop() {

  // ***********************************************
  // Room temperature and humidity
  chk = DHT.read11(DHT11_PIN);

  switch (chk)
  {
    case DHTLIB_OK:
      room_humidity += DHT.humidity;
      room_temp += DHT.temperature;
      break;
    case DHTLIB_ERROR_CHECKSUM:
      Serial.print("Checksum error,\t");
      break;
    case DHTLIB_ERROR_TIMEOUT:
      Serial.print("Time out error,\t");
      break;
    default:
      Serial.print("Unknown error,\t");
      break;
  }

  // ***********************************************
  // Outside laundry temp and hum
  chk = DHT.read22(DHT22_PIN);

  switch (chk)
  {
    case DHTLIB_OK:
      outside_laundry_hum += DHT.humidity;
      outside_laundry_temp += DHT.temperature;
      break;
    case DHTLIB_ERROR_CHECKSUM:
      Serial.print("Checksum error,\t");
      break;
    case DHTLIB_ERROR_TIMEOUT:
      Serial.print("Time out error,\t");
      break;
    default:
      Serial.print("Unknown error,\t");
      break;
  }

  // ***********************************************
  // Window state
  room_window_state += digitalRead(ROOM_WINDOW);

  // ***********************************************
  // Light sensor
  room_light += analogRead(ROOM_LIGHT);

  // ***********************************************
  // Street temp
  sensors.requestTemperatures();
  outside_street_temp += sensors.getTempCByIndex(0);

  // ***********************************************

  ticks++;
  if (ticks == 60) {
    
    sendData("Kladno-room-humidity", String(room_humidity / ticks));  
    Serial.println("Room humidity: " + String(room_humidity / ticks));
    
    sendData("Kladno-room-temperature", String(room_temp / ticks));  
    Serial.println("Room temperature " +  String(room_temp / ticks));
    
    sendData("Kladno-outside_laundry-humidity", String(outside_laundry_hum / ticks));  
    Serial.println("Outside laundry humidity: " + String(outside_laundry_hum / ticks));
    
    sendData("Kladno-outside_laundry-temperature", String(outside_laundry_temp / ticks));  
    Serial.println("Outside laundry temperature " +  String(outside_laundry_temp / ticks));

    if (room_window_state / ticks > .5) {
      sendData("Kladno-room-window_state", "1");  
      Serial.println("Window open");
    } else {
      sendData("Kladno-room-window_state", "0");  
      Serial.println("Window closed");
    }
    
    // convert to relative
    room_light = room_light / 1023;
    sendData("Kladno-room-light", String(room_light / ticks, 8));  
    Serial.println("Room light: " + String(room_light / ticks, 8));

    sendData("Kladno-outside_street-temperature", String(outside_street_temp / ticks));  
    Serial.println("Street temp: " + String(outside_street_temp / ticks));
    
    Serial.println();  

    room_window_state = 0;
    room_light = 0;
    room_humidity = .0;
    room_temp = .0;
    outside_street_temp = .0;
    outside_laundry_hum = .0;
    outside_laundry_temp = .0;

    ticks = 0;
  }

  delay(1000);
}


void sendData(String key, String value) {
  // close any connection before send a new request.
  client.stop();
  
  String PostData = "{\"key\": \"" + key + "\", \"value\": \"" + value + "\"}";
  
  // if there's a successful connection:
  if (client.connect(server, 8000)) {
    //Serial.println("connecting...");
    // send the HTTP POST request:
    client.println("POST /api/add-log HTTP/1.1");
    //client.println("Host: www.arduino.cc");
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: close");
    client.println("Content-Type: application/json;");
    client.print("Content-Length: ");
    client.println(PostData.length());
    client.println();
    client.println(PostData);    
  }
  else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }
}

