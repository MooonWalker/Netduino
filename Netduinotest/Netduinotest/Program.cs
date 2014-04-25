using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using SecretLabs.NETMF.Hardware;
using SecretLabs.NETMF.Hardware.NetduinoPlus;
using Microsoft.SPOT;
using Microsoft.SPOT.Hardware;


namespace Netduinotest
{
    public class Program
    {
        private static Thread mainThread;
        private static Thread ledThread;

        static OutputPort led = new OutputPort(Pins.ONBOARD_LED, false);
        static bool togl = false;

        public static void Main()
        {
            mainThread = Thread.CurrentThread;
            ledThread = new Thread(ledThreadMethod);

            InterruptPort button = new InterruptPort(Pins.ONBOARD_SW1, false, Port.ResistorMode.Disabled,
                Port.InterruptMode.InterruptEdgeHigh);
            button.OnInterrupt += new NativeEventHandler(button_OnInterrupt);

           Thread.Sleep(-1);         
            //mainThread.Suspend();
        }

        static void button_OnInterrupt(uint data1, uint data2, DateTime time)
        {          
         
            togl = !togl;
            if (!ledThread.IsAlive)
            {
                ledThread.Start();
            }
            else
            {
                ledThread = new Thread(ledThreadMethod);
            }
            
        }

        private static void ledThreadMethod()
        {
            //dimLed(togl);
            //mainThread.Resume();
            while (togl)
            {
                        led.Write(true);
                        funDelay(50);

                        led.Write(false);
                        funDelay(200);
            }
            Debug.Print("Bye from worker thread");
            led.Write(false);
            
        }


        static private void funDelay(long lTicks)
        {
            DateTime tNow = DateTime.Now;
            DateTime tDue = tNow.AddTicks(lTicks * 0x100);
            while (DateTime.Now < tDue) ;

            return;
        }

        static void dimLed(bool _toggl)
        {
                    //while (true)
                    //{
                    //    led.Write(true);
                    //    funDelay(50);

                    //    led.Write(false);
                    //    funDelay(200);
                    //}
           
                    
           

            
        }
        
    }
}
