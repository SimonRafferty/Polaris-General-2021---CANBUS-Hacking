/*
  Simon Rafferty 2022
  
  Receive key data from a Polaris ECU and decode
  Send some data back to the bus to drive selected dash & EPAS functions.

  On models having ABS, this will not drive the brake controller.  Brakes will function but without ABS
*/
#include <mcp_can.h>
static unsigned constexpr MCP_CS = 10;  //Chip Select Pin

MCP_CAN CAN(10);

//Things we might want to read:  (Byte count starting from Zero [0])
const long unsigned int CAN_ID_SHIFTER        = 0x18F00500; //Byte [5] - 4C=L, 48=H, 4E=N, 52=R, 50=P - Gear Selector
const long unsigned int CAN_ID_THROTTLE       = 0x18FEF200; //Byte [3] - 00=Minimum, ED=Maximum
const long unsigned int CAN_ID_HANDBRAKE      = 0x18FEF100; //Byte [0] - 37=Applied, 33=Released
const long unsigned int CAN_ID_FOOTBRAKE      = 0x18FEF1FE; //Byte [3] - DF=Applied, CF=Released
const long unsigned int CAN_ID_SEATBELT       = 0x18FF6CFE; //Byte [1] - F7=Secured, F3=Released - Seatbelt
const long unsigned int CAN_ID_DIFFLOCK       = 0x18F006FE; //Byte [1] - DF=Locked, CF=Open
const long unsigned int CAN_ID_4WD            = 0x18FDDFFE; //Byte [0] - FC=2WD, FD=4WD



