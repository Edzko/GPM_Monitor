using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;
using System.Management;

namespace GPM_MQTTClient2
{
    /// <summary>
    /// Application GUI Window for configuring the Device
    /// </summary>
    public partial class Configure : Form
    {

        /// <summary> </summary>
        public class USBDeviceInfo
        {
            /// <summary> </summary>
            public int com;
            /// <summary> </summary>
            public string id;
            /// <summary> </summary>
            public string pnp;
            /// <summary> </summary>
            public string name;
            /// <summary> </summary>
            public string Description;

            /// <summary> </summary>
            public USBDeviceInfo(string _id, string _pnp, string _name)
            {
                id = _id;
                pnp = _pnp;
                name = _name;

                //System.Diagnostics.Debug.WriteLine("Found USB: " + name + " (" + id + ")");
            }
        }

        /// <summary> </summary>
        public class Device
        {
            /// <summary> </summary>
            public string name;
            /// <summary> </summary>
            public string firmware;
            /// <summary> </summary>
            public class Vibration
            {
                /// <summary> </summary>
                public int samplerate;
                /// <summary> </summary>
                public int samplesize;
                /// <summary> </summary>
                public int axis;
                /// <summary> </summary>
                public int subsample;
                
            }
            /// <summary> </summary>
            public class MQTT
            {
                /// <summary> </summary>
                public string host;
                /// <summary> </summary>
                public int port;
                /// <summary> </summary>
                public string username;
                /// <summary> </summary>
                public string password;
                /// <summary> </summary>
                public string topic;
                /// <summary> </summary>
                public int interval;
            }
            /// <summary> </summary>
            public class WIFI
            {
                /// <summary> </summary>
                public string ssid;
                /// <summary> </summary>
                public string password;
            }
        }
        /// <summary> </summary>
        public Device device = new Device();

        /// <summary>
        /// Constructor method for the GUI class
        /// </summary>
        public Configure()
        {
            InitializeComponent();

            //show list of valid com ports
            foreach (string s in SerialPort.GetPortNames())
            {
                
                //cbCOMList.Items.Add(s);
            }


            List<USBDeviceInfo> devices = new List<USBDeviceInfo>();
            ManagementObjectSearcher searcher =
                new ManagementObjectSearcher("root\\CIMV2",
                "SELECT * FROM Win32_PnPEntity");
            foreach (ManagementObject queryObj in searcher.Get())
            {
                devices.Add(new USBDeviceInfo(
                    (string)queryObj["DeviceID"],
                    (string)queryObj["PNPDeviceID"],
                    (string)queryObj["Name"]
                ));
            }

            foreach (USBDeviceInfo usbDevice in devices)
            {
                if (usbDevice.name != null)
                {
                    int iCOM = usbDevice.name.IndexOf("(COM");
                    if (iCOM>=0) //use your own device's name
                    {

                        usbDevice.com = usbDevice.name[iCOM + 4]-'0';
                        if (usbDevice.name[iCOM + 5] != ')') usbDevice.com = usbDevice.com * 10 + usbDevice.name[iCOM + 5] - '0';
                        System.Diagnostics.Debug.WriteLine("Found USB: " + usbDevice.name + " at COM=" + usbDevice.com.ToString());
                        cbCOMList.Items.Add(usbDevice.name);
                    }
                }
            }

        }

        

        private void butClose_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void butHelp_Click(object sender, EventArgs e)
        {
            //open Help

            string p1 = Environment.CurrentDirectory;
            string p2 = System.AppDomain.CurrentDomain.BaseDirectory;
            string helpFileName = @"GPM_MQTTClient2.chm";
            if (System.IO.File.Exists(helpFileName))
            {
                Help.ShowHelp(this, "file:" + p1 + "\\" + helpFileName, "mqttclient_intro.htm");
            }

            //Help.ShowHelp(null, @".\JazzManager.chm", @"html\jazzmgr_intro.htm");
        }

        private void butStore_Click(object sender, EventArgs e)
        {

        }

        private void butRetrieve_Click(object sender, EventArgs e)
        {

        }

        private void butConnect_Click(object sender, EventArgs e)
        {

        }

        private void butLoad_Click(object sender, EventArgs e)
        {
            using (OpenFileDialog openFileDialog = new OpenFileDialog())
            {
                //openFileDialog.InitialDirectory = "c:\\";
                openFileDialog.Filter = "firmware files (*.bin)|*.bin|All files (*.*)|*.*";
                openFileDialog.FilterIndex = 1;
                openFileDialog.RestoreDirectory = true;

                if (openFileDialog.ShowDialog() == DialogResult.OK)
                {
                    // call Firmware Upgrade with 
                    //System.Diagnostics.Process.Start("CMD.exe", "/C GPM_UpdateLite.exe " + devices[idev].ip + " " + openFileDialog.FileName);
                    //Update(devices[idev].ip, openFileDialog.FileName);
                }
            }
        }

        private void butSave_Click(object sender, EventArgs e)
        {
            string confDoc = JsonSerializer.Serialize(device);

            using (SaveFileDialog saveFileDialog = new SaveFileDialog())
            {
                //openFileDialog.InitialDirectory = "c:\\";
                saveFileDialog.Filter = "Configuration files (*.xml)|*.xml|All files (*.*)|*.*";
                saveFileDialog.FilterIndex = 1;
                saveFileDialog.RestoreDirectory = true;

                if (saveFileDialog.ShowDialog() == DialogResult.OK)
                {
                    // call Firmware Upgrade with 
                    //System.Diagnostics.Process.Start("CMD.exe", "/C GPM_UpdateLite.exe " + devices[idev].ip + " " + openFileDialog.FileName);
                    //Update(devices[idev].ip, openFileDialog.FileName);
                }
            }

        }
    }
}
