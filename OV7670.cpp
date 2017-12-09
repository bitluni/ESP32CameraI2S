#include "OV7670.h"
#include "XClk.h"
#include "Log.h"

OV7670::OV7670(Mode m, const int SIOD, const int SIOC, const int VSYNC, const int HREF, const int XCLK, const int PCLK, const int D0, const int D1, const int D2, const int D3, const int D4, const int D5, const int D6, const int D7)
  :i2c(SIOD, SIOC)
{
  ClockEnable(XCLK, 20000000); //base is 80MHz
  
  DEBUG_PRINT("Waiting for VSYNC...");  
  pinMode(VSYNC, INPUT);
  while(!digitalRead(VSYNC));
  while(digitalRead(VSYNC));
  DEBUG_PRINTLN(" done");

  mode = m;
  switch(mode)
  {
    case VGA_RGB565:
    xres = 640;
    yres = 480;
    break;
    case QVGA_RGB565:
    xres = 320;
    yres = 240;
    break;
    case QQVGA_RGB565:
    xres = 160;
    yres = 120;
    QQVGARGB565();
    break;
    case QQQVGA_RGB565:
    xres = 80;
    yres = 60;
    QQQVGARGB565();    
    break;
    default:
    xres = 0;
    yres = 0;
  }
  //testImage();
  I2SCamera::init(xres, yres, VSYNC, HREF, XCLK, PCLK, D0, D1, D2, D3, D4, D5, D6, D7);
}

void OV7670::testImage()
{
  i2c.writeRegister(ADDR, 0x71, 0x35 | 0x80);
}
  
void OV7670::saturation(int s)  //-2 to 2
{
  //color matrix values
  i2c.writeRegister(ADDR, 0x4f, 0x80 + 0x20 * s);
  i2c.writeRegister(ADDR, 0x50, 0x80 + 0x20 * s);
  i2c.writeRegister(ADDR, 0x51, 0x00);
  i2c.writeRegister(ADDR, 0x52, 0x22 + (0x11 * s) / 2);
  i2c.writeRegister(ADDR, 0x53, 0x5e + (0x2f * s) / 2);
  i2c.writeRegister(ADDR, 0x54, 0x80 + 0x20 * s);
  i2c.writeRegister(ADDR, 0x58, 0x9e);  //matrix signs
}

void OV7670::frameControl(int hStart, int hStop, int vStart, int vStop)
{
  i2c.writeRegister(ADDR, REG_HSTART, hStart >> 3);
  i2c.writeRegister(ADDR, REG_HSTOP,  hStop >> 3);
  i2c.writeRegister(ADDR, REG_HREF, ((hStop & 0b111) << 3) | (hStart & 0b111));

  i2c.writeRegister(ADDR, REG_VSTART, vStart >> 2);
  i2c.writeRegister(ADDR, REG_VSTOP, vStop >> 2);
  i2c.writeRegister(ADDR, REG_VREF, ((vStop & 0b11) << 2) | (vStart & 0b11));
}

void OV7670::QQQVGA()
{
    i2c.writeRegister(ADDR, REG_COM3, 0x04);  //DCW enable
    i2c.writeRegister(ADDR, REG_COM14, 0x1b); //pixel clock divided by 4, manual scaling enable, DCW and PCLK controlled by register
    i2c.writeRegister(ADDR, REG_SCALING_XSC, 0x3a);
    i2c.writeRegister(ADDR, REG_SCALING_YSC, 0x35);
    i2c.writeRegister(ADDR, REG_SCALING_DCWCTR, 0x33); //downsample by 8
    i2c.writeRegister(ADDR, REG_SCALING_PCLK_DIV, 0xf3); //pixel clock divided by 8
    i2c.writeRegister(ADDR, REG_SCALING_PCLK_DELAY, 0x02);
}

void OV7670::QQVGA()
{
  //160x120 (1/4)
  //i2c.writeRegister(ADDR, REG_CLKRC, 0x01);
  i2c.writeRegister(ADDR, REG_COM3, 0x04);  //DCW enable
  
  i2c.writeRegister(ADDR, REG_COM14, 0x1a); //pixel clock divided by 4, manual scaling enable, DCW and PCLK controlled by register
  i2c.writeRegister(ADDR, REG_SCALING_XSC, 0x3a);
  i2c.writeRegister(ADDR, REG_SCALING_YSC, 0x35);
  
  i2c.writeRegister(ADDR, REG_SCALING_DCWCTR, 0x22); //downsample by 4
  i2c.writeRegister(ADDR, REG_SCALING_PCLK_DIV, 0xf2); //pixel clock divided by 4
  i2c.writeRegister(ADDR, REG_SCALING_PCLK_DELAY, 0x02);
}
  
void OV7670::QQVGARGB565()
{
  i2c.writeRegister(ADDR, REG_COM7, 0b10000000);  //all registers default
      
  i2c.writeRegister(ADDR, REG_CLKRC, 0b10000000); //double clock
  i2c.writeRegister(ADDR, REG_COM11, 0b1000 | 0b10); //enable auto 50/60Hz detect + exposure timing can be less...

  i2c.writeRegister(ADDR, REG_COM7, 0b100); //RGB
  i2c.writeRegister(ADDR, REG_COM15, 0b11000000 | 0b010000); //RGB565

  QQVGA();

  frameControl(196, 52, 8, 488); //no clue why horizontal needs such strange values, vertical works ok
  
  //i2c.writeRegister(ADDR, REG_COM10, 0x02); //VSYNC negative
  //i2c.writeRegister(ADDR, REG_MVFP, 0x2b);  //mirror flip

  i2c.writeRegister(ADDR, 0xb0, 0x84);// no clue what this is but it's most important for colors
  saturation(0);
  i2c.writeRegister(ADDR, 0x13, 0xe7); //AWB on
  i2c.writeRegister(ADDR, 0x6f, 0x9f); // Simple AWB
}

void OV7670::QQQVGARGB565()
{
  i2c.writeRegister(ADDR, REG_COM7, 0b10000000);  //all registers default
      
  i2c.writeRegister(ADDR, REG_CLKRC, 0b10000000); //double clock
  i2c.writeRegister(ADDR, REG_COM11, 0b1000 | 0b10); //enable auto 50/60Hz detect + exposure timing can be less...

  i2c.writeRegister(ADDR, REG_COM7, 0b100); //RGB
  i2c.writeRegister(ADDR, REG_COM15, 0b11000000 | 0b010000); //RGB565

  QQQVGA();
  
  frameControl(196, 52, 8, 488); //no clue why horizontal needs such strange values, vertical works ok
  
  //i2c.writeRegister(ADDR, REG_MVFP, 0x2b);  //mirror flip
   
  i2c.writeRegister(ADDR, 0xb0, 0x84);// no clue what this is but it's most important for colors
  saturation(0);
  i2c.writeRegister(ADDR, 0x13, 0xe7); //AWB on
  i2c.writeRegister(ADDR, 0x6f, 0x9f); // Simple AWB
}
  
