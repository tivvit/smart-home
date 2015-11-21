// temp and humi (blue and white brick)
#include <dht.h>

// temp
#include <OneWire.h>

// strings
#include <string.h>

// Ethernet
#include <EtherCard.h>

// define sensor positions
#define KITCHEN_TEMP 2
#define KITCHEN_LIGHT 5
#define KITCHEN_TEMP_HUMIDITY_DHT11 3
#define BOARD_LED 13

#define SEND true

// init libs
dht DHT;
OneWire room_temp_sensor(KITCHEN_TEMP);

// ethernet interface mac address, must be unique on the LAN
byte mymac[] = { 0x74, 0x69, 0x69, 0x2D, 0x30, 0x33 };

static byte session;

byte Ethernet::buffer[700];

static byte server[] = { 10, 0, 0, 1 };

void setup () {
  Serial.begin(9600);

  // internal LED
  pinMode(BOARD_LED, OUTPUT);

  // turn off LED
  digitalWrite(BOARD_LED, LOW);

  if (SEND) {
    if (ether.begin(sizeof Ethernet::buffer, mymac) == 0)
      Serial.println(F("Failed to access Ethernet controller"));
    if (!ether.dhcpSetup())
      Serial.println(F("DHCP failed"));

    ether.printIp("IP:  ", ether.myip);
    ether.printIp("GW:  ", ether.gwip);
    ether.printIp("DNS: ", ether.dnsip);

    //if (!ether.dnsLookup(website))
    //  Serial.println(F("DNS failed"));

    //static byte hisip[] = { 37,157,195,170 };

    ether.copyIp(ether.hisip, server);
    ether.hisport = 8000;

    ether.printIp("SRV: ", ether.hisip);
  }
}

int ticks;
int tick_cnt = 60;

int sent = 0;
boolean prepared = false;

int send_ticks = 0;

int chk;

double room_light = .0;
double room_humidity = .0;
double reference_room_temp = .0;
double actual_room_temp = .0;

double frozen_room_light = .0;
double frozen_room_humidity = .0;
double frozen_reference_room_temp = .0;
double frozen_actual_room_temp = .0;

uint32_t timer;

void loop() {

  if (millis() > timer) {

    timer = millis() + 1000;

    actual_room_temp += getTemp(room_temp_sensor);

    // ***********************************************
    // Room temperature and humidity
    chk = DHT.read11(KITCHEN_TEMP_HUMIDITY_DHT11);

    switch (chk)
    {
      case DHTLIB_OK:
        room_humidity += DHT.humidity;
        reference_room_temp += DHT.temperature;
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
    // Light sensor
    room_light += analogRead(KITCHEN_LIGHT);

    // ***********************************************

    ticks++;
    if (ticks == tick_cnt) {
      prepared = true;

      ticks = 0;

      // convert to relative
      room_light = room_light / 1023;
      // invert
      room_light = 1 - (room_light / tick_cnt);

      frozen_room_light = room_light;
      frozen_room_humidity = room_humidity / tick_cnt;
      frozen_reference_room_temp = reference_room_temp / tick_cnt;
      frozen_actual_room_temp = actual_room_temp / tick_cnt;

      room_humidity = .0;
      actual_room_temp = .0;
      reference_room_temp = .0;
      room_light = .0;
    }
  }

  if (prepared) {
    //Serial.println(sent);

    if (send_ticks == 0) {

      switch (sent)
      {
        //Serial.println("sending !");
        case 0:
          sendData("flat-kitchen-humidity", String(frozen_room_humidity));
          //Serial.println("Room humidity: " + String(room_humidity / tick_cnt));
          break;
        case 1:
          sendData("flat-kitchen-temp", String(frozen_actual_room_temp));
          //Serial.println("Room temperature " +  String(actual_room_temp / tick_cnt));
          break;
        case 2:
          sendData("flat-kitchen-ref_temp", String(frozen_reference_room_temp));
          //Serial.println("Room reference temperature: " +  String(reference_room_temp / tick_cnt));
          break;
        case 3:
          sendData("flat-kitchen-light", String(frozen_room_light, 8));
          //Serial.println("Room light: " + String(room_light, 8));

          sent = -1;
          send_ticks = 1; // because of --
          prepared = false;

          break;
      }

      sent++;
      send_ticks == 20;
    }

    send_ticks--;

  }

  if (SEND) {
    ether.packetLoop(ether.packetReceive());
  }
}


int freeRam ()
{
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void sendData(String key, String value) {
  String PostData = "{\"key\": \"" + key + "\", \"value\": \"" + value + "\"}";

  Serial.println(PostData);
  
  if (SEND) {
    Stash stash;
    byte sd = stash.create();

    stash.println(PostData);
    stash.save();

    // Compose the http POST request, taking the headers below and appending
    // previously created stash in the sd holder.
    Stash::prepare(PSTR("POST /api/add-log HTTP/1.1" "\r\n"
                        "Connection: close" "\r\n"
                        "Content-Type: application/json;" "\r\n"
                        "Content-Length: $D" "\r\n"
                        "\r\n"
                        "$H"),
                   stash.size(), sd
                  );

    // send the packet - this also releases all stash buffers once done
    // Save the session ID so we can watch for it in the main loop.
    session = ether.tcpSend();
  }
}

float getTemp(OneWire sensor) {
  //returns the temperature from one DS18S20 in DEG Celsius

  byte data[12];
  byte addr[8];

  if ( !sensor.search(addr)) {
    //no more sensors on chain, reset search
    sensor.reset_search();
    return -1000;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
    Serial.println("CRC is not valid!");
    return -1000;
  }

  if ( addr[0] != 0x10 && addr[0] != 0x28) {
    Serial.print("Device is not recognized");
    return -1000;
  }

  sensor.reset();
  sensor.select(addr);
  sensor.write(0x44, 1); // start conversion, with parasite power on at the end

  byte present = sensor.reset();
  sensor.select(addr);
  sensor.write(0xBE); // Read Scratchpad


  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = sensor.read();
  }

  sensor.reset_search();

  byte MSB = data[1];
  byte LSB = data[0];

  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;

  return TemperatureSum;

}
