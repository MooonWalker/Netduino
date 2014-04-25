using System;
using System.Threading;
using Microsoft.SPOT;
using Microsoft.SPOT.Hardware;
using netduino.helpers.Helpers;

namespace netduino.helpers.Hardware {
    /// <summary>
    /// This class is a driver for the monochrome 128x64 OLED display referenced SSD1306.
    /// http://www.adafruit.com/index.php?main_page=product_info&cPath=37&products_id=326
    /// The code is an adaptation of the Arduino library written by Limor Fried: https://github.com/adafruit/SSD1306
    /// </summary>
    public class AdaFruitSSD1306 : IDisposable {
        public enum Color {
            Black,
            White
        }

        public const int Width = 128;
        public const int Height = 64;

        public enum Command {
            SETCONTRAST = 0x81,
            DISPLAYALLON_RESUME = 0xA4,
            DISPLAYALLON = 0xA5,
            NORMALDISPLAY = 0xA6,
            INVERTDISPLAY = 0xA7,
            DISPLAYOFF = 0xAE,
            DISPLAYON = 0xAF,
            SETDISPLAYOFFSET = 0xD3,
            SETCOMPINS = 0xDA,
            SETVCOMDETECT = 0xDB,
            SETDISPLAYCLOCKDIV = 0xD5,
            SETPRECHARGE = 0xD9,
            SETMULTIPLEX = 0xA8,
            SETLOWCOLUMN = 0x00,
            SETHIGHCOLUMN = 0x10,
            SETSTARTLINE = 0x40,
            MEMORYMODE = 0x20,
            COMSCANINC = 0xC0,
            COMSCANDEC = 0xC8,
            SEGREMAP = 0xA0,
            CHARGEPUMP = 0x8D
        }

        public enum VccType {
            EXTERNALVCC = 0x1,
            SWITCHCAPVCC = 0x2
        }

        private const bool Data = true;
        private const bool DisplayCommand = false;

        public AdaFruitSSD1306(Cpu.Pin dc, Cpu.Pin reset, Cpu.Pin chipSelect, SPI.SPI_module spiModule = SPI.SPI_module.SPI1, uint speedKHz = 10000) {

            AutoRefreshScreen = false;

            var spiConfig = new SPI.Configuration(
                SPI_mod: spiModule,
                ChipSelect_Port: chipSelect,
                ChipSelect_ActiveState: false,
                ChipSelect_SetupTime: 0,
                ChipSelect_HoldTime: 0,
                Clock_IdleState: false,
                Clock_Edge: true,
                Clock_RateKHz: speedKHz
                );

            Spi = new SPI(spiConfig);

            dcPin = new OutputPort(dc, false);
            resetPin = new OutputPort(reset, false);
        }

        public bool AutoRefreshScreen { get; set; }

        public void InvertDisplay(bool cmd) {
            dcPin.Write(DisplayCommand);

            if (cmd) {
                SendCommand(Command.INVERTDISPLAY);
            } else {
                SendCommand(Command.NORMALDISPLAY);
            }

            dcPin.Write(Data);
        }

        public void DrawBitmap(int x, int y, ref byte[] bitmap, int w, int h, Color color) {
            for (int j=0; j<h; j++) {
                for (int i=0; i<w; i++ ) {
                    var Pixel = bitmap[i + (j/8)*w];
                    Pixel &= (byte) (1 << (j%8));
                    if ((Pixel) != 0) {
	                    SetPixel(x+i, y+j, color);
                    }
                }
            }
            if (AutoRefreshScreen) {
                Refresh();
            }
        }

        public void DrawString(int x, int line, string str) {
            foreach (Char c in str) {
                DrawCharacter(x, line, c);

                x += 6; // 6 pixels wide

                if (x + 6 >= Width) {
                    x = 0;    // ran out of this line
                    line++;
                }
                
                if (line >= Height / 8) {
                    return;        // ran out of space :(
                }
            }
            if (AutoRefreshScreen) {
                Refresh();
            }
        }

