#include "MessageHandler.h"
#include "SystemState.h"

#define rxPin 3
#define txPin 4

enum 
{
  cmdSetPixel,                // Done
  cmdSetNumPixels,            // Not done
};

const int rspSuccess = 1;
const int rspError = 0;

SoftwareSerial softSerial(rxPin,txPin);

void MessageHandler::initialize()
{
  softSerial.begin(9600);
  dprint.setSerialObj(&softSerial);
  setSerialObj(&softSerial);
}

void MessageHandler::processMsg() 
{
  while (softSerial.available() > 0) 
  {
    process(softSerial.read());
    if (messageReady()) 
    {
        msgSwitchYard();
        reset();
    }   
  }
  return;
}

void MessageHandler::msgSwitchYard() 
{
  int cmd = readInt(0); 
  dprint.start();
  dprint.addIntItem("cmdId", cmd);

  switch (cmd) 
  {
    case cmdSetPixel:
      handleSetPixel();
      break;

    case cmdSetNumPixels:
      handleSetNumPixels();
      break;

    default:
      dprint.addIntItem("status", rspError);
      dprint.addStrItem("errMsg", "unknown command");
      break;
    }              
    dprint.stop();
}

void MessageHandler::systemCmdRsp(bool flag) 
{
    if (flag) 
    {
        dprint.addIntItem("status", rspSuccess);
    }
    else 
    {
        dprint.addIntItem("status", rspError);
        dprint.addStrItem("errMsg", systemState.errMsg); 
    }
} 

// TO-DO: validate r, g, b ranges
void MessageHandler::handleSetPixel() 
{
    if (!checkNumberOfArgs(5)) {return;}
    uint16_t i = readInt(1);
    //uint16_t y = readInt(1);
    uint8_t r = readInt(2);
    uint8_t g = readInt(3);
    uint8_t b = readInt(4);
    systemCmdRsp(systemState.setPixel(i,r,g,b));
}

void MessageHandler::handleSetNumPixels() 
{
}

bool MessageHandler::checkNumberOfArgs(int num) 
{
    bool flag = true;
    if (numberOfItems() != num) {
        dprint.addIntItem("status", rspError);
        dprint.addStrItem("errMsg", "incorrect number of arguments");
        flag = false;
    }
    return flag;
}

MessageHandler messageHandler;
