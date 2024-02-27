#include "Spv1.h"

//SoftwareSerial newSerial(RX_PIN, TX_PIN);

serial::serial (int, int)
{
  rx_pin = RX_PIN;
  tx_pin = TX_PIN;
}

void serial::baudrate(uint8_t* baudrate_value)
{
  Serial2.begin(baudrate_value);
}

void serial::init()
{
  SerialNodeAddress = 0x00;
  SerialRxState = RXSTATE_SOF;
  SerialChecksum = 0;
  SerialReceivedDataCount = 0;
  SerialFlag = 0;
}

void serial::uart_rx_event(uint8_t rx_data[MaxDataCount])
{

  //RxSerialTimeOut = MAX_TIMEOUT;
  SerialFlag = SERIAL_FLAG_RX_IN_PROGRESS;

  switch (SerialRxState)
  {
    case RXSTATE_SOF:
      {
        if (rx_data == SERIAL_HEADER_BYTE)
        {
          SerialHeaderByte = rx_data;
          SerialChecksum = 0;
          SerialReceivedDataCount = 0;
          SerialRxState = RXSTATE_ADDRESS;

        }
        break;
      }

    case RXSTATE_ADDRESS:
      {
        if ((rx_data == 0) || (rx_data == SerialNodeAddress))
        {
          SerialNodeAddress = rx_data;
          SerialChecksum = rx_data;
          SerialRxState = RXSTATE_DATACOUNT_LSB;
        }
        else
        {
          SerialRxState = RXSTATE_SOF;
        }

        break;
      }
    case RXSTATE_DATACOUNT_LSB:
      {
        SerialDataLength = rx_data;
        if ((SerialDataLength > MaxDataCount + 1) || (SerialDataLength == 0))
        {
          SerialRxState = RXSTATE_SOF;
        }
        else
        {
          SerialChecksum += rx_data;
          SerialRxState = RXSTATE_COMMAND;
        }
        break;
      }
    case RXSTATE_COMMAND:
      {
        SerialCommandByte = rx_data;
        SerialReceivedDataCount = 0;
        SerialChecksum += rx_data;

        if (SerialDataLength == 1)
        {
          SerialRxState = RXSTATE_CHECKSUM;
        }
        else
        {
          SerialRxState = RXSTATE_DATA;
        }
        break ;
      }
    case RXSTATE_DATA:
      {
        SerialRxDataBuffer[SerialReceivedDataCount++] = rx_data;
        SerialChecksum += rx_data;

        if (SerialReceivedDataCount == SerialDataLength - 1)
        {
          SerialRxState = RXSTATE_CHECKSUM;
        }

        break;
      }
    case RXSTATE_CHECKSUM:
      {
        if (SerialChecksum == rx_data)
        {
          SerialRxState = RXSTATE_SOF;
          SerialFlag = SERIAL_FLAG_COMMAND_READY;
          Command_Control();
        }
        else
        {
          SerialRxState = RXSTATE_SOF;
        }

        break;
      }

    default:
      {
        SerialRxState = RXSTATE_SOF;

        break ;
      }
  }
}

void serial::Command_Control()
{
  if (SerialFlag == SERIAL_FLAG_COMMAND_READY)
  {
    if (SerialCommandByte == CMD_ACTIVATE_ALL)
    {
      Func_ActivateAll();
    }
    if (SerialCommandByte == CMD_AUTHENTICATE)
    {
      Func_Authenticate();
    }
    if (SerialCommandByte == CMD_READ_BLOCK)
    {
      Func_ReadBlock();
    }
    if(SerialCommandByte == CMD_WRITE_BLOCK)
    {
      Func_WriteBlock();
    }
    if (SerialCommandByte == CMD_READ_VALUE_BLOCK)
    {
      Func_ReadValueBlock();
    }
    if (SerialCommandByte == CMD_WRITE_VALUE_BLOCK)
    {
      Func_WriteValueBlock();
    }
    if (SerialCommandByte == CMD_INCREMENT_VALUE_BLOCK)
    {
      Func_IncrementValueBlock();
    }
    if(SerialCommandByte == CMD_DECREMENT_VALUE_BLOCK)
    {
      Func_DecrementValueBlock();
    }
    if(SerialCommandByte == CMD_WRITE_BLOCK_4_BYTE)
    {
      Func_WriteBlock4Byte();
    }
    if (SerialCommandByte == CMD_SEEK_FOR_TAG)
    {
      Func_SeekForTag();
    }
    if (SerialCommandByte == CMD_ACTIVATE_IDLE)
    {
      Func_ActivateIdle();
    }
    if (SerialCommandByte == CMD_HALT)
    {
      Func_Halt();
    }


  }
}


