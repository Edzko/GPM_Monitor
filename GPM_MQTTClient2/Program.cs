using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;

/// <summary>
/// GPM_MQTTClient defines a Client Application to an MQTT broker for managing vibration 
/// data acquisition devices.
/// </summary>
namespace GPM_MQTTClient2
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new VibManager());
        }
    }
}