        protected void DrawCharacter(int x, int line, Char c)  {
            for (int i =0; i<5; i++ ) {
                displayBuffer[x + (line*128) ] = Font[(c*5)+i];
                x++;
            }
        }

        // bresenham's algorithm - thx wikipedia
        public void DrawLine(int x0, int y0, int x1, int y1, Color color) {
            int steep = (System.Math.Abs(y1 - y0) > System.Math.Abs(x1 - x0)) ? 1 : 0;
          
            if (steep != 0) {
                Swap(ref x0, ref y0);
                Swap(ref x1, ref y1);
            }

            if (x0 > x1) {
                Swap(ref x0, ref x1);
                Swap(ref y0, ref y1);
            }

            int dx, dy;
            dx = x1 - x0;
            dy = System.Math.Abs(y1 - y0);

            int err = dx / 2;
            int ystep;

            if (y0 < y1) {
                ystep = 1;
            } 
            else {
                ystep = -1;
            }

            for (; x0<x1; x0++) {
                if (steep != 0) {
                    SetPixel(y0, x0, color);
                }
                else {
                    SetPixel(x0, y0, color);
                }
            
                err -= dy;
            
                if (err < 0) {
                    y0 += ystep;
                    err += dx;
                }
            }

            if (AutoRefreshScreen) {
                Refresh();
            }
        }

        public void DrawRectangle(int x, int y, int w, int h, Color color) {
            for (int i=x; i<x+w; i++) {
                SetPixel(i, y, color);
                SetPixel(i, y+h-1, color);
            }
            for (int i=y; i<y+h; i++) {
                SetPixel(x, i, color);
                SetPixel(x+w-1, i, color);
            }
            if (AutoRefreshScreen) {
                Refresh();
            }
        }

        public void FillRectangle(int x, int y, int w, int h, Color color) {
            for (int i=x; i<x+w; i++) {
                for (int j=y; j<y+h; j++) {
                    SetPixel(i, j, color);
                }
            }
            if (AutoRefreshScreen) {
                Refresh();
            }
        }

        public void DrawCircle(int x0, int y0, int r, Color color) {
            int f = 1 - r;
            int ddF_x = 1;
            int ddF_y = -2 * r;
            int x = 0;
            int y = r;

            SetPixel(x0, y0+r, color);
            SetPixel(x0, y0 - r, color);
            SetPixel(x0 + r, y0, color);
            SetPixel(x0 - r, y0, color);

            while (x<y) {
                if (f >= 0) {
                    y--;
                    ddF_y += 2;
                    f += ddF_y;
                }

                x++;
                ddF_x += 2;
                f += ddF_x;
  
                SetPixel(x0 + x, y0 + y, color);
                SetPixel(x0 - x, y0 + y, color);
                SetPixel(x0 + x, y0 - y, color);
                SetPixel(x0 - x, y0 - y, color);

                SetPixel(x0 + y, y0 + x, color);
                SetPixel(x0 - y, y0 + x, color);
                SetPixel(x0 + y, y0 - x, color);
                SetPixel(x0 - y, y0 - x, color);    
            }
            if (AutoRefreshScreen) {
                Refresh();
            }
        }

        public void FillCircle(int x0, int y0, int r, Color color) {
            int f = 1 - r;
            int ddF_x = 1;
            int ddF_y = -2 * r;
            int x = 0;
            int y = r;

            for (int i=y0-r; i<=y0+r; i++) {
            SetPixel(x0, i, color);
            }

            while (x<y) {
                if (f >= 0) {
                    y--;
                    ddF_y += 2;
                    f += ddF_y;
                }

                x++;
                ddF_x += 2;
                f += ddF_x;
  
                for (int i=y0-y; i<=y0+y; i++) {
                    SetPixel(x0+x, i, color);
                    SetPixel(x0-x, i, color);
                }

                for (int i=y0-x; i<=y0+x; i++) {
                    SetPixel(x0+y, i, color);
                    SetPixel(x0-y, i, color);
                }    
            }
            if (AutoRefreshScreen) {
                Refresh();
            }
        }