void serial::CmdActivateAll()
{
  Buf_Activate_All[0] = 0xFF;
  Buf_Activate_All[1] = 0x00;
  Buf_Activate_All[2] = 0x01;
  Buf_Activate_All[3] = 0x83;
  Buf_Activate_All[4] = 0x84;

  Serial2.write(Buf_Activate_All, 5);
}

void serial::Func_ActivateAll()
{
  if (SerialRxDataBuffer[0] == 0x4E)
  {
    Serial.println(F("No Tag"));
  }

  else if ((SerialDataLength == 6) || (SerialDataLength == 9))
  {
    Serial.print(F("Tag_Type: "));
    Serial.print(SerialRxDataBuffer[0], HEX);
    Serial.print(F(" "));
    int uidlength = SerialDataLength - 2;

    Serial.print(F("UID Length:"));
    Serial.print(uidlength);
    Serial.print(F(" , UID:"));
    for (int i = 0; i < uidlength; i++)
    {
      Serial.print(SerialRxDataBuffer[uidlength - i], HEX);
      Serial.print(F("-"));
    }
    Serial.print(F("\r\n"));
  }

}

void serial::CmdAuthenticate(uint8_t authenticate_block_no)
{

  Buf_Authenticate[0] = 0xFF;
  Buf_Authenticate[1] = 0x00;
  Buf_Authenticate[2] = 0x03;
  Buf_Authenticate[3] = 0x85;
  Buf_Authenticate[4] = authenticate_block_no;
  Buf_Authenticate[5] = 0xFF;

  for (int i = 1 ; i < sizeof(Buf_Authenticate) - 1 ; i++)
  {
    authenticate_checksum = Buf_Authenticate[i] + authenticate_checksum;
  }

  Buf_Authenticate[6] = authenticate_checksum;
  Serial2.write (Buf_Authenticate, 7);
}

void serial::Func_Authenticate()
{
  Serial.print(SerialHeaderByte, HEX);
  Serial.print(SerialNodeAddress, HEX);
  Serial.print(SerialDataLength, HEX);
  Serial.print(SerialCommandByte, HEX);
  for (int k = 0; k < SerialDataLength - 1 ;  k++)
  {
    Serial.print(SerialRxDataBuffer[k], HEX);
  }
  Serial.print(SerialChecksum, HEX);
  Serial.print(F("\r\n"));
}


void serial::CmdReadBlock(uint8_t read_block_no)
{
  Buf_Read_Block[0] = 0xFF;
  Buf_Read_Block[1] = 0x00;
  Buf_Read_Block[2] = 0x02;
  Buf_Read_Block[3] = 0x86;
  Buf_Read_Block[4] = read_block_no;

  for (int i = 1 ; i < sizeof(Buf_Read_Block) - 1 ; i++)
  {
    read_block_checksum = Buf_Read_Block[i] + read_block_checksum;
  }
  Buf_Read_Block[5] = read_block_checksum;

  Serial2.write (Buf_Read_Block, 6);
}

void serial::Func_ReadBlock()
{
  Serial.print(F("Mifare Card Block no:"));
  Serial.print(SerialRxDataBuffer[0], HEX);
  Serial.print(F(" "));
  Serial.print(SerialHeaderByte, HEX);
  Serial.print(SerialNodeAddress, HEX);
  Serial.print(SerialDataLength, HEX);
  Serial.print(SerialCommandByte, HEX);

  for (int  i = 0; i < SerialDataLength - 1 ; i++)
  {
    Serial.print(SerialRxDataBuffer[i], HEX);
  }
  Serial.print(SerialChecksum, HEX);
  Serial.print(F("\r\n"));

  if (SerialRxDataBuffer[0] == 0x4E)
  {
    Serial.print(F("0x4E ('N'):No Tag or operation failed."));
  }
  if (SerialRxDataBuffer[0] == 0x46)
  {
    Serial.print(F("0x46 ('F'): No Tag or operation failed."));
  }
  if (SerialRxDataBuffer[0] == 0x55)
  {
    Serial.print(F("0x55 ('U'): Operation failed. The Antenna power was switched off."));
  }
}

