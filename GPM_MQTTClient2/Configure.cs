using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;
using System.Management;
using System.IO;
using System.Runtime.InteropServices;

namespace GPM_MQTTClient2
{
    /// <summary>
    /// Application GUI Window for configuring the Device
    /// </summary>
    public partial class Configure : Form
    {
        static SerialPort port;
        //internal Thread readThread;
        static bool connected = false;
        public StringBuilder name;
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
        List<USBDeviceInfo> devices = new List<USBDeviceInfo>();

        /// <summary> </summary>
        public class DeviceSettings
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
        public DeviceSettings deviceSettings = new DeviceSettings();
        //internal DeviceConfig deviceConfig = new DeviceConfig();
        internal static Configure thisInstance;

        public delegate void ResponseDelegate(string s);

        [DllImport(@"GPM_USBInterface.dll", EntryPoint = "Foo", CallingConvention = CallingConvention.StdCall)]
        public static extern void Foo(string str, ResponseDelegate response);
 

        /// <summary>
        /// Constructor method for the GUI class
        /// </summary>
        public Configure()
        {
            InitializeComponent();
            thisInstance = this;
            name = new StringBuilder(100);

            List<USBDeviceInfo> usbdevices = new List<USBDeviceInfo>();
            ManagementObjectSearcher searcher =
                new ManagementObjectSearcher("root\\CIMV2",
                "SELECT * FROM Win32_PnPEntity");
            foreach (ManagementObject queryObj in searcher.Get())
            {
                usbdevices.Add(new USBDeviceInfo(
                    (string)queryObj["DeviceID"],
                    (string)queryObj["PNPDeviceID"],
                    (string)queryObj["Name"]
                ));
            }

            foreach (USBDeviceInfo usbDevice in usbdevices)
            {
                if (usbDevice.name != null)
                {
                    int iCOM = usbDevice.name.IndexOf("(COM");
                    if (iCOM>=0) //use your own device's name
                    {
                        devices.Add(usbDevice);
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

        /// <summary>
        /// Update the fields in the Windows Form with the values in the configuration record
        /// </summary>
        public void updateGUI()
        {
            //txtName.Text = DeviceConfig.deviceConfig.pm_name.ToString();
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

        private void Connect(int com)
        {
            Thread readThread = new Thread(Read);
            port = new SerialPort();

            // Allow the user to set the appropriate properties.
            port.PortName = "COM"+devices[com].com.ToString();
            port.BaudRate = 115200;
            port.Parity = Parity.None;
            port.DataBits = 8;
            port.StopBits = StopBits.One;
            port.Handshake  = Handshake.None;

            // Set the read/write timeouts
            port.ReadTimeout = 500;
            port.WriteTimeout = 500;

            port.Open();
            connected = true;
            readThread.Start();
        }

        private static void Read()
        {
            while (connected)
            {
                try
                {
                    string message = port.ReadLine();
                    Console.WriteLine(message);

                    if (message.StartsWith("SP#"))
                    {
                        DeviceConfig.Parse(message);

                        thisInstance.BeginInvoke(new MethodInvoker(delegate
                        {
                            thisInstance.updateGUI();
                        }));
                    }
                }
                catch (TimeoutException toe) { }
                catch (System.IO.IOException ioe) { }
                catch (Exception e) { }
            }
        }

        private void butStore_Click(object sender, EventArgs e)
        {
            Foo("Input", s =>
            {
                Console.WriteLine(s);
            });
        }

        private void butRetrieve_Click(object sender, EventArgs e)
        {
            if (!connected) Connect(cbCOMList.SelectedIndex);
            if (connected)
            {
                port.Write("?v\r");
            }
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
                    // load deviceConfig
                    BinaryReader reader = new BinaryReader(File.Open(openFileDialog.FileName, FileMode.Open));
                    reader.Read(DeviceConfig.devmsg.data, 0, 1000);
                }
            }
        }

        private void butSave_Click(object sender, EventArgs e)
        {
            //string confDoc = JsonSerializer.Serialize(deviceSettings);

            using (SaveFileDialog saveFileDialog = new SaveFileDialog())
            {
                //openFileDialog.InitialDirectory = "c:\\";
                saveFileDialog.Filter = "Configuration files (*.xml)|*.xml|All files (*.*)|*.*";
                saveFileDialog.FilterIndex = 1;
                saveFileDialog.RestoreDirectory = true;

                if (saveFileDialog.ShowDialog() == DialogResult.OK)
                {
                    // save deviceConfig
                    BinaryWriter writer = new BinaryWriter(File.Open(saveFileDialog.FileName, FileMode.Create));
                    writer.Write(DeviceConfig.devmsg.data, 0, 1000);
                }
            }

        }

        private void Configure_FormClosed(object sender, FormClosedEventArgs e)
        {
            if (connected)
            {
                connected = false;
                port.Close();
            }
        }
    }
}