        public void SetPixel(int x, int y, Color color) {
            if ((x >= Width) || (y >= Height)) {
                return;
            }

            if (color == Color.White) {
                displayBuffer[x + (y / 8) * 128] |= (byte) (1 << (y % 8));
            }
            else {
                displayBuffer[x + (y / 8) * 128] &= (byte) ~(1 << (y % 8));
            }
        }

        protected void SendCommand(Command cmd) {
            SpiBuffer[0] = (byte)cmd;
            Spi.Write(SpiBuffer);
        }

        public void Refresh() {
            Spi.Write(displayBuffer);
        }

        public void ClearScreen() {
            displayBuffer[0] = 0;
            displayBuffer[1] = 0;
            displayBuffer[2] = 0;
            displayBuffer[3] = 0;
            displayBuffer[4] = 0;
            displayBuffer[5] = 0;
            displayBuffer[6] = 0;
            displayBuffer[7] = 0;
            displayBuffer[8] = 0;
            displayBuffer[9] = 0;
            displayBuffer[10] = 0;
            displayBuffer[11] = 0;
            displayBuffer[12] = 0;
            displayBuffer[13] = 0;
            displayBuffer[14] = 0;
            displayBuffer[15] = 0;
            Array.Copy(displayBuffer, 0, displayBuffer, 16, 16);
            Array.Copy(displayBuffer, 0, displayBuffer, 32, 32);
            Array.Copy(displayBuffer, 0, displayBuffer, 64, 64);
            Array.Copy(displayBuffer, 0, displayBuffer, 128, 128);
            Array.Copy(displayBuffer, 0, displayBuffer, 256, 256);
            Array.Copy(displayBuffer, 0, displayBuffer, 512, 512);

            if (AutoRefreshScreen) {
                Refresh();
            }
        }

        public void Dispose() {
            dcPin.Dispose();
            resetPin.Dispose();
            Spi.Dispose();

            dcPin = null;
            resetPin = null;
            Spi = null;
            SpiBuffer = null;
            displayBuffer = null;
        }

        public void Initialize(VccType vcctype = VccType.SWITCHCAPVCC) {
            resetPin.Write(true);
            Thread.Sleep(1); // VDD (3.3V) goes high at start, lets just chill for a ms
            resetPin.Write(false); // bring reset low
            Thread.Sleep(10); // wait 10ms
            resetPin.Write(true); // bring out of reset

            dcPin.Write(DisplayCommand);

            SendCommand(Command.DISPLAYOFF);  // 0xAE
            SendCommand(Command.SETLOWCOLUMN | (Command)0x0);  // low col = 0
            SendCommand(Command.SETHIGHCOLUMN | (Command)0x0);  // hi col = 0
            SendCommand(Command.SETSTARTLINE | (Command)0x0); // line #0
            SendCommand(Command.SETCONTRAST);  // 0x81

            if (vcctype == VccType.EXTERNALVCC) {
                SendCommand((Command)0x9F);  // external 9V
            } else {
                SendCommand((Command)0xCF);  // chargepump
            }

            SendCommand((Command)0xA1);  // setment remap 95 to 0 (?)
            SendCommand(Command.NORMALDISPLAY); // 0xA6
            SendCommand(Command.DISPLAYALLON_RESUME); // 0xA4
            SendCommand(Command.SETMULTIPLEX); // 0xA8
            SendCommand((Command)0x3F);  // 0x3F 1/64 duty
            SendCommand(Command.SETDISPLAYOFFSET); // 0xD3
            SendCommand((Command)0x0); // no offset
            SendCommand(Command.SETDISPLAYCLOCKDIV);  // 0xD5
            SendCommand((Command)0x80);  // the suggested ratio 0x80
            SendCommand(Command.SETPRECHARGE); // 0xd9

            if (vcctype == VccType.EXTERNALVCC) {
                SendCommand((Command)0x22); // external 9V
            } else {
                SendCommand((Command)0xF1); // DC/DC
            }

            SendCommand(Command.SETCOMPINS); // 0xDA
            SendCommand((Command)0x12); // disable COM left/right remap

            SendCommand(Command.SETVCOMDETECT); // 0xDB
            SendCommand((Command)0x40); // 0x20 is default?

            SendCommand(Command.MEMORYMODE); // 0x20
            SendCommand((Command)0x00); // 0x0 act like ks0108

            // left to right scan
            SendCommand(Command.SEGREMAP | (Command)0x1);

            SendCommand(Command.COMSCANDEC);

            SendCommand(Command.CHARGEPUMP); //0x8D
            if (vcctype == VccType.EXTERNALVCC) {
                SendCommand((Command)0x10);  // disable
            } else {
                SendCommand((Command)0x14);  // disable    
            }

            SendCommand(Command.DISPLAYON);//--turn on oled panel

            // Switch to 'data' mode
            dcPin.Write(Data);
        }