void serial::CmdWriteBlock(uint8_t write_block_no, uint8_t* Write_Block_No)
{
  Buf_Write_Block[0] = 0xFF;
  Buf_Write_Block[1] = 0x00;
  Buf_Write_Block[2] = 0x12;
  Buf_Write_Block[3] = 0x89;
  Buf_Write_Block[4] = write_block_no;

  memcpy(&Buf_Write_Block[5], &Write_Block , sizeof(Write_Block));

  for (int i = 1 ; i < sizeof(Buf_Write_Block) - 1 ; i++)
  {
    write_block_checksum = Buf_Write_Block[i] + write_block_checksum;
  }

  Buf_Write_Block[21] = write_block_checksum;
}

void serial::Func_WriteBlock()
{
  Serial.print(F("Mifare Card Block no:"));
  Serial.print(SerialRxDataBuffer[0], HEX);
  Serial.print(F(" "));
  Serial.print(SerialHeaderByte, HEX);
  Serial.print(SerialNodeAddress, HEX);
  Serial.print(SerialDataLength, HEX);
  Serial.print(SerialCommandByte, HEX);

  for (int  i = 0; i < SerialDataLength - 1 ; i++)
  {
    Serial.print(SerialRxDataBuffer[i], HEX);
  }
  Serial.print(SerialChecksum, HEX);
  Serial.print(F("\r\n"));

  if (SerialRxDataBuffer[0] == 0x4E)
  {
    Serial.print(F("0x4E ('N'):No Tag or operation failed."));
  }
  if (SerialRxDataBuffer[0] == 0x46)
  {
    Serial.print(F("0x46 ('F'): No Tag or operation failed."));
  }
  if (SerialRxDataBuffer[0] == 0x55)
  {
    Serial.print(F("0x55 ('U'): Read after write failed."));
  }
  if (SerialRxDataBuffer[0] == 0x58)
  {
    Serial.print(F("0x58 ('X'): Unable to read after write."));
  }
}

void serial::CmdReadValueBlock(uint8_t read_value_block_no)
{
  Buf_Read_Value_Block[0] = 0xFF;
  Buf_Read_Value_Block[1] = 0x00;
  Buf_Read_Value_Block[2] = 0x02;
  Buf_Read_Value_Block[3] = 0x87;
  Buf_Read_Value_Block[4] = read_value_block_no;

  for (int i = 1 ; i < sizeof(Buf_Read_Value_Block) - 1 ; i++)
  {
    read_value_block_checksum = Buf_Read_Value_Block[i] + read_value_block_checksum;
  }
  Buf_Read_Value_Block[5] = read_value_block_checksum;

  Serial2.write(Buf_Read_Value_Block , 6);
}

void serial::Func_ReadValueBlock()
{
  Serial.print(F("The value is "));
  Serial.print(SerialRxDataBuffer[1], DEC);
  Serial.print(F(" Mifare Card Block no = "));
  Serial.print(SerialRxDataBuffer[0], HEX);
  Serial.print(F(" "));
  Serial.print(SerialHeaderByte, HEX);
  Serial.print(SerialNodeAddress, HEX);
  Serial.print(SerialDataLength, HEX);
  Serial.print(SerialCommandByte, HEX);

  for (int  i = 0; i < SerialDataLength - 1 ; i++)
  {
    Serial.print(SerialRxDataBuffer[i], HEX);
  }
  Serial.print(SerialChecksum, HEX);
  Serial.print(F("\r\n"));

  if (SerialRxDataBuffer[0] == 0x4E)
  {
    Serial.println(F("0x4E (‘N’): No Tag or operation failed"));
  }
  if (SerialRxDataBuffer[0] == 0x46)
  {
    Serial.println(F("0x46 (‘F’): No Tag or operation failed"));
  }
  if (SerialRxDataBuffer[0] == 0x49)
  {
    Serial.println(F("0x49 (‘I’): Invalid Value Block. The block was not in the proper value format."));
  }
}

