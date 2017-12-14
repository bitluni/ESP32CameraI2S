#include "Arduino.h"

class I2C
{
  void inline DELAY()
  {
    delayMicroseconds(1);
  }

  void inline SCLLOW()
  {
    pinMode(SCL, OUTPUT);
    digitalWrite(SCL, 0);
  }

  void inline SCLHIGH()
  {
    pinMode(SCL, INPUT_PULLUP);  
    digitalWrite(SCL, 1);
  }

  void inline CLOCK()
  {
    DELAY();
    SCLHIGH();
    DELAY();
    DELAY();
    SCLLOW();
    DELAY();
  }
  
  void inline SDALOW()
  {
    pinMode(SDA, OUTPUT);
    digitalWrite(SDA, 0);  
  }
  
  void inline SDAHIGH()
  {
    pinMode(SDA, OUTPUT);
    digitalWrite(SDA, 1);  
  }

  void inline SDAPULLUP()
  {
    pinMode(SDA, INPUT_PULLUP);  
  }

  void pushByte(unsigned char b)
  {
    for(char i = 0; i < 8; i++)
    {
      if(b & 0x80)
        SDAHIGH();
      else
        SDALOW();
      b <<= 1;
      CLOCK();
    }  
  }
  
  bool getAck()
  {
    SDAPULLUP();
    DELAY();
    SCLHIGH();
    DELAY();
    int r = digitalRead(SDA);
    SDALOW();
    DELAY();
    SCLLOW();
    DELAY();
    return r == 0;
  }

  void start()
  {
    SDAPULLUP();
    DELAY();
    SCLHIGH();
    DELAY();
    SDALOW();
    DELAY();
    SCLLOW();
    DELAY();
  }
  
  void end()
  {
    SCLHIGH();
    DELAY();
    SDAPULLUP();
    DELAY();
  }
  
  public:
  int SDA;
  int SCL;
  I2C(const int data, const int clock)
  {
    SDA = data;
    SCL = clock;
    pinMode(SDA, INPUT_PULLUP);
    pinMode(SCL, INPUT_PULLUP);
    digitalWrite(SDA, 0);
    digitalWrite(SCL, 0);
  }
  
  bool writeRegister(unsigned char addr, unsigned char reg, unsigned char data)
  {
    start();
    pushByte(addr);
    
    if(!getAck())
    {
      end();
      return false;
    }
    
    pushByte(reg);
    if(!getAck())
    {
      end();
      return false;
    }
  
    pushByte(data);
    if(!getAck())
    {
      end();
      return false;
    }
  
    end();
    return true;
  }
};