        protected void Swap(ref int a, ref int b) {
            var t = a; a = b; b = t;
        }

        protected OutputPort dcPin;
        protected OutputPort resetPin;

        protected SPI Spi;
        protected byte[] SpiBuffer = new byte[1];

        public const int bufferSize = 1024;
        public byte[] displayBuffer = new byte[bufferSize];

        public static readonly byte[] Font = new byte[] {
            0x00, 0x00, 0x00, 0x00, 0x00,   
	        0x3E, 0x5B, 0x4F, 0x5B, 0x3E, 	
	        0x3E, 0x6B, 0x4F, 0x6B, 0x3E, 	
	        0x1C, 0x3E, 0x7C, 0x3E, 0x1C, 
	        0x18, 0x3C, 0x7E, 0x3C, 0x18, 
	        0x1C, 0x57, 0x7D, 0x57, 0x1C, 
	        0x1C, 0x5E, 0x7F, 0x5E, 0x1C, 
	        0x00, 0x18, 0x3C, 0x18, 0x00, 
	        0xFF, 0xE7, 0xC3, 0xE7, 0xFF, 
	        0x00, 0x18, 0x24, 0x18, 0x00, 
	        0xFF, 0xE7, 0xDB, 0xE7, 0xFF, 
	        0x30, 0x48, 0x3A, 0x06, 0x0E, 
	        0x26, 0x29, 0x79, 0x29, 0x26, 
	        0x40, 0x7F, 0x05, 0x05, 0x07, 
	        0x40, 0x7F, 0x05, 0x25, 0x3F, 
	        0x5A, 0x3C, 0xE7, 0x3C, 0x5A, 
	        0x7F, 0x3E, 0x1C, 0x1C, 0x08, 
	        0x08, 0x1C, 0x1C, 0x3E, 0x7F, 
	        0x14, 0x22, 0x7F, 0x22, 0x14, 
	        0x5F, 0x5F, 0x00, 0x5F, 0x5F, 
	        0x06, 0x09, 0x7F, 0x01, 0x7F, 
	        0x00, 0x66, 0x89, 0x95, 0x6A, 
	        0x60, 0x60, 0x60, 0x60, 0x60, 
	        0x94, 0xA2, 0xFF, 0xA2, 0x94, 
	        0x08, 0x04, 0x7E, 0x04, 0x08, 
	        0x10, 0x20, 0x7E, 0x20, 0x10, 
	        0x08, 0x08, 0x2A, 0x1C, 0x08, 
	        0x08, 0x1C, 0x2A, 0x08, 0x08, 
	        0x1E, 0x10, 0x10, 0x10, 0x10, 
	        0x0C, 0x1E, 0x0C, 0x1E, 0x0C, 
	        0x30, 0x38, 0x3E, 0x38, 0x30, 
	        0x06, 0x0E, 0x3E, 0x0E, 0x06, 
	        0x00, 0x00, 0x00, 0x00, 0x00, 
	        0x00, 0x00, 0x5F, 0x00, 0x00, 
	        0x00, 0x07, 0x00, 0x07, 0x00, 
	        0x14, 0x7F, 0x14, 0x7F, 0x14, 
	        0x24, 0x2A, 0x7F, 0x2A, 0x12, 
	        0x23, 0x13, 0x08, 0x64, 0x62, 
	        0x36, 0x49, 0x56, 0x20, 0x50, 
	        0x00, 0x08, 0x07, 0x03, 0x00, 
	        0x00, 0x1C, 0x22, 0x41, 0x00, 
	        0x00, 0x41, 0x22, 0x1C, 0x00, 
	        0x2A, 0x1C, 0x7F, 0x1C, 0x2A, 
	        0x08, 0x08, 0x3E, 0x08, 0x08, 
	        0x00, 0x80, 0x70, 0x30, 0x00, 
	        0x08, 0x08, 0x08, 0x08, 0x08, 
	        0x00, 0x00, 0x60, 0x60, 0x00, 
	        0x20, 0x10, 0x08, 0x04, 0x02, 
	        0x3E, 0x51, 0x49, 0x45, 0x3E, 
	        0x00, 0x42, 0x7F, 0x40, 0x00, 
	        0x72, 0x49, 0x49, 0x49, 0x46, 
	        0x21, 0x41, 0x49, 0x4D, 0x33, 
	        0x18, 0x14, 0x12, 0x7F, 0x10, 
	        0x27, 0x45, 0x45, 0x45, 0x39, 
	        0x3C, 0x4A, 0x49, 0x49, 0x31, 
	        0x41, 0x21, 0x11, 0x09, 0x07, 
	        0x36, 0x49, 0x49, 0x49, 0x36, 
	        0x46, 0x49, 0x49, 0x29, 0x1E, 
	        0x00, 0x00, 0x14, 0x00, 0x00, 
	        0x00, 0x40, 0x34, 0x00, 0x00, 
	        0x00, 0x08, 0x14, 0x22, 0x41, 
	        0x14, 0x14, 0x14, 0x14, 0x14, 
	        0x00, 0x41, 0x22, 0x14, 0x08, 
	        0x02, 0x01, 0x59, 0x09, 0x06, 
	        0x3E, 0x41, 0x5D, 0x59, 0x4E, 
	        0x7C, 0x12, 0x11, 0x12, 0x7C, 
	        0x7F, 0x49, 0x49, 0x49, 0x36, 
	        0x3E, 0x41, 0x41, 0x41, 0x22, 
	        0x7F, 0x41, 0x41, 0x41, 0x3E, 
	        0x7F, 0x49, 0x49, 0x49, 0x41, 
	        0x7F, 0x09, 0x09, 0x09, 0x01, 
	        0x3E, 0x41, 0x41, 0x51, 0x73, 
	        0x7F, 0x08, 0x08, 0x08, 0x7F, 
	        0x00, 0x41, 0x7F, 0x41, 0x00, 
	        0x20, 0x40, 0x41, 0x3F, 0x01, 
	        0x7F, 0x08, 0x14, 0x22, 0x41, 
	        0x7F, 0x40, 0x40, 0x40, 0x40, 
	        0x7F, 0x02, 0x1C, 0x02, 0x7F, 
	        0x7F, 0x04, 0x08, 0x10, 0x7F, 
	        0x3E, 0x41, 0x41, 0x41, 0x3E, 
	        0x7F, 0x09, 0x09, 0x09, 0x06, 
	        0x3E, 0x41, 0x51, 0x21, 0x5E, 
	        0x7F, 0x09, 0x19, 0x29, 0x46, 
	        0x26, 0x49, 0x49, 0x49, 0x32, 
	        0x03, 0x01, 0x7F, 0x01, 0x03, 
	        0x3F, 0x40, 0x40, 0x40, 0x3F, 
	        0x1F, 0x20, 0x40, 0x20, 0x1F, 
	        0x3F, 0x40, 0x38, 0x40, 0x3F, 
	        0x63, 0x14, 0x08, 0x14, 0x63, 
	        0x03, 0x04, 0x78, 0x04, 0x03, 
	        0x61, 0x59, 0x49, 0x4D, 0x43, 
	        0x00, 0x7F, 0x41, 0x41, 0x41, 
	        0x02, 0x04, 0x08, 0x10, 0x20, 
	        0x00, 0x41, 0x41, 0x41, 0x7F, 
	        0x04, 0x02, 0x01, 0x02, 0x04, 
	        0x40, 0x40, 0x40, 0x40, 0x40, 
	        0x00, 0x03, 0x07, 0x08, 0x00, 
	        0x20, 0x54, 0x54, 0x78, 0x40, 
	        0x7F, 0x28, 0x44, 0x44, 0x38, 
	        0x38, 0x44, 0x44, 0x44, 0x28, 
	        0x38, 0x44, 0x44, 0x28, 0x7F, 
	        0x38, 0x54, 0x54, 0x54, 0x18, 
	        0x00, 0x08, 0x7E, 0x09, 0x02, 
	        0x18, 0xA4, 0xA4, 0x9C, 0x78, 
	        0x7F, 0x08, 0x04, 0x04, 0x78, 
	        0x00, 0x44, 0x7D, 0x40, 0x00, 
	        0x20, 0x40, 0x40, 0x3D, 0x00, 
	        0x7F, 0x10, 0x28, 0x44, 0x00, 
	        0x00, 0x41, 0x7F, 0x40, 0x00, 
	        0x7C, 0x04, 0x78, 0x04, 0x78, 
	        0x7C, 0x08, 0x04, 0x04, 0x78, 
	        0x38, 0x44, 0x44, 0x44, 0x38, 
	        0xFC, 0x18, 0x24, 0x24, 0x18, 
	        0x18, 0x24, 0x24, 0x18, 0xFC, 
	        0x7C, 0x08, 0x04, 0x04, 0x08, 
	        0x48, 0x54, 0x54, 0x54, 0x24, 
	        0x04, 0x04, 0x3F, 0x44, 0x24, 
	        0x3C, 0x40, 0x40, 0x20, 0x7C, 
	        0x1C, 0x20, 0x40, 0x20, 0x1C, 
	        0x3C, 0x40, 0x30, 0x40, 0x3C, 
	        0x44, 0x28, 0x10, 0x28, 0x44, 
	        0x4C, 0x90, 0x90, 0x90, 0x7C, 
	        0x44, 0x64, 0x54, 0x4C, 0x44, 
	        0x00, 0x08, 0x36, 0x41, 0x00, 
	        0x00, 0x00, 0x77, 0x00, 0x00, 
	        0x00, 0x41, 0x36, 0x08, 0x00, 
	        0x02, 0x01, 0x02, 0x04, 0x02, 
	        0x3C, 0x26, 0x23, 0x26, 0x3C, 
	        0x1E, 0xA1, 0xA1, 0x61, 0x12, 
	        0x3A, 0x40, 0x40, 0x20, 0x7A, 
	        0x38, 0x54, 0x54, 0x55, 0x59, 
	        0x21, 0x55, 0x55, 0x79, 0x41, 
	        0x21, 0x54, 0x54, 0x78, 0x41, 
	        0x21, 0x55, 0x54, 0x78, 0x40, 
	        0x20, 0x54, 0x55, 0x79, 0x40, 
	        0x0C, 0x1E, 0x52, 0x72, 0x12, 
	        0x39, 0x55, 0x55, 0x55, 0x59, 
	        0x39, 0x54, 0x54, 0x54, 0x59, 
	        0x39, 0x55, 0x54, 0x54, 0x58, 
	        0x00, 0x00, 0x45, 0x7C, 0x41, 
	        0x00, 0x02, 0x45, 0x7D, 0x42, 
	        0x00, 0x01, 0x45, 0x7C, 0x40, 
	        0xF0, 0x29, 0x24, 0x29, 0xF0, 
	        0xF0, 0x28, 0x25, 0x28, 0xF0, 
	        0x7C, 0x54, 0x55, 0x45, 0x00, 
	        0x20, 0x54, 0x54, 0x7C, 0x54, 
	        0x7C, 0x0A, 0x09, 0x7F, 0x49, 
	        0x32, 0x49, 0x49, 0x49, 0x32, 
	        0x32, 0x48, 0x48, 0x48, 0x32, 
	        0x32, 0x4A, 0x48, 0x48, 0x30, 
	        0x3A, 0x41, 0x41, 0x21, 0x7A, 
	        0x3A, 0x42, 0x40, 0x20, 0x78, 
	        0x00, 0x9D, 0xA0, 0xA0, 0x7D, 
	        0x39, 0x44, 0x44, 0x44, 0x39, 
	        0x3D, 0x40, 0x40, 0x40, 0x3D, 
	        0x3C, 0x24, 0xFF, 0x24, 0x24, 
	        0x48, 0x7E, 0x49, 0x43, 0x66, 
	        0x2B, 0x2F, 0xFC, 0x2F, 0x2B, 
	        0xFF, 0x09, 0x29, 0xF6, 0x20, 
	        0xC0, 0x88, 0x7E, 0x09, 0x03, 
	        0x20, 0x54, 0x54, 0x79, 0x41, 
	        0x00, 0x00, 0x44, 0x7D, 0x41, 
	        0x30, 0x48, 0x48, 0x4A, 0x32, 
	        0x38, 0x40, 0x40, 0x22, 0x7A, 
	        0x00, 0x7A, 0x0A, 0x0A, 0x72, 
	        0x7D, 0x0D, 0x19, 0x31, 0x7D, 
	        0x26, 0x29, 0x29, 0x2F, 0x28, 
	        0x26, 0x29, 0x29, 0x29, 0x26, 
	        0x30, 0x48, 0x4D, 0x40, 0x20, 
	        0x38, 0x08, 0x08, 0x08, 0x08, 
	        0x08, 0x08, 0x08, 0x08, 0x38, 
	        0x2F, 0x10, 0xC8, 0xAC, 0xBA, 
	        0x2F, 0x10, 0x28, 0x34, 0xFA, 
	        0x00, 0x00, 0x7B, 0x00, 0x00, 
	        0x08, 0x14, 0x2A, 0x14, 0x22, 
	        0x22, 0x14, 0x2A, 0x14, 0x08, 
	        0xAA, 0x00, 0x55, 0x00, 0xAA, 
	        0xAA, 0x55, 0xAA, 0x55, 0xAA, 
	        0x00, 0x00, 0x00, 0xFF, 0x00, 
	        0x10, 0x10, 0x10, 0xFF, 0x00, 
	        0x14, 0x14, 0x14, 0xFF, 0x00, 
	        0x10, 0x10, 0xFF, 0x00, 0xFF, 
	        0x10, 0x10, 0xF0, 0x10, 0xF0, 
	        0x14, 0x14, 0x14, 0xFC, 0x00, 
	        0x14, 0x14, 0xF7, 0x00, 0xFF, 
	        0x00, 0x00, 0xFF, 0x00, 0xFF, 
	        0x14, 0x14, 0xF4, 0x04, 0xFC, 
	        0x14, 0x14, 0x17, 0x10, 0x1F, 
	        0x10, 0x10, 0x1F, 0x10, 0x1F, 
	        0x14, 0x14, 0x14, 0x1F, 0x00, 
	        0x10, 0x10, 0x10, 0xF0, 0x00, 
	        0x00, 0x00, 0x00, 0x1F, 0x10, 
	        0x10, 0x10, 0x10, 0x1F, 0x10, 
	        0x10, 0x10, 0x10, 0xF0, 0x10, 
	        0x00, 0x00, 0x00, 0xFF, 0x10, 
	        0x10, 0x10, 0x10, 0x10, 0x10, 
	        0x10, 0x10, 0x10, 0xFF, 0x10, 
	        0x00, 0x00, 0x00, 0xFF, 0x14, 
	        0x00, 0x00, 0xFF, 0x00, 0xFF, 
	        0x00, 0x00, 0x1F, 0x10, 0x17, 
	        0x00, 0x00, 0xFC, 0x04, 0xF4, 
	        0x14, 0x14, 0x17, 0x10, 0x17, 
	        0x14, 0x14, 0xF4, 0x04, 0xF4, 
	        0x00, 0x00, 0xFF, 0x00, 0xF7, 
	        0x14, 0x14, 0x14, 0x14, 0x14, 
	        0x14, 0x14, 0xF7, 0x00, 0xF7, 
	        0x14, 0x14, 0x14, 0x17, 0x14, 
	        0x10, 0x10, 0x1F, 0x10, 0x1F, 
	        0x14, 0x14, 0x14, 0xF4, 0x14, 
	        0x10, 0x10, 0xF0, 0x10, 0xF0, 
	        0x00, 0x00, 0x1F, 0x10, 0x1F, 
	        0x00, 0x00, 0x00, 0x1F, 0x14, 
	        0x00, 0x00, 0x00, 0xFC, 0x14, 
	        0x00, 0x00, 0xF0, 0x10, 0xF0, 
	        0x10, 0x10, 0xFF, 0x10, 0xFF, 
	        0x14, 0x14, 0x14, 0xFF, 0x14, 
	        0x10, 0x10, 0x10, 0x1F, 0x00, 
	        0x00, 0x00, 0x00, 0xF0, 0x10, 
	        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	        0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 
	        0xFF, 0xFF, 0xFF, 0x00, 0x00, 
	        0x00, 0x00, 0x00, 0xFF, 0xFF, 
	        0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 
	        0x38, 0x44, 0x44, 0x38, 0x44, 
	        0x7C, 0x2A, 0x2A, 0x3E, 0x14, 
	        0x7E, 0x02, 0x02, 0x06, 0x06, 
	        0x02, 0x7E, 0x02, 0x7E, 0x02, 
	        0x63, 0x55, 0x49, 0x41, 0x63, 
	        0x38, 0x44, 0x44, 0x3C, 0x04, 
	        0x40, 0x7E, 0x20, 0x1E, 0x20, 
	        0x06, 0x02, 0x7E, 0x02, 0x02, 
	        0x99, 0xA5, 0xE7, 0xA5, 0x99, 
	        0x1C, 0x2A, 0x49, 0x2A, 0x1C, 
	        0x4C, 0x72, 0x01, 0x72, 0x4C, 
	        0x30, 0x4A, 0x4D, 0x4D, 0x30, 
	        0x30, 0x48, 0x78, 0x48, 0x30, 
	        0xBC, 0x62, 0x5A, 0x46, 0x3D, 
	        0x3E, 0x49, 0x49, 0x49, 0x00, 
	        0x7E, 0x01, 0x01, 0x01, 0x7E, 
	        0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 
	        0x44, 0x44, 0x5F, 0x44, 0x44, 
	        0x40, 0x51, 0x4A, 0x44, 0x40, 
	        0x40, 0x44, 0x4A, 0x51, 0x40, 
	        0x00, 0x00, 0xFF, 0x01, 0x03, 
	        0xE0, 0x80, 0xFF, 0x00, 0x00, 
	        0x08, 0x08, 0x6B, 0x6B, 0x08,
	        0x36, 0x12, 0x36, 0x24, 0x36, 
	        0x06, 0x0F, 0x09, 0x0F, 0x06, 
	        0x00, 0x00, 0x18, 0x18, 0x00, 
	        0x00, 0x00, 0x10, 0x10, 0x00, 
	        0x30, 0x40, 0xFF, 0x01, 0x01, 
	        0x00, 0x1F, 0x01, 0x01, 0x1E, 
	        0x00, 0x19, 0x1D, 0x17, 0x12, 
	        0x00, 0x3C, 0x3C, 0x3C, 0x3C, 
	        0x00, 0x00, 0x00, 0x00, 0x00
            };
    }
}