void serial::CmdWriteValueBlock(uint8_t write_value_block_no , uint8_t* Write_Value_Block)
{
  Buf_Write_Value_Block[0] = 0xFF;
  Buf_Write_Value_Block[1] = 0x00;
  Buf_Write_Value_Block[2] = 0x06;
  Buf_Write_Value_Block[3] = 0x8A;
  Buf_Write_Value_Block[4] = write_value_block_no;

  memcpy(Buf_Write_Value_Block[5], &Write_Value_Block , sizeof(Write_Value_Block));
  for (int i = 1 ; i < sizeof(Buf_Write_Value_Block) - 1 ; i++)
  {
    write_value_block_checksum = Buf_Write_Value_Block[i] + write_value_block_checksum;
  }

  Buf_Write_Value_Block[9] = write_value_block_checksum;
  Serial2.write (Buf_Write_Value_Block, 10);
}

void serial::Func_WriteValueBlock()
{
  Serial.print(F("Mifare Card Block No:"));
  Serial.print(SerialRxDataBuffer[0], HEX);
  Serial.print(F(" "));
  Serial.print(SerialHeaderByte, HEX);
  Serial.print(SerialNodeAddress, HEX);
  Serial.print(SerialDataLength, HEX);
  Serial.print(SerialCommandByte, HEX);

  for (int  i = 0; i < SerialDataLength - 1 ; i++)
  {
    Serial.print(SerialRxDataBuffer[i], HEX);
  }
  Serial.print(SerialChecksum, HEX);
  Serial.print(F("\r\n"));

  if (SerialRxDataBuffer[0] == 0x4E)
  {
    Serial.print(F("0x4E ('N'):No Tag or operation failed."));
  }
  if (SerialRxDataBuffer[0] == 0x46)
  {
    Serial.print(F("0x46 ('F'): No Tag or operation failed."));
  }
  if (SerialRxDataBuffer[0] == 0x55)
  {
    Serial.print(F("0x55 ('U'): Read after write failed (Expected response for sector trailer block write)*"));
  }
  if (SerialRxDataBuffer[0] == 0x58)
  {
    Serial.print(F("Unable to read after write"));
  }
}


void serial::CmdIncrementValueBlock(uint8_t increment_value_block_no , uint8_t* Increment_Value_Block)
{
  Buf_Increment_Value_Block[0] = 0xFF;
  Buf_Increment_Value_Block[1] = 0x00;
  Buf_Increment_Value_Block[2] = 0x06;
  Buf_Increment_Value_Block[3] = 0x8D;
  Buf_Increment_Value_Block[4] = increment_value_block_no;

  memcpy(Buf_Increment_Value_Block[5], &Increment_Value_Block , sizeof(Increment_Value_Block));
  for (int i = 1 ; i < sizeof(Buf_Increment_Value_Block) - 1 ; i++)
  {
    increment_value_block_checksum = Buf_Increment_Value_Block[i] + increment_value_block_checksum;
  }

  Buf_Increment_Value_Block[9] = increment_value_block_checksum;

  Serial2.write (Buf_Increment_Value_Block, 10);
}

void serial::Func_IncrementValueBlock()
{
  Serial.print(F("Mifare Card Block no:"));
  Serial.print(SerialRxDataBuffer[0], HEX);
  Serial.print(F(" "));
  Serial.print(SerialHeaderByte, HEX);
  Serial.print(SerialNodeAddress, HEX);
  Serial.print(SerialDataLength, HEX);
  Serial.print(SerialCommandByte, HEX);

  for (int  i = 0; i < SerialDataLength - 1 ; i++)
  {
    Serial.print(SerialRxDataBuffer[i], HEX);
  }
  Serial.print(SerialChecksum, HEX);
  Serial.print(F("\r\n"));

  if (SerialRxDataBuffer[0] == 0x4E)
  {
    Serial.print(F("0x4E ('N'):No Tag or operation failed."));
  }
  if (SerialRxDataBuffer[0] == 0x46)
  {
    Serial.print(F("0x46 ('F'): No Tag or operation failed."));
  }

  if (SerialRxDataBuffer[0] == 0x49)
  {
    Serial.print(F("0x49 ('I'): Invalid Value Block. The block was not in the proper value format when read back."));
  }
}

