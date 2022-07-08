/*
	Based on the Simple J1939 Library, Copyright (C) 2022 Victor Chavez

  This compiles a list of CAN messages received as well as keeping a count of how many times the
  data for that message has changed.  If more than 10 times (value seems to work OK), it no longer prints the 
  message data.

  If you leave it to settle for a couple of mins, so it is ignoring all the frequent update messages then clear
  the serial monitor window, anything that pops up has changed just then.

  This is ideal for discovering the codes for switches & things like that

*/
#include <mcp_can.h>
static unsigned constexpr MCP_CS = 10;  //Chip Select Pin

unsigned long PID_List[50];
byte Data_Byte_0[50];
byte Data_Byte_1[50];
byte Data_Byte_2[50];
byte Data_Byte_3[50];
byte Data_Byte_4[50];
byte Data_Byte_5[50];
byte Data_Byte_6[50];
byte Data_Byte_7[50];
int  List_Cnt = 0;

bool bFound=false;

int MessageCnt = 0;

MCP_CAN CAN0(MCP_CS);

void setup()
{
  // Set the serial interface baud rate
  Serial.begin(115200);
  if(CAN0.begin(MCP_ANY, CAN_250KBPS, MCP_8MHZ) == CAN_OK)
  {
    CAN0.setMode(MCP_NORMAL); // Set operation mode to normal so the MCP2515 sends acks to received data.
  }
  else
  {
    Serial.print("CAN Init Failed.\n\r");
    while(1);
  }

  //Initialise the list
  for(int i=0; i<50; i++) {
    PID_List[i] = 0;
    Data_Byte_0[i] = 0;
    Data_Byte_1[i] = 0;
    Data_Byte_2[i] = 0;
    Data_Byte_3[i] = 0;
    Data_Byte_4[i] = 0;
    Data_Byte_5[i] = 0;
    Data_Byte_6[i] = 0;
    Data_Byte_7[i] = 0; 
  }
}

void loop()
{
  // Declarations
  byte nPriority;
  byte nSrcAddr;
  byte nDestAddr;
  byte nData[8];
  unsigned char nDataLen;
  long unsigned int canID;
  char sString[80];

  int Offset = 150;
  int GetCount = 50;
  
  // Check for received J1939 messages
  int canReceive = CAN0.checkReceive();
  if (CAN_MSGAVAIL == canReceive)
  {
    MessageCnt++;
    CAN0.readMsgBuf(&canID,&nDataLen, nData);
    if(MessageCnt<(Offset+GetCount)){
      if(MessageCnt>=(Offset-1) && MessageCnt<(Offset+GetCount)){
        List_Cnt = MessageCnt-Offset;
        PID_List[List_Cnt]=canID;
        Data_Byte_0[List_Cnt] = nData[0];
        Data_Byte_1[List_Cnt] = nData[1];
        Data_Byte_2[List_Cnt] = nData[2];
        Data_Byte_3[List_Cnt] = nData[3];
        Data_Byte_4[List_Cnt] = nData[4];
        Data_Byte_5[List_Cnt] = nData[5];
        Data_Byte_6[List_Cnt] = nData[6];
        Data_Byte_7[List_Cnt] = nData[7];
   
        //Print the new data
        sprintf(sString, "PID: 0x%.8lX  Data:", (canID & 0x1FFFFFFF));
        Serial.print(sString);
        Serial.print("Data: ");
        for (int nIndex = 0; nIndex < nDataLen; nIndex++)
        {
          sprintf(sString, "0x%X ", nData[nIndex]);
          Serial.print(sString);
    
        }
        Serial.print("\n\r"); 
      } 
    } else {
      Serial.println("######### Got data, waiting 30 sec to start sending ###########");
      delay(30000);
      for(int SendCnt=0; SendCnt<500; SendCnt++) {
        for(int index=0; index<GetCount; index++) {
          unsigned char BUFFER[8] = { Data_Byte_0[index], Data_Byte_1[index], Data_Byte_2[index], Data_Byte_3[index], Data_Byte_4[index], Data_Byte_5[index], Data_Byte_6[index], Data_Byte_7[index] };      
          CAN0.sendMsgBuf(PID_List[index], 1, 8, BUFFER);
          sprintf(sString, "PID: 0x%.8lX  Data:", (PID_List[index] & 0x1FFFFFFF));
          Serial.print(sString);
          
          for (int nIndex = 0; nIndex < 8; nIndex++)
          {
            sprintf(sString, "0x%X ", BUFFER[nIndex]);
            Serial.print(sString);
      
          }
          Serial.print("\n\r");  
          delay(20);
        }
        Serial.println("**********************");
      }
      Serial.println("######### Finished sending ###########");
      
    }
  }

    
  
}