//Things we might want to write:  (Byte count starting from Zero [0])
const long unsigned int CAN_ID_SPEED          = 0x18FEF100; //Byte [1] LSB, Byte [2] MSB - Speed mph x 500
unsigned char CAN_BUF_SPEED[8] { 0xFF, 0x0C, 0x84, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

const long unsigned int CAN_ID_RPM            = 0x18FF6600; //Byte [0] LSB, Byte [1] MSB - RPM
unsigned char CAN_BUF_RPM[8] = { 0xAA, 0xAA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

const long unsigned int CAN_ID_CEL            = 0x18FECA00; //Various warning lights, including Check Engine
unsigned char CAN_BUF_CEL[8] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

const long unsigned int CAN_ID_ENG_TEMP       = 0x18FEEEFE; //Byte [0] - Engine Temperature (Degrees C + 40)
unsigned char CAN_BUF_RZR_ENG_TEMP[8] = { 0x45, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

const long unsigned int CAN_ID_RZR_4WD            = 0x1CFDDF00; //Byte [0] - 4wheel drive - FC off, FD on.
unsigned char CAN_BUF_RZR_4WD[8] = { 0xFD, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

const long unsigned int CAN_ID_FUEL_LEVEL       = 0x18FEFC00; //Byte [1] - Fuel level 0-254
unsigned char CAN_BUF_FUEL_LEVEL[8] = { 0xFF, 0x6F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

const long unsigned int CAN_ID_ENG_HOURS       = 0x18FEE500; //Byte [0] LSB, Byte [1] MSB - Total Engine Hours
unsigned char CAN_BUF_ENG_HOURS[8] = { 0xCC, 0x4, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF };


String sSHIFTER, sSPEED, sRPM, sCEL, sFUEL_LEVEL, sENG_TEMP, s4WD, sDIFFLOCK, sTHROTTLE, sHANDBRAKE, sFOOTBRAKE, sSEATBELT, sMESSAGE; 

long lDispTimer = millis();

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

}

void loop()
{
  // Declarations
  byte nData[8];
  unsigned char  nDataLen;
  long unsigned int lPID;
  char sString[80];
  unsigned int Value;
  
  //These are messages you might want to send, if for example, you've removed the OE ECU
  //CAN.sendMsgBuf(CAN_ID_CEL, 1, 8, CAN_BUF_CEL);
  //CAN.sendMsgBuf(CAN_ID_FUEL_LEVEL, 1, 8, CAN_BUF_RZR_FUEL_LEVEL);
  //CAN.sendMsgBuf(CAN_ID_ENG_TEMP, 1, 8, CAN_BUF_RZR_ENG_TEMP);
  //CAN.sendMsgBuf(CAN_ID_FUEL_LEVEL, 1, 8, CAN_BUF_FUEL_LEVEL);
  //CAN.sendMsgBuf(CAN_ID_ENG_HOURS, 1, 8, CAN_BUF_ENG_HOURS);

  /*
  lPID = CAN_ID_RPM; //This should make the EPAS start
  nData[0] = 0xFF; nData[1] = 0xFF; nData[2] = 0xFF; nData[3] = 0xFF; nData[4] = 0xFF; nData[5] = 0xFF; nData[6] = 0xFF; nData[7] = 0xFF; //Reset all the bytes
  //Set the ones that matter
  nData[0] = byte(5000 & 0x00FF);  //1000 rpm
  nData[1] = byte(int(5000 & 0xFF00)/255);
  CAN.sendMsgBuf(lPID, 1, 8, nData);
  */
  /*
  lPID = CAN_ID_SPEED; //This should make the EPAS start
  nData[0] = 0xFF; nData[1] = 0xFF; nData[2] = 0xFF; nData[3] = 0xFF; nData[4] = 0xFF; nData[5] = 0xFF; nData[6] = 0xFF; nData[7] = 0xFF; //Reset all the bytes
  //Set the ones that matter
  nData[0] = byte(5000 & 0x00FF);  //1000 rpm
  nData[1] = byte(int(5000 & 0xFF00)/255);
  CAN.sendMsgBuf(lPID, 1, 8, nData);
  */
  /*
  lPID = CAN_ID_ENG_TEMP; //This should make the EPAS start
  nData[0] = 0xFF; nData[1] = 0xFF; nData[2] = 0xFF; nData[3] = 0xFF; nData[4] = 0xFF; nData[5] = 0xFF; nData[6] = 0xFF; nData[7] = 0xFF; //Reset all the bytes
  //Set the ones that matter
  nData[0] = 30;  //-10C
  CAN.sendMsgBuf(lPID, 1, 8, nData);
  */
  //delay(100); //Don't spam the bus!
  
  //Once per second, display current values - the kind of things you might want to display on a DIY Dashboard?
  
  if(millis()-lDispTimer > 1000) {
    lDispTimer = millis();

    Serial.print("Gear="); Serial.print(sSHIFTER); //Works
    Serial.print("|Speed="); Serial.print(sSPEED);
    Serial.print("|RPM="); Serial.print(sRPM); //Works
    Serial.print("|CEL="); Serial.print(sCEL);
    Serial.print("|Fuel="); Serial.print(sFUEL_LEVEL); //Valuesibly Works
    Serial.print("|4WD="); Serial.print(s4WD); //Works
    Serial.print("|DIFF="); Serial.print(sDIFFLOCK); //Works
    Serial.print("|Temp="); Serial.print(sENG_TEMP); //Works
    Serial.print("|Gas="); Serial.print(sTHROTTLE); //Works
    Serial.print("|PkBrk="); Serial.print(sHANDBRAKE); //Works
    Serial.print("|FtBrk="); Serial.print(sFOOTBRAKE);  //Works
    Serial.print("|Belt="); Serial.print(sSEATBELT);  //Works
    
    Serial.print("|Msg="); Serial.print(sMESSAGE);
    Serial.println(); 
  }

  
  
  // Check for received messages
  int canReceive = CAN0.checkReceive();
  if (CAN_MSGAVAIL == canReceive) {
    CAN0.readMsgBuf(&lPID,&nDataLen, nData);
    if(nDataLen>0) {
        lPID = (lPID & 0x00FFFF00); //Remove source & destination, not interested on receive
        if(lPID==(CAN_ID_SHIFTER & 0x00FFFF00)) {
          //Serial.println("Shifter");
          Value = nData[5];
          if (Value == 0x4C) {
            sSHIFTER = "L";
          }
          if (Value == 0x48) {
            sSHIFTER = "H";
          }
          if (Value == 0x4e) {
            sSHIFTER = "N";
          }
          if (Value == 0x52) {
            sSHIFTER = "R";
          }
          if (Value == 0x50) {
            sSHIFTER = "P";
          }
        }
        if(lPID==(CAN_ID_SPEED & 0x00FFFF00)) { 
          //Serial.println("Speed");
          Value = int(nData[1])*255 + int(nData[0]);
          Value=Value/500; //Speed scale factor
          sSPEED = String(Value); 
        }
        if(lPID==(CAN_ID_RPM & 0x00FFFF00)) {
          //Serial.println("RPM");
          Value = int(nData[1])*255 + int(nData[0]);
          sRPM = String(Value); 
        }
        if(lPID==(CAN_ID_CEL & 0x00FFFF00)) {
          //Serial.println("CEL");
          Value = nData[1];
          sCEL = String(Value); 
        }
        if(lPID==(CAN_ID_FUEL_LEVEL & 0x00FFFF00)) {
          //Serial.println("FUEL");
          Value = nData[1]; //0-254
          sFUEL_LEVEL = String(Value); 
        }
          
        if(lPID==(CAN_ID_ENG_TEMP & 0x00FFFF00)) {
          //Serial.println("ENG TEMP");
          Value = nData[0]; //Temp in C + 40C
          sENG_TEMP = String(Value-40); 
        }
        if(lPID==(CAN_ID_4WD & 0x00FFFF00)) {
          //Serial.println("4WD");
          Value = nData[0];
          if (Value == 0xFD) {
            s4WD = "4WD"; 
          } else if(Value == 0xFC) {
            s4WD = "2WD"; 
          } else {
            s4WD = String(Value, HEX);  //Unknown, Valuesibly turf or difflock
          }
        }
        if(lPID==(CAN_ID_DIFFLOCK & 0x00FFFF00)) {
          //Serial.println("Difflock");
          Value = nData[1];
          if(Value == 0xDF) {
            sDIFFLOCK = "LOCK"; 
          } else if(Value == 0xCF) {
            sDIFFLOCK = "OPEN";
          }
        }

        if(lPID==(CAN_ID_THROTTLE & 0x00FFFF00)) {
          //Serial.println("Throttle");
          Value = nData[6];  //Range 25 to 250
          sTHROTTLE = String(Value); 
        }
        if(lPID==(CAN_ID_HANDBRAKE & 0x00FFFF00)) {
          //Serial.println("HANDBRAKE");
          Value = nData[0];
          if (Value == 0x37) {
            sHANDBRAKE = "APP"; 
          } else if (Value == 0x33){
            sHANDBRAKE = "REL";; 
          } else sHANDBRAKE = String(Value,HEX);
        }
        if(lPID==(CAN_ID_SEATBELT & 0x00FFFF00)) {
          Value = nData[1];
          if (Value == 0xF7) {
            sSEATBELT = "SEC";
          } else if(Value == 0xF3) {
            sSEATBELT = "REL";
          } else sSEATBELT = String(Value,HEX);
        }

          
        if(lPID==(CAN_ID_FOOTBRAKE & 0x00FFFF00)) {
          Value = nData[3];
          if (Value == 0xDF) {
            sFOOTBRAKE = "APP";
          } else if(Value == 0xCF) {
            sFOOTBRAKE = "REL"; 
          }
        
      }
    }
  }  
}