void serial::CmdDecrementValueBlock(uint8_t decrement_value_block_no , uint8_t* Decrement_Value_Block)
{
  Buf_Decrement_Value_Block[0] = 0xFF;
  Buf_Decrement_Value_Block[1] = 0x00;
  Buf_Decrement_Value_Block[2] = 0x06;
  Buf_Decrement_Value_Block[3] = 0x8E;
  Buf_Decrement_Value_Block[4] = decrement_value_block_no;

  memcpy(&Buf_Decrement_Value_Block[5], &Decrement_Value_Block , sizeof(Decrement_Value_Block));
  for (int i = 1 ; i < sizeof(Buf_Decrement_Value_Block) - 1 ; i++)
  {
    decrement_value_block_checksum = Buf_Decrement_Value_Block[i] + decrement_value_block_checksum;
  }
  Buf_Decrement_Value_Block[9] = decrement_value_block_checksum;
  Serial2.write (Buf_Decrement_Value_Block, 10);
}

void serial::Func_DecrementValueBlock()
{
  Serial.print(F("Mifare Card Block no:"));
  Serial.print(SerialRxDataBuffer[0], HEX);
  Serial.print(F(" "));
  Serial.print(SerialHeaderByte, HEX);
  Serial.print(SerialNodeAddress, HEX);
  Serial.print(SerialDataLength, HEX);
  Serial.print(SerialCommandByte, HEX);

  for (int  i = 0; i < SerialDataLength - 1 ; i++)
  {
    Serial.print(SerialRxDataBuffer[i], HEX);
  }
  Serial.print(SerialChecksum, HEX);
  Serial.print(F("\r\n"));

  if (SerialRxDataBuffer[0] == 0x4E)
  {
    Serial.print(F("0x4E ('N'): No Tag or operation failed."));
  }
  if (SerialRxDataBuffer[0] == 0x46)
  {
    Serial.print(F("0x46 ('F'): No Tag or operation failed."));
  }

  if (SerialRxDataBuffer[0] == 0x49)
  {
    Serial.print(F("0x49 ('I'): Invalid Value Block. The block was not in the proper value format when read back."));
  }
}

void serial::CmdWriteBlock4Byte(uint8_t write_block_4_byte_no, uint8_t* Write_Block_4_Byte)
{
  Buf_Write_Block_4_Byte[0] = 0xFF;
  Buf_Write_Block_4_Byte[1] = 0x00;
  Buf_Write_Block_4_Byte[2] = 0x06;
  Buf_Write_Block_4_Byte[3] = CMD_WRITE_BLOCK_4_BYTE;
  Buf_Write_Block_4_Byte[4] = write_block_4_byte_no;

  memcpy(&Buf_Write_Block_4_Byte[5], &Write_Block_4_Byte , sizeof(Write_Block_4_Byte));
  for (int i = 1; i < sizeof(Buf_Write_Block_4_Byte) - 1; i++ )
  {
    write_block_4_byte_checksum = Buf_Write_Block_4_Byte[i] + write_block_4_byte_checksum;
  }
  Buf_Write_Block_4_Byte[9] = write_block_4_byte_checksum;
  Serial2.write(Buf_Write_Block_4_Byte, 10);
}

void serial::Func_WriteBlock4Byte()
{
  Serial.print(F("Mifare Ultralight Page No:"));
  Serial.print(SerialRxDataBuffer[0], HEX);
  Serial.print(F(" "));
  Serial.print(SerialHeaderByte, HEX);
  Serial.print(SerialNodeAddress, HEX);
  Serial.print(SerialDataLength, HEX);
  Serial.print(SerialCommandByte, HEX);

  for (int  i = 0; i < SerialDataLength - 1 ; i++)
  {
    Serial.print(SerialRxDataBuffer[i], HEX);
  }
  Serial.print(SerialChecksum, HEX);
  Serial.print(F("\r\n"));

  if (SerialRxDataBuffer[0] == 0x4E)
  {
    Serial.print(F("0x4E ('N'):No Tag or operation failed."));
  }
  if (SerialRxDataBuffer[0] == 0x46)
  {
    Serial.print(F("0x46 ('F'): No Tag or operation failed."));
  }
  if (SerialRxDataBuffer[0] == 0x55)
  {
    Serial.print(F("0x55 ('U'): Read after write failed (Expected response for sector trailer block write)*"));
  }
  if (SerialRxDataBuffer[0] == 0x58)
  {
    Serial.print(F("Unable to read after write"));
  }
}


