//#include Camera.h
#include "soc/soc.h"
#include "soc/gpio_sig_map.h"
#include "soc/i2s_reg.h"
#include "soc/i2s_struct.h"
#include "soc/io_mux_reg.h"

#include <driver/dac.h>

#include "I2S.h"

//esp32 mini kit
//boot       0 2 5
//flash      6(clk) 7(sd0) 8(sd1) 9(sd2) 10(sd3) 11(cmd)
//UART       1(tx) 3(rx)
//free pins  4 12(tdi) 13(tck) 14(tms) 15(tdo) 16 17 27 32 33
//spi        18(sck) 19(miso) 23(mosi) 5(ss)
//i2c        21(sda) 22(scl)
//dac        25 26
//input only 34 35 36(svp) 39(svn)  

//pins 0 2 5 are used for boot.. only connect hi-z. can still be used as output without external pullups
//free pins 4 12 13 14 15 16 17 27 32 33
//input only 34, 35, 36(VP), 39(VN)
//dac 25, 26

const int SIOD = 21; //SDA
const int SIOC = 22; //SCL

const int VSYNC = 34;//25;
const int HREF = 35;//23;

const int XCLK = 32;//21;
const int PCLK = 33;

const int D0 = 4;
const int D1 = 12;
const int D2 = 13;
const int D3 = 14;
const int D4 = 15;
const int D5 = 16;
const int D6 = 17;
const int D7 = 27;

const int DC = 2;

//const int DAC1 = 25;
//const int DAC2 = 26;
#include <math.h>

const int XRES = 640;
const int YRES = 480;

void setup() 
{
  Serial.begin(115200);
  I2S::init(XRES, YRES, VSYNC, HREF, XCLK, PCLK, D0, D1, D2, D3, D4, D5, D6, D7);
  //dac_output_enable(DAC_CHANNEL_1); //25
  //dac_output_enable(DAC_CHANNEL_2); //26
  
}


void loop()
{
  Serial.print(I2S::blocksReceived);
  Serial.print(' ');
  Serial.println(I2S::framesReceived);
  Serial.print(' ');
  for(int i = 0; i < 16; i++)
  { 
      if(I2S::dmaBuffer[0]->buffer[i] < 16) Serial.print('0');
      Serial.print(I2S::dmaBuffer[0]->buffer[i], HEX);
  }
  Serial.println();
  delay(1000); 
}
