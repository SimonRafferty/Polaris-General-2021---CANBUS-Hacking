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
long Change_Cnt[50];
int  List_Cnt = 0;

bool bFound=false;



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
    Change_Cnt[i] = 0;   
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
  
  // Check for received J1939 messages
  int canReceive = CAN0.checkReceive();
  if (CAN_MSGAVAIL == canReceive)
  {
   
    CAN0.readMsgBuf(&canID,&nDataLen, nData);
    

    //sprintf(sString, "PGN: 0x%X Src: 0x%X Dest: 0x%X ", (int)canID, nSrcAddr, nDestAddr);
    //Traverse the list for this PGN.  If it's present, compare data to last.
    //If it's different, print & update.  
    //If not in list, add a new one.
    bFound = false;
    
    for(int i=0; i<50; i++) {
      if(PID_List[i]==canID){
        bFound = true;
        //Found PGN, compare data
        if((nData[0]==Data_Byte_0[i]) && (nData[1]==Data_Byte_1[i]) && (nData[2]==Data_Byte_2[i]) && (nData[3]==Data_Byte_3[i]) && (nData[4]==Data_Byte_4[i]) && (nData[5]==Data_Byte_5[i]) && (nData[6]==Data_Byte_6[i]) && (nData[7]==Data_Byte_7[i])) {
          //Data is the same, ignore
         } else {
          //Print the data     
          //If the data changes frequently, don't display - it's effectively noise
          if(Change_Cnt[i]<10) {     
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
          //Update the list
          Data_Byte_0[i] = nData[0];
          Data_Byte_1[i] = nData[1];
          Data_Byte_2[i] = nData[2];
          Data_Byte_3[i] = nData[3];
          Data_Byte_4[i] = nData[4];
          Data_Byte_5[i] = nData[5];
          Data_Byte_6[i] = nData[6];
          Data_Byte_7[i] = nData[7];
          Change_Cnt[i]++;
        }   
      }
  
    }
    if(!bFound && List_Cnt<50) {
      //PGN Not found, add to list
      PID_List[List_Cnt]=canID;
      Data_Byte_0[List_Cnt] = nData[0];
      Data_Byte_1[List_Cnt] = nData[1];
      Data_Byte_2[List_Cnt] = nData[2];
      Data_Byte_3[List_Cnt] = nData[3];
      Data_Byte_4[List_Cnt] = nData[4];
      Data_Byte_5[List_Cnt] = nData[5];
      Data_Byte_6[List_Cnt] = nData[6];
      Data_Byte_7[List_Cnt] = nData[7];
      Change_Cnt[List_Cnt] = 0; //Keep track of the number of changes
      //Print the new data
      Serial.print("New #"); Serial.print(List_Cnt); Serial.print("# ");  //Print the index
      
      
      
      
      sprintf(sString, "PID: 0x%.8lX  Data:", (canID & 0x1FFFFFFF));
      Serial.print(sString);
      Serial.print("Data: ");
      for (int nIndex = 0; nIndex < nDataLen; nIndex++)
      {
        sprintf(sString, "0x%X ", nData[nIndex]);
        Serial.print(sString);

      }
      Serial.print("\n\r");  
      List_Cnt++;          
    }
  }  
    
  
}
