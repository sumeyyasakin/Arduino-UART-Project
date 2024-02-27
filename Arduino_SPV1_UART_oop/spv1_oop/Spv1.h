#ifndef SPV1_H
#define SPV1_H

#include <Arduino.h>
#include <SoftwareSerial.h>

//(RX TX)PINS
#define RX_PIN 17
#define TX_PIN 16

//Commands
#define CMD_ACTIVATE_ALL           0x83
#define CMD_AUTHENTICATE           0x85
#define CMD_READ_BLOCK             0x86
#define CMD_WRITE_BLOCK            0x89
#define CMD_READ_VALUE_BLOCK       0x87
#define CMD_WRITE_VALUE_BLOCK      0x8A
#define CMD_INCREMENT_VALUE_BLOCK  0x8D
#define CMD_DECREMENT_VALUE_BLOCK  0x8E
#define CMD_WRITE_BLOCK_4_BYTE     0x8B
#define CMD_ACTIVATE_IDLE          0x84
#define CMD_HALT                   0x93
#define CMD_SEEK_FOR_TAG           0X82

// RXSTATE
#define RXSTATE_SOF            0
#define RXSTATE_ADDRESS        1
#define RXSTATE_DATACOUNT_MSB  2
#define RXSTATE_DATACOUNT_LSB  3
#define RXSTATE_COMMAND        4
#define RXSTATE_DATA           5
#define RXSTATE_CHECKSUM       6

//MIN-MAX VALUE
#define MAX_TIMEOUT   5
#define MaxDataCount  528

#define SERIAL_HEADER_BYTE     0xFF

//FLAGS
#define SERIAL_FLAG_RX_IN_PROGRESS 0x01
#define SERIAL_FLAG_COMMAND_READY  0x02


class serial
{
  private:
    byte rx_pin;
    byte tx_pin;

    uint8_t  rx_data[MaxDataCount];
    uint8_t  SerialMaxDataLength ;
    uint8_t  SerialRxState;
    uint16_t SerialReceivedDataCount;
    uint8_t  SerialFlag;
    uint16_t RxSerialTimeOut;

    uint8_t SerialHeaderByte;
    uint8_t SerialDataLength ;
    uint8_t SerialNodeAddress;
    uint8_t SerialCommandByte;
    uint8_t SerialChecksum;
    uint8_t SerialRxDataBuffer[MaxDataCount];


    uint8_t Buf_Activate_All[5];
    uint8_t Buf_Authenticate[7];
    uint8_t Buf_Read_Block[6];
    uint8_t Buf_Write_Block[22];
    uint8_t Buf_Read_Value_Block[6];
    uint8_t Buf_Write_Value_Block[10];
    uint8_t Buf_Increment_Value_Block[10];
    uint8_t Buf_Decrement_Value_Block[10];
    uint8_t Buf_Write_Block_4_Byte[10];
    uint8_t Buf_Seek_For_Tag[5];
    uint8_t Buf_Activate_Idle[5];
    uint8_t Buf_Halt[5];


    uint8_t authenticate_checksum;
    uint8_t read_block_checksum;
    uint8_t write_block_checksum;
    uint8_t read_value_block_checksum;
    uint8_t write_value_block_checksum;
    uint8_t increment_value_block_checksum;
    uint8_t decrement_value_block_checksum;
    uint8_t write_block_4_byte_checksum;
    

  public:
    serial() {} // do not use


    serial (int, int);
    uint8_t Write_Block [16] ;
    uint8_t Write_Value_Block[4];
    uint8_t Increment_Value_Block[4];
    uint8_t Decrement_Value_Block [4];
    uint8_t Write_Block_4_Byte[4];
    void baudrate(uint8_t* baudrate_value);
    void init();
    void Command_Control();
    void Func_ActivateAll();
    void Func_Authenticate();
    void Func_ReadBlock();
    void Func_WriteBlock();
    void Func_ReadValueBlock();
    void Func_WriteValueBlock();
    void Func_IncrementValueBlock();
    void Func_DecrementValueBlock();
    void Func_SeekForTag();
    void Func_ActivateIdle();
    void Func_Halt();
    void uart_rx_event(uint8_t rx_data[MaxDataCount]);
    void CmdActivateAll();
    void CmdAuthenticate(uint8_t authenticate_block_no);
    void CmdReadBlock(uint8_t read_block_no);
    void CmdWriteBlock(uint8_t write_block_no, uint8_t* Write_Block_No);
    void CmdReadValueBlock(uint8_t read_value_block_no);
    void CmdWriteValueBlock(uint8_t write_value_block_no , uint8_t* Write_Value_Block);
    void CmdIncrementValueBlock(uint8_t increment_value_block_no , uint8_t* Increment_Value_Block);
    void CmdDecrementValueBlock(uint8_t decrement_value_block_no , uint8_t* Decrement_Value_Block);
    void CmdWriteBlock4Byte(uint8_t write_block_4_byte_no, uint8_t* Write_Block_4_Byte);
    void Func_WriteBlock4Byte();
    void CmdSeekForTag();
    void CmdActivateIdle();
    void CmdHalt();


};

#endif