void serial::CmdSeekForTag()
{
  Buf_Seek_For_Tag[0] = 0xFF;
  Buf_Seek_For_Tag[1] = 0x00;
  Buf_Seek_For_Tag[2] = 0x01;
  Buf_Seek_For_Tag[3] = 0x82;
  Buf_Seek_For_Tag[4] = 0x83;
  Serial2.write(Buf_Seek_For_Tag, 5);
}

void serial::Func_SeekForTag()
{
  if (SerialRxDataBuffer[0] == 0x4C)
  {
    Serial.println(F("0x4C (‘L’) : Operation is successful.Card UID will be reported as soon as detected."));
  }
  if (SerialRxDataBuffer[0] == 0x46)
  {
    Serial.println(F("0x46 (‘F’) : Operation failed. CmdSeekForTag cannot be used if Auto Mode is enabled"));
  }
  if (SerialRxDataBuffer[0] == 0x55)
  {
    Serial.println(F("0x55 (‘U’) : Operation failed. The Antenna power was switched off."));
  }

  else if ((SerialDataLength == 6) || (SerialDataLength == 9))
  {
    Serial.print(F("Tag_Type: "));
    Serial.print(SerialRxDataBuffer[0], HEX);
    Serial.print(F(" "));
    int uidlength = SerialDataLength - 2;

    Serial.print(F("UID Length:"));
    Serial.print(uidlength);
    Serial.print(F(" , UID:"));
    for (int i = 0; i < uidlength; i++)
    {
      Serial.print(SerialRxDataBuffer[uidlength - i], HEX);
      Serial.print(F("-"));
    }
    Serial.print(F("\r\n"));
  }
}

void serial::CmdActivateIdle()
{
  Buf_Activate_Idle[0] = 0xFF;
  Buf_Activate_Idle[1] = 0x00;
  Buf_Activate_Idle[2] = 0x01;
  Buf_Activate_Idle[3] = 0x84;
  Buf_Activate_Idle[4] = 0x85;

  Serial2.write (Buf_Activate_Idle, 5);
}

void serial::Func_ActivateIdle()
{
  if (SerialRxDataBuffer[0] == 0x4E)
  {
    Serial.println(F("No Tag"));
  }

  if (SerialRxDataBuffer[0] == 0x55)
  {
    Serial.println(F("Operation failed. The Antenna power was switched off."));
  }

  else if ((SerialDataLength == 6) || (SerialDataLength == 9))
  {
    Serial.print(F("Tag_Type: "));
    Serial.print(SerialRxDataBuffer[0], HEX);
    Serial.print(F(" "));
    int uidlength = SerialDataLength - 2;

    Serial.print(F("UID Length:"));
    Serial.print(uidlength);
    Serial.print(F(" , UID:"));
    for (int i = 0; i < uidlength; i++)
    {
      Serial.print(SerialRxDataBuffer[uidlength - i], HEX);
      Serial.print(F("-"));
    }
    Serial.print(F("\r\n"));
  }
}

void serial::CmdHalt()
{
  Buf_Halt[0] = 0xFF;
  Buf_Halt[1] = 0x00;
  Buf_Halt[2] = 0x01;
  Buf_Halt[3] = 0x93;
  Buf_Halt[4] = 0x94;
  Serial2.write(Buf_Halt, 5);
}

void serial::Func_Halt()
{
  if (SerialRxDataBuffer[0] == 0x4C)
  {
    Serial.println(F(" Status Code: 0x4C(‘L’). Halt command is executed successfully."));
  }
  if (SerialRxDataBuffer[0] == 0x55)
  {
    Serial.println(F(" Status Code: 0x55(‘U’). Operation failed. The Antenna power was switched off."));
  }

}
