using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using Microsoft.SPOT;
using Microsoft.SPOT.Hardware;
using SecretLabs.NETMF.Hardware;
using SecretLabs.NETMF.Hardware.NetduinoPlus;

namespace breathing_led
{
    public class Program
    {
        public static void Main()
        {
              // write your code here
            OutputPort ledPort = new OutputPort(Pins.ONBOARD_LED, false);

            int iMax=255, iMin=1;
            int i = iMin;

            while (true)
            {
                                    
                    ledPort.Write(true);
                    funDelay(50);

                    ledPort.Write(false);
                    //funDelay(iMax - i);                    
                    funDelay(200);                    
               

                //for (; i > iMin; i--)
                //{                    
                //    ledPort.Write(true);
                //    funDelay(i);

                //    ledPort.Write(false);
                //    funDelay(iMax - i);                    
                //}
                //funDelay(10000);
            }
        }

        static private void funDelay(long lTicks)
        {            
            DateTime tNow = DateTime.Now;
            DateTime tDue = tNow.AddTicks(lTicks*0x100);
            while (DateTime.Now < tDue) ;

            return;
        }
    }
}


        