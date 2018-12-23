#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Module connection pins (Digital Pins)
//#define CLK 2
//#define DIO 0
#define ONE_WIRE_BUS 1
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

int8_t _requestEvent=0;

void i2c_send_addr(void);
union u16bits
{
  int16_t temp;
  uint8_t c[2];

};
uint8_t addr0[8];
uint8_t addr[10][8];
int j = 0;
void oneWireSearchDevices(void)
{


  
  //Serial.println("Looking for 1-Wire devices...");
  while (oneWire.search(addr0)) {
    /* Module 1-Wire découvert ! */
    //Serial.print(F("Found "));

    for (byte i = 0; i < 8; ++i) {
      //if (addr0[i] < 0x10) Serial.write('0');
      addr[j][i] = addr0[i];
      //Serial.print(addr0[i], HEX);
      //Serial.write(' ');
    }
    j++;
 
    if (OneWire::crc8(addr0, 7) != addr0[7]) {
      //Serial.println( "=> CRC is not valid!");
    }
  }

  //Serial.println("Done=");Serial.println(j);
  oneWire.reset_search();

}
void setup() {
  //Serial.begin(115200);
  //Serial.println(F("~~ Scanner 1-Wire ~~"));
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
  Wire.onReceive(receiveEvent);
  
  sensors.begin();
  uint8_t count = 0;
  while (count == 0) {
    count = sensors.getDeviceCount();
    delay(500);
  }
  //Serial.print("count="); Serial.print(" "); Serial.println(count);
  oneWireSearchDevices();
  ///////////////////////////////////////////////////////
  // Protocol definition
  //
  //                      packed struct
  // +----+-----+------+------+---------+-----+
  // | FF | Len | | Type | | CRC |        Frame
  // +----+-----+------+------+---------+-----+
  //   1     1      1         1              Size (byte)
  //         =  \__________ _____/         Frame len
  //      \______________________/  =      Frame CRC

  ////////////////////////////////////////////////////////


  sensors.setResolution(12);
  


}
union u16bits u;
union u16bits temperatur;
int16_t _temp[2];

void loop() {
  
  if (sensors.getDeviceCount() != 0) {
    for (int pp = 0; pp < j; pp++)
    {
    uint8_t r_add[8]={addr[pp][0],addr[pp][1],addr[pp][2],addr[pp][3],addr[pp][4],addr[pp][5],addr[pp][6],addr[pp][7]};
    sensors.requestTemperaturesByAddress(r_add);//requestTemperatures();
    }
    delay(800);
      for (int pp = 0; pp < j; pp++)
    { uint8_t r_add[8]={addr[pp][0],addr[pp][1],addr[pp][2],addr[pp][3],addr[pp][4],addr[pp][5],addr[pp][6],addr[pp][7]};
     double temp0 = sensors.getTempC(r_add);//getTempCByIndex(0);
     u.temp = int(temp0 * 10);
     _temp[pp]=u.temp;
      //Serial.println(u.temp);
    }

   
  }
  //u.temp=10;
}
uint8_t receivedCommands[4]={0,0,0,0};
// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {

switch (_requestEvent)
{
case 1:
i2c_send_nbr_addr();
break;
case 2:
i2c_send_addr(receivedCommands[2]);
break;
case 3:
i2c_send_data(receivedCommands[2]);
break;
}
 //Wire.write(receivedCommands, 4);
//u.temp=10;
 // Wire.write(u.c, 2); // respond with message of 2 bytes
  // as expected by master
}

/////////////////////////////
// Protocol definition
//
//                      packed struct
// +----+-----+------+------+---------+-----+
// | F0 | | Type | | index one wire |   F1     Frame
// +----+-----+------+------+---------+-----+
//   1       1            1              Size (byte)
////////////////////////////////////////////////////////

void receiveEvent(int bytesReceived)
{
  //char receivedCommands[4]={0,0,0,0};
  int a=0;
  while (Wire.available()>=1) {
              //for (int a = 0; a < 4; a++)
                     //      {
         receivedCommands[a] = Wire.read();
         
         
         a++;
                           // }
    }
  //}

   
  if ( receivedCommands[0] == 0xF0) {
if ( receivedCommands[3] == 0xF1) {
    switch (receivedCommands[1]) {

       case 0x0A:// demande les adresses trouvées
       //i2c_send_nbr_addr();
    _requestEvent=1;
        break;
      case 0x0B:// demande les adresses trouvées
      _requestEvent=2;
       // i2c_send_addr(receivedCommands[2]);

        break;

      case 0x0C://demande température du capteur indexé par index
       _requestEvent=3;
       // i2c_send_data(receivedCommands[2]);

        break;

        default:
        _requestEvent=0;
       // i2c_send_nbr_addr();

    }
  }
}

}
////////////
// Protocol definition
//
//          0XDA           packed struct
// +----+-----+------+------+---------+-----+
// | F0 | | Type | | addr one wire | : F1     Frame
// +----+-----+------+------+---------+-----+
//   1       1            8              Size (byte)
////////////////////////////////////////////////////////

void i2c_send_nbr_addr(void) {
 
      uint8_t addrs[4]={0xF0,0xA0,j,0xF1};
      while(_requestEvent==0){}
      Wire.write(addrs, 4);
      _requestEvent=0;
      
  
  };
  
void i2c_send_addr(uint8_t index) {
 //for (int p = 0; p < j; p++)
   // {
      uint8_t p=index;
      uint8_t addrs[11]={0xF0,0xDA,addr[p][0],addr[p][1],addr[p][2],addr[p][3],addr[p][4],addr[p][5],addr[p][6],addr[p][7],0xF1};
      while(_requestEvent==0){}
      Wire.write(addrs, 11);
      _requestEvent=0;
   // }
  
  };
  ////////////////////////////////
void i2c_send_data(uint8_t addrs){
    temperatur.temp=_temp[addrs];
    while(_requestEvent==0){}
   Wire.write(temperatur.c, 2); // respond with message of 2 bytes
    _requestEvent=0;
     
  };
/////////////////
