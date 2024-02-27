#include "Spv1.h"
 
//Arduino uno da UART event çalışmıyor. (newSerial)
//Arduino mega da Serial2 pinlerine bağlanılarak UART event haberleşmesi yapıldı.( TX2: 16, RX2: 17)
//Test edilmek isteniyorsa arduino mega da çalışıyor.

serial serial;
uint8_t Write_Block [16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
uint8_t Write_Value_Block[4] = {0x0C, 0x00, 0x00, 0x00};
uint8_t Increment_Value_Block[4] = {0x02,0x00,0x00,0x00};
uint8_t Decrement_Value_Block[4] = {0x02,0x00,0x00,0x00};
uint8_t Write_Block_4_Byte[4];

void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(19200);
  Serial2.begin(19200);
  //serial.baudrate(19200);
  serial.init();
  serial.CmdActivateAll();
  delay(100);
  serial.CmdAuthenticate(8);
  delay(100);

}

void loop() {
  // put your main code here, to run repeatedly:

}


void serialEvent2()
{
  uint8_t rx_data;
  while(Serial2.available() > 0)
  {
    rx_data = Serial2.read();
    serial.uart_rx_event(rx_data);
  }
}
