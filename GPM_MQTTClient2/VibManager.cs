using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;

using uPLibrary.Networking.M2Mqtt;
using uPLibrary.Networking.M2Mqtt.Messages;

namespace GPM_MQTTClient2
{
    
    /// <summary>
    /// class definition for the Main Application form
    /// </summary>
    public partial class VibManager : Form
    {
        /// <summary>MQTT security access token</summary>
        public static String access_token = "no valid token received yet.";
        /// <summary>MQTT client instance</summary>
        public static MqttClient client;
        /// <summary>MQTT Client access unique identifier</summary>
        public static string clientId;
        internal bool guiUpdate = false;
        internal int idev = 0;
        internal int ichart = -1;
        List<string> topiclist = new List<string>();
        RegistryKey key;

        const string userRoot = "HKEY_CURRENT_USER";
        const string subkey = "Software\\Magna\\MQTTClient";
        const string keyName = userRoot + "\\" + subkey;

        internal class DeviceData
        {
            internal string name;
            internal string topic;
            internal bool valid;
            internal string firmware;
            internal string timestamp;
            internal List<double> rms;
            internal List<DateTime> rmstime;
            internal string ip;
            internal double scale;
            internal int window;
            internal int axes;
            internal int rate;
            internal int samples;
            internal int interval;
            internal int pnts;
            internal double[] FFTmode;
            internal double[] FFToct;
            internal double[,] FFTvalues;
        }
        internal List<DeviceData> devices = new List<DeviceData>();

        // ID for the About item on the system menu
        private int SYSMENU_ABOUT_ID = 0x1;
        private int SYSMENU_LOGFILE_ID = 0x2;
        private int SYSMENU_UPDATE_ID = 0x3;

        /// <summary>
        /// String value for the complete path and file name for the log file.
        /// </summary>
        public string logFileName;

        /// <summary>
        /// Bitmap object that is used as canvas for the charts
        /// </summary>
        public Bitmap chartImg;
        private const int WM_SYSCOMMAND = 0x112;
        private const int MF_STRING = 0x0;
        private const int MF_SEPARATOR = 0x800;
        private const int MF_CHECKED = 0x008;

        // P/Invoke declarations
        [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        private static extern IntPtr GetSystemMenu(IntPtr hWnd, bool bRevert);

        [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        private static extern bool AppendMenu(IntPtr hMenu, int uFlags, int uIDNewItem, string lpNewItem);

        [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        private static extern bool InsertMenu(IntPtr hMenu, int uPosition, int uFlags, int uIDNewItem, string lpNewItem);



        /// <summary>
        /// Initialization call for the Main User Interface form
        /// </summary>
        public VibManager()
        {
            InitializeComponent();
            
            VersionInfo myInfo = new VersionInfo();

            //AboutBox about = new AboutBox();
            //about.Show();
            //about.Update();
            //System.Threading.Thread.Sleep(100);

            client = new MqttClient(txtHost.Text, Convert.ToInt32(txtPort.Text), false, MqttSslProtocols.SSLv3, null, null);
            // register a callback-function (we have to implement, see below) which is called by the library when a message was received
            client.MqttMsgPublishReceived += client_MqttMsgPublishReceived;
            client.ConnectionClosed += Client_ConnectionClosed;
            client.MqttMsgSubscribed += Client_MqttMsgSubscribed;

            vibChart.Series["Series1"].ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;

            key = Registry.CurrentUser.CreateSubKey(@"SOFTWARE\Magna\MQTTClient");

            txtHost.Text = (String)key.GetValue("Host", "oakmonte.homedns.org");
            txtPort.Text = (String)key.GetValue("Port", "1883");
            txtUser.Text = (String)key.GetValue("User", "Anonymous");
            txtPassword.Text = (String)key.GetValue("Password", "user12345");
        }

        /// <summary>
        /// System Override to add the "About" command to the system menu
        /// </summary>
        /// <param name="e"> Event arguments for the callback</param>
        protected override void OnHandleCreated(EventArgs e)
        {
            base.OnHandleCreated(e);

            // Get a handle to a copy of this form's system (window) menu
            IntPtr hSysMenu = GetSystemMenu(this.Handle, false);

            // Add a separator
            AppendMenu(hSysMenu, MF_SEPARATOR, 0, string.Empty);

            // Add the About menu item
            AppendMenu(hSysMenu, MF_STRING, SYSMENU_LOGFILE_ID, "&Log File");
            int AutoUpdate = (int)Registry.GetValue(keyName, "AutoUpdate", 1);
            if (AutoUpdate == 1)
                AppendMenu(hSysMenu, MF_STRING | MF_CHECKED, SYSMENU_UPDATE_ID, "Check &Update");
            else
                AppendMenu(hSysMenu, MF_STRING, SYSMENU_UPDATE_ID, "Check &Update");
            AppendMenu(hSysMenu, MF_STRING, SYSMENU_ABOUT_ID, "&About…");
        }

        /// <summary>
        /// System override to catch About Box Command message
        /// </summary>
        /// <param name="m">Message to be processed</param>
        protected override void WndProc(ref Message m)
        {
            base.WndProc(ref m);

            // Test if the About item was selected from the system menu
            if (m.Msg == WM_SYSCOMMAND)
            {
                if ((int)m.WParam == SYSMENU_ABOUT_ID)
                {
                    AboutBox pAbout = new AboutBox();
                    pAbout.ShowDialog();
                }
                if ((int)m.WParam == SYSMENU_LOGFILE_ID)
                {
                    Process.Start(logFileName);
                }
                if ((int)m.WParam == SYSMENU_UPDATE_ID)
                {
                    int AutoUpdate = (int)Registry.GetValue(keyName, "AutoUpdate", 1);
                    AutoUpdate = 1 - AutoUpdate;
                    Registry.SetValue(keyName, "AutoUpdate", AutoUpdate, RegistryValueKind.DWord);

                    //UpdatePtcm();
                }
            }
        }
        /// <summary>
        /// Internal Function call to request the current Application's version information
        /// </summary>
        public string AssemblyVersion
        {
            get
            {
                return Assembly.GetExecutingAssembly().GetName().Version.ToString();
            }
        }

        /// <summary>
        /// Callback function for the Connect/Disconnect Button.
        /// This function will create a connection to a MQTT broker as specified in the GUI.
        /// </summary>
        /// <param name="sender">Button object</param>
        /// <param name="e">Button click arguments</param>
        public void butConnect_Click(object sender, EventArgs e)
        {
            if (client.IsConnected)
            {
                client.Disconnect();
                butConnect.Text = "Connect";

                guiUpdate = true;
                topiclist.Clear();
                devices.Clear();
                cbTopics.Items.Clear();
                cbDevices.Items.Clear();
                txtRMS.Text = "";

                cbTopics.Text = "";
                cbDevices.Text = "";

                lblFirmware.Text = "Firmware: ";
                lblIP.Text = "IP Address: ";
                cbAxis.Text = "";
                cbWindow.Text = "";
                cbSamples.Text = "";
                cbRate.Text = "";
                cbScale.Text = "";
                cbInterval.Text = "";
                cbDSamp.Text = "";

                guiUpdate = false;

                cbChart.Text = "";
                vibChart.ChartAreas.Clear();
            }
            else
            {
                // use a unique id as client id, each time we start the application
                clientId = Guid.NewGuid().ToString();

                client.Connect(clientId, txtUser.Text, txtPassword.Text);

                client.Subscribe(new string[] { "#" }, new byte[] { MqttMsgBase.QOS_LEVEL_AT_MOST_ONCE });

                butConnect.Text = "Disconnect";

                key.GetValue("Host", txtHost.Text);
                key.GetValue("Port", txtPort.Text);
                key.GetValue("User", txtUser.Text);
                key.GetValue("Password", txtPassword.Text);
            }
        }

        /// <summary>
        /// When a message is received from the MQTT broker this function is called.
        /// It registers the topic and device with the Application database.
        /// </summary>
        /// <param name="topic">full topic string of the received message</param>
        /// <param name="dev">device name as registered in the message under the property "device".</param>
        public void addDevice(string topic, string dev)
        {

            // Add the current device to the global Devices list
            int idev = devices.FindIndex(x => x.name == dev);
            if (idev == -1)
            {
                DeviceData dd = new DeviceData();
                dd.name = dev;
                dd.topic = topic;
                dd.valid = false;
                dd.rms = new List<double>();
                dd.rmstime = new List<DateTime>();
                dd.FFTmode = new double[7];
                dd.FFToct = new double[7];
                dd.FFTvalues = new double[100, 128];
                devices.Add(dd);
                cbDevices.Items.Add(dev);
                if (devices.Count == 1) cbDevices.Text = dev;

                
            } 
            else if (devices[idev].valid==false)
            {
                // request parameters
                System.Threading.Thread.Sleep(500);
                System.Diagnostics.Debug.WriteLine("Requesting settings from " + devices[idev].name);
                int it = devices[idev].topic.LastIndexOf('/');
                if ((it > 0) && (devices[idev].topic.Length > 0))
                {
                    string top = devices[idev].topic.Substring(0, it);
                    client.Publish(top + '/' + devices[idev].name, Encoding.UTF8.GetBytes("{\"cmd\":\"sp*\"}"));
                }
            }
        }

        /// <summary>
        /// The function is called when a device message is received by the Client from the MQTT broker.
        /// The RMS and RMStime arrays are updated with the current value.
        /// If the device is selected in the GUI, then also the GUI values for the timestamp and RMS are updated.
        /// If the RMS chart is selected, then the chart is also updated.
        /// </summary>
        /// <param name="dev">Device name</param>
        /// <param name="rms">RMS value of the captured vibration dataset</param>
        /// <param name="time">Timestamp of the current dataset</param>
        public void updateDev(string dev, double rms, string time)
        {
            DateTime rmstime = DateTime.Parse(time);
            int idev = devices.FindIndex(x => x.name == dev);
            if (idev >= 0)
            {
                if (devices[idev].rms.Count < 1000)
                {
                    devices[idev].rms.Add(rms);
                    devices[idev].rmstime.Add(rmstime);
                }
                else
                {
                    for (int i = 0; i < 999; i++)
                        devices[idev].rms[i] = devices[idev].rms[i + 1];
                    devices[idev].rms[999] = rms;
                }
                devices[idev].timestamp = time;
            }
            try
            {
                if (cbDevices.Text == dev)
                {
                    txtRMS.Text = rms.ToString();
                    lblTime.Text = "Date and Time: " + time;
                    if (ichart == 0)
                    {
                        vibChart.Series["RMS"].Points.AddXY(rmstime, rms);
                        if (vibChart.Series["RMS"].Points.Count > 1000)
                            vibChart.Series["RMS"].Points.RemoveAt(0);

                        if (vibChart.Series["RMS"].Points.Count>200)
                            vibChart.ChartAreas["chart"].AxisX.LabelStyle.Format = "hh:mm";
                        else
                            vibChart.ChartAreas["chart"].AxisX.LabelStyle.Format = "hh:mm:ss";

                    }
                }
            }
            catch (Exception) { }
        }

        /// <summary>
        /// The function is called when a device settings message is received by the Client from the MQTT broker.
        /// The local settings information for the device are updated.
        /// If the device is selected in the Devices list, then also the various GUI controls for the settings are updated.
        /// </summary>
        /// <param name="dev">Device name</param>
        /// <param name="fw">Device firmware version</param>
        /// <param name="ip">Current Device IP address on it's local WiFi network. This may not be the same address as the MQTT broker recognizes.</param>
        /// <param name="scale">Scaling factor for the FFT data</param>
        /// <param name="window">FFT Windowing function</param>
        /// <param name="axis">IMU axis selected for acquisition</param>
        /// <param name="rate">Acquisition rage in kHz</param>
        /// <param name="samples">Down sampling factor from the direct FFT data to the 128 capture bins</param>
        /// <param name="interval">MQTT message interval rate in ms</param>
        /// <param name="pnts">Number of samples to capture per batch. This should be a power of 2, for effective FFT </param>
        private void updateSettings(string dev, string fw, string ip, double scale, int window, int axis, int rate, int samples, int interval, int pnts)
        {
            int idev = devices.FindIndex(x => x.name == dev);
            if (idev >= 0)
            {
                devices[idev].firmware = fw;
                devices[idev].ip = ip;
                devices[idev].scale = scale;
                devices[idev].window = window;
                devices[idev].axes = axis;
                devices[idev].rate = rate;
                devices[idev].samples = samples;
                devices[idev].interval = interval;
                devices[idev].pnts = pnts;
                devices[idev].valid = true;
            }

            if (dev == cbDevices.Text)
            {
                guiUpdate = true;
                lblFirmware.Text = "Firmware: " + fw;
                lblIP.Text = "IP Address: " + ip;
                cbAxis.SelectedIndex = axis;
                cbWindow.SelectedIndex = window;
                cbSamples.Text = pnts.ToString();
                cbRate.Text = rate.ToString() + " kHz";
                cbScale.Text = scale.ToString();
                cbInterval.Text = interval.ToString();
                cbDSamp.Text = samples.ToString();
                guiUpdate = false;
            }
        }

        private void updateFFT(string dev, double[] values, double fmax)
        {
            int idev = devices.FindIndex(x => x.name == dev);
            if (idev >= 0)
            {
                // shift data
                for (int k=0;k<99;k++)
                {
                    for (int i = 0; i < 128; i++)
                        devices[idev].FFTvalues[k, i] = devices[idev].FFTvalues[k+1, i];
                }
                for (int i = 0; i < values.Length; i++)
                    devices[idev].FFTvalues[99, i] = values[i];
            }
            try
            {
                if ((dev == cbDevices.Text) && ((ichart == 4) || (ichart == 6)))
                {
                    vibChart.Series["FFT"].Points.Clear();
                    for (int i = 0; i < values.Length; i++)
                    {
                        vibChart.Series["FFT"].Points.AddXY(i * fmax / 128, values[i]);
                    }
                    vibChart.ChartAreas["fftChart"].AxisX.Maximum = fmax;
                    if (fmax < 200)
                        vibChart.ChartAreas["fftChart"].AxisX.Interval = 25;
                    else if (fmax < 500)
                        vibChart.ChartAreas["fftChart"].AxisX.Interval = 50;
                    else
                        vibChart.ChartAreas["fftChart"].AxisX.Interval = 100;
                    vibChart.Update();
                }
                if ((dev == cbDevices.Text) && ((ichart == 5) || (ichart == 6)))
                {
                    vibChart.Series["Waterfall"].Points.Clear();
                    for (int k = 0; k < 100; k++)
                    {
                        for (int i = 0; i < 128; i++)
                        {
                            DataPoint p = new DataPoint(i * fmax / 128, k);
                            int red = (int)devices[idev].FFTvalues[k, i] * 10;
                            if (red > 255) red = 255; else if (red < 0) red = 0;
                            int green = (int)devices[idev].FFTvalues[k, i] * 2 - 20;
                            if (green > 255) green = 255; else if (green < 0) green = 0;
                            int blue = (int)devices[idev].FFTvalues[k, i] - 50;
                            if (blue > 255) blue = 255; else if (blue < 0) blue = 0;
                            p.Color = Color.FromArgb(red, green, blue);

                            vibChart.Series["Waterfall"].Points.Add(p);
                        }
                    }

                    vibChart.ChartAreas["wfChart"].AxisX.Maximum = fmax;
                    if (fmax<200)
                        vibChart.ChartAreas["wfChart"].AxisX.Interval = 25;
                    else if (fmax<500)
                        vibChart.ChartAreas["wfChart"].AxisX.Interval = 50;
                    else
                        vibChart.ChartAreas["wfChart"].AxisX.Interval = 100;


                    vibChart.Update();
                }
                if ((dev == cbDevices.Text) && (ichart == 7))
                {
                    vibChart.Series.Clear();
                    int ymax = 0;
                    for (int k = 0; k < 100; k++)
                    {
                        //string fftname = "FFT" + k.ToString();
                        Series fft = new Series();
                        fft.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.SplineArea;
                        for (int i = 0; i < 128; i++)
                        {
                            if ((int)devices[idev].FFTvalues[k, i] > ymax) ymax = (int)devices[idev].FFTvalues[k, i];
                            DataPoint p = new DataPoint(i * fmax / 128, (int)devices[idev].FFTvalues[k, i]);
                            int red = 25 + (int)devices[idev].FFTvalues[k, i] * 5;
                            if (red > 255) red = 255; else if (red < 0) red = 0;
                            int green = 25 + (int)devices[idev].FFTvalues[k, i] - 20;
                            if (green > 255) green = 255; else if (green < 0) green = 0;
                            int blue = 255 - (int)devices[idev].FFTvalues[k, i]*5;
                            if (blue > 255) blue = 255; else if (blue < 0) blue = 0;
                            p.Color = Color.FromArgb(red, green, blue);

                            fft.Points.Add(p);
                        }
                        vibChart.Series.Add(fft);
                    }
                    vibChart.ChartAreas["surfChart"].AxisY.Maximum = ymax;
                    vibChart.ChartAreas["surfChart"].AxisX.Maximum = fmax;
                    vibChart.ChartAreas["surfChart"].AxisX.Interval = 25;
                    vibChart.Update();
                }
            }
            catch (Exception) { }
        }


            private void updateBins(string dev, double[] values, double[] valuesOct)
        {
            int idev = devices.FindIndex(x => x.name == dev);
            if (idev >= 0)
            {
                for (int i = 0; i < values.Length; i++)
                {
                    devices[idev].FFTmode[i] = values[i];
                    devices[idev].FFToct[i] = valuesOct[i];
                }
            }
            try
            {
                if (dev == cbDevices.Text)
                {
                    if ((ichart == 1) || (ichart == 3))
                    {
                        vibChart.Series["modeBins"].Points.Clear();
                        double[] x = { 30, 60, 90, 120, 150, 180, 210 };
                        for (int i = 0; i < values.Length; i++)
                        {
                            DataPoint p = new DataPoint(x[i], valuesOct[i]);
                            p.AxisLabel = x[i].ToString();
                            p.MarkerStyle = MarkerStyle.Diamond;
                            p.MarkerSize = 10;
                            p.MarkerColor = Color.Red;
                            vibChart.Series["modeBins"].Points.Add(p);
                        }
                        
                        vibChart.Update();
                    }
                    if ((ichart == 2) || (ichart == 3))
                    {
                        vibChart.Series["octoBins"].Points.Clear();
                        double[] x = { 10, 20, 40, 80, 160, 320, 640 };
                        for (int i = 0; i < valuesOct.Length; i++)
                        {
                            DataPoint p = new DataPoint(x[i], valuesOct[i]);
                            p.AxisLabel = x[i].ToString();
                            p.MarkerStyle = MarkerStyle.Diamond;
                            p.MarkerSize = 10;
                            p.MarkerColor = Color.Black;
                            vibChart.Series["octoBins"].Points.Add(p);
                        }

                        

                        // https://stackoverflow.com/questions/44352528/chart-x-axis-numbering
                        //vibChart.ResetAutoValues();
                        vibChart.Update();
                    }
                }
            }
            catch (Exception) { }
        }

        private void Client_ConnectionClosed(object sender, EventArgs e)
        {
            //throw new NotImplementedException();
        }

        private void Client_MqttMsgSubscribed(object sender, MqttMsgSubscribedEventArgs e)
        {
            //throw new NotImplementedException();

            System.Diagnostics.Debug.WriteLine(e.ToString());
        }



        void client_MqttMsgPublishReceived(object sender, MqttMsgPublishEventArgs e)
        {
            //string ReceivedMessage = Encoding.UTF8.GetString(e.Message);

            //System.Diagnostics.Debug.WriteLine(e.Topic);
            if (topiclist.IndexOf(e.Topic) == -1)
            {
                topiclist.Add(e.Topic);
                //cbTopics.Items.Add(e.Topic);

                MethodInvoker cbtopicDelegate = delegate ()
                { cbTopics.Items.Add(e.Topic); if (topiclist.Count == 1) cbTopics.Text = e.Topic; };
                //This will be true if Current thread is not UI thread.
                if (this.InvokeRequired) this.Invoke(cbtopicDelegate); else cbtopicDelegate();
            }

            //System.Diagnostics.Debug.WriteLine(Encoding.UTF8.GetString(e.Message));
            string data = Encoding.UTF8.GetString(e.Message);
            try
            {

                JsonDocument rec = JsonDocument.Parse(data);
                JsonElement root = rec.RootElement;
                double rms = 0.0;
                string time = "";

                string dev = root.GetProperty("device").GetString();  // should always be included in the messages
                //string fmt = root.GetProperty("fmt").GetString();  // should always be included in the messages

                MethodInvoker cbdevDelegate = delegate ()
                { addDevice(e.Topic, dev); };
                //This will be true if Current thread is not UI thread.
                if (this.InvokeRequired) this.Invoke(cbdevDelegate); else cbdevDelegate();

                try
                {
                    rms = root.GetProperty("rms").GetDouble();
                    time = root.GetProperty("timestamp").GetString();
                    // Update time, rms in GUI
                    MethodInvoker updateDelegate = delegate ()
                    { updateDev(dev, rms, time); };
                    //This will be true if Current thread is not UI thread.
                    if (this.InvokeRequired) this.Invoke(updateDelegate); else updateDelegate();
                }
                catch (Exception) { }

                if (e.Topic.IndexOf("Console") >= 0)
                {
                    string contxt = (string)root.GetProperty("console").GetString();
                    char[] contxtb = contxt.ToCharArray();
                    for (int i = 0; i < contxtb.Length; i++)
                    {
                        if (contxtb[i] == '@')
                        {
                            contxtb[i - 1] = '\r';
                            contxtb[i] = '\n';
                        }
                    }
                    contxt += "\r\n";
                    MethodInvoker conDelegate = delegate ()
                    { rtxtConsole.Text += new string(contxtb); };
                    //This will be true if Current thread is not UI thread.
                    if (this.InvokeRequired) this.Invoke(conDelegate); else conDelegate();
                }

                if (e.Topic.IndexOf("Settings") >= 0)
                {
                    string fw = (string)root.GetProperty("firmware").GetString();
                    string ip = (string)root.GetProperty("ip").GetString();
                    double scale = (double)root.GetProperty("scale").GetDouble();
                    int window = (int)root.GetProperty("window").GetInt32();
                    int axes = (int)root.GetProperty("axis").GetInt32();
                    int rate = (int)root.GetProperty("rate").GetInt32();
                    int samples = (int)root.GetProperty("samples").GetInt32();
                    int interval = (int)root.GetProperty("interval").GetInt32();

                    int pnts = 1024;
                    try
                    {
                        pnts = (int)root.GetProperty("pnts").GetInt32();
                    }
                    catch (Exception) { }
                    MethodInvoker spDelegate = delegate ()
                    { updateSettings(dev, fw, ip, scale, window, axes, rate, samples, interval, pnts); };
                    //This will be true if Current thread is not UI thread.
                    if (this.InvokeRequired) this.Invoke(spDelegate); else spDelegate();
                }


                double[] values = new double[root.GetProperty("values").GetArrayLength()];
                JsonElement vals = root.GetProperty("values");
                int k = 0;
                foreach (JsonElement val in vals.EnumerateArray())
                {
                    values[k++] = val.GetDouble();
                }
                if (values.Length == 7)
                {
                    // now also get Octaves
                    double[] valuesOct = new double[root.GetProperty("valuesOct").GetArrayLength()];
                    JsonElement valsOct = root.GetProperty("valuesOct");
                    k = 0;
                    foreach (JsonElement val in vals.EnumerateArray())
                    {
                        valuesOct[k++] = val.GetDouble();
                    }
                    // then update bins

                    MethodInvoker fftDelegate = delegate ()
                    { updateBins(dev, values, valuesOct); };
                    //This will be true if Current thread is not UI thread.
                    if (this.InvokeRequired) this.Invoke(fftDelegate); else fftDelegate();
                }
                if (values.Length == 128)
                {
                    double fmax = (double)root.GetProperty("fmax").GetDouble();
                    MethodInvoker fftDelegate = delegate ()
                    { updateFFT(dev, values, fmax); };
                    //This will be true if Current thread is not UI thread.
                    if (this.InvokeRequired) this.Invoke(fftDelegate); else fftDelegate();
                }
            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.WriteLine(ex.ToString());
            }
        }

        private void butCmd_Click(object sender, EventArgs e)
        {
            if (cbTopics.Text.Length == 0)
            {
                MessageBox.Show("Must Select a Topic and Device.", "Vibration Manager", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }
            else
            {
                string topic = cbTopics.Text.Substring(0, cbTopics.Text.LastIndexOf('/'));
                client.Publish(topic + '/' + cbDevices.Text, Encoding.UTF8.GetBytes("{\"cmd\":\"" + txtCmd.Text + "\"}"));
            }

        }

        private void chChart_SelectedIndexChanged(object sender, EventArgs e)
        {
            ichart = cbChart.SelectedIndex;
            UpdateChartType();
        }
        private void UpdateChartType()
        {
            vibChart.ResetAutoValues();
            vibChart.Series.Clear();
            vibChart.ChartAreas.Clear();
            vibChart.Titles.Clear();
            switch (ichart)
            {
                case 0:   // RMS
                    vibChart.ChartAreas.Add("chart");
                    vibChart.Series.Add("RMS");
                    vibChart.Series["RMS"].ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
                    for (int i = 0; i < devices[idev].rms.Count; i++)
                    {
                        vibChart.Series["RMS"].Points.AddXY(devices[idev].rmstime[i], devices[idev].rms[i]);
                    }
                    vibChart.Series["RMS"].XValueType = ChartValueType.DateTime;
                    vibChart.Series["RMS"].Color = Color.Red;
                    vibChart.ChartAreas["chart"].AxisX.LabelStyle.Format = "hh:mm:ss";
                    //vibChart.ChartAreas["chart"].AxisX.Interval = 1;
                    vibChart.ChartAreas["chart"].AxisX.IntervalType = DateTimeIntervalType.Seconds;
                    //vibChart.ChartAreas["chart"].AxisX.IntervalOffset = 1;
                    vibChart.Titles.Add("RMS Trend");
                    break;
                case 1:   // 30 Hz Modes
                    vibChart.ChartAreas.Add("modeChart");
                    vibChart.Series.Add("modeBins");
                    vibChart.Series["modeBins"].ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
                    vibChart.Titles.Clear();
                    vibChart.Titles.Add("30 Hz Modes");
                    vibChart.ChartAreas["modeChart"].AxisX.Title = "Frequency [Hz]";
                    vibChart.ChartAreas["modeChart"].AxisX.Minimum = 30;
                    vibChart.ChartAreas["modeChart"].AxisX.Maximum = 210;
                    vibChart.ChartAreas["modeChart"].AxisX.Interval = 30;
                    break;
                case 2:  // 30 Hz Octaves
                    vibChart.ChartAreas.Add("octoChart");
                    vibChart.Series.Add("octoBins");
                    vibChart.Series["octoBins"].ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
                    vibChart.Titles.Add("30 Hz Octaves");
                    vibChart.ChartAreas["octoChart"].AxisX.Title = "Frequency [Hz]";

                    vibChart.ChartAreas["octoChart"].AxisX.Minimum = 0;  // optional
                    vibChart.ChartAreas["octoChart"].AxisX.MajorGrid.Enabled = false;
                    vibChart.ChartAreas["octoChart"].AxisX.MajorTickMark.Enabled = false;
                    vibChart.ChartAreas["octoChart"].AxisX.LabelStyle.Enabled = false;
                    //vibChart.Series["octoBins"].AxisLabel()
                    vibChart.ChartAreas["octoChart"].AxisX.Minimum = 0;
                    vibChart.ChartAreas["octoChart"].AxisX.Maximum = 650;
                    vibChart.ChartAreas["octoChart"].AxisX.Interval = 50;
                    break;
                case 3:  // 30 Hz Modes and Octaves
                    vibChart.ChartAreas.Add("modeChart");
                    vibChart.Series.Add("modeBins");
                    vibChart.Series["modeBins"].ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
                    ChartArea octochart = vibChart.ChartAreas.Add("octoChart");
                    vibChart.ChartAreas["modeChart"].AxisX.Minimum = 30;
                    vibChart.ChartAreas["modeChart"].AxisX.Maximum = 210;
                    vibChart.ChartAreas["modeChart"].AxisX.Interval = 30;

                    Series octoseries = new Series("octoBins");
                    octoseries.ChartArea = "octoChart";
                    vibChart.Series.Add(octoseries);
                    vibChart.Series["octoBins"].ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
                    vibChart.Titles.Add("30 Hz Modes & Octaves");
                    vibChart.ChartAreas["octoChart"].AxisX.Title = "Frequency [Hz]";
                    vibChart.ChartAreas["octoChart"].AxisX.Minimum = 0;  // optional
                    vibChart.ChartAreas["octoChart"].AxisX.MajorGrid.Enabled = false;
                    vibChart.ChartAreas["octoChart"].AxisX.MajorTickMark.Enabled = false;
                    vibChart.ChartAreas["octoChart"].AxisX.LabelStyle.Enabled = false;
                    //vibChart.Series["octoBins"].AxisLabel()
                    vibChart.ChartAreas["octoChart"].AxisX.Minimum = 0;
                    vibChart.ChartAreas["octoChart"].AxisX.Maximum = 650;
                    vibChart.ChartAreas["octoChart"].AxisX.Interval = 50;
                    break;
                case 4:  // FFT
                    vibChart.ChartAreas.Add("fftChart");
                    vibChart.Series.Add("FFT");
                    vibChart.Series["FFT"].ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
                    vibChart.Titles.Add("Frequency Spectrum");
                    vibChart.ChartAreas["fftChart"].AxisX.Title = "Frequency [Hz]";
                    vibChart.ChartAreas["fftChart"].AxisX.Minimum = 0;

                    break;
                case 5:  // Waterfall
                    vibChart.ChartAreas.Add("wfChart");
                    vibChart.Series.Add("Waterfall");
                    vibChart.Series["Waterfall"].ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Point;
                    //vibChart.Series["Waterfall"].YValueType = ChartValueType.DateTime;
                    vibChart.ChartAreas["wfChart"].AxisX.Title = "Frequency [Hz]";
                    vibChart.Titles.Add("Frequency Spectrum Waterfall");
                    vibChart.ChartAreas["wfChart"].AxisX.Minimum = 0;

                    break;
                case 6:  // FFT and Waterfall
                    vibChart.ChartAreas.Add("fftChart");
                    vibChart.Series.Add("FFT");
                    vibChart.Series["FFT"].ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
                    vibChart.Titles.Add("Frequency Spectrum & Waterfall");
                    vibChart.ChartAreas["fftChart"].AxisX.Minimum = 0;

                    ChartArea wfchart = vibChart.ChartAreas.Add("wfChart");
                    Series wfseries = new Series("Waterfall");
                    wfseries.ChartArea = "wfChart";
                    vibChart.Series.Add(wfseries);
                    vibChart.Series["Waterfall"].ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Point;
                    //vibChart.Series["Waterfall"].YValueType = ChartValueType.DateTime;
                    vibChart.ChartAreas["wfChart"].AxisX.Minimum = 0;

                    //vibChart.Titles.Add("Frequency Spectrum Waterfall");
                    break;
                case 7:
                    vibChart.ChartAreas.Add("surfChart");
                    vibChart.ChartAreas["surfChart"].AxisX.Minimum = 0;
                    vibChart.ChartAreas["surfChart"].AxisX.TextOrientation = TextOrientation.Rotated90;
                    vibChart.ChartAreas["surfChart"].AxisX.LabelStyle.Angle = 60;
                    vibChart.ChartAreas["surfChart"].Area3DStyle.Enable3D = true;
                    vibChart.ChartAreas["surfChart"].Area3DStyle.Rotation = 10;

                    break;
                default:
                    break;
            }
            
        }

        private void cbAxis_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (!guiUpdate)
            {
                client.Publish(devices[idev].topic.Substring(0, devices[idev].topic.LastIndexOf('/') + 1) + cbDevices.Text, 
                    Encoding.UTF8.GetBytes("{\"cmd\":\"sp88," + cbAxis.SelectedIndex.ToString() + "\"}"));
                devices[idev].axes = cbAxis.SelectedIndex;
            }
        }

        private void cbScale_SelectedIndexChanged(object sender, EventArgs e)
        {
            int scale = (int)(1000.0 * Convert.ToDouble(cbScale.Text));
            if (!guiUpdate)
            {
                client.Publish(devices[idev].topic.Substring(0, devices[idev].topic.LastIndexOf('/') + 1) + cbDevices.Text,
                      Encoding.UTF8.GetBytes("{\"cmd\":\"sp80," + scale.ToString() + "\"}"));
                devices[idev].scale = scale;
            }
        }

        private void cbDSamp_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (!guiUpdate)
            {
                client.Publish(devices[idev].topic.Substring(0, devices[idev].topic.LastIndexOf('/') + 1) + cbDevices.Text,
                    Encoding.UTF8.GetBytes("{\"cmd\":\"sp85," + (cbDSamp.SelectedIndex + 1).ToString() + "\"}"));
                devices[idev].samples = cbSamples.SelectedIndex + 1;
            }
        }

        private void cbRate_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (!guiUpdate)
            {
                client.Publish(devices[idev].topic.Substring(0, devices[idev].topic.LastIndexOf('/') + 1) + cbDevices.Text,
                    Encoding.UTF8.GetBytes("{\"cmd\":\"sp87," + (cbRate.SelectedIndex + 1).ToString() + "\"}"));
                devices[idev].rate = cbRate.SelectedIndex + 1;
            }
        }

        private void cbSamples_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (!guiUpdate)
            {
                client.Publish(devices[idev].topic.Substring(0, devices[idev].topic.LastIndexOf('/') + 1) + cbDevices.Text,
                    Encoding.UTF8.GetBytes("{\"cmd\":\"sp84," + (1024 * (cbSamples.SelectedIndex + 1)).ToString() + "\"}"));
                devices[idev].pnts = 1024 * (cbSamples.SelectedIndex + 1);
            }
        }

        private void cbFmt_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (!guiUpdate)
            {
                client.Publish(devices[idev].topic.Substring(0, devices[idev].topic.LastIndexOf('/') + 1) + cbDevices.Text,
                    Encoding.UTF8.GetBytes("{\"cmd\":\"wf53," + (cbFmt.SelectedIndex + 2).ToString() + "\"}"));
                //devices[idev].
            }
        }

        private void cbWindow_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (!guiUpdate)
            {
                client.Publish(devices[idev].topic.Substring(0, devices[idev].topic.LastIndexOf('/') + 1) + cbDevices.Text,
                    Encoding.UTF8.GetBytes("{\"cmd\":\"sp83," + cbWindow.SelectedIndex.ToString() + "\"}"));
                devices[idev].window = cbWindow.SelectedIndex;
            }
        }

        private void cbInterval_SelectedIndexChanged(object sender, EventArgs e)
        {
            int iv = (int)(1000.0 * Convert.ToDouble(cbInterval.Text));
            if (!guiUpdate)
            {
                client.Publish(devices[idev].topic.Substring(0, devices[idev].topic.LastIndexOf('/') + 1) + cbDevices.Text,
                    Encoding.UTF8.GetBytes("{\"cmd\":\"sp95," + iv.ToString() + "\"}"));
                devices[idev].interval = iv;
            }
        }

        private void VibManager_FormClosed(object sender, FormClosedEventArgs e)
        {
            if (client.IsConnected)
            {
                client.Unsubscribe(new string[] { "#" });
                client.Disconnect();
                System.Threading.Thread.Sleep(1000);
            }
            key.Close();

        }

        private void cbDevices_SelectedIndexChanged(object sender, EventArgs e)
        {
            string dev = cbDevices.Text;

            idev = devices.FindIndex(x => x.name == dev);
            if (idev >= 0)
            {
                guiUpdate = true;
                lblFirmware.Text = "Firmware: " + devices[idev].firmware;
                lblIP.Text = "IP Address: " + devices[idev].ip;
                cbAxis.SelectedIndex = devices[idev].axes;
                cbSamples.Text = devices[idev].samples.ToString();
                cbRate.Text = devices[idev].rate.ToString() + " kHz";
                cbScale.Text = devices[idev].scale.ToString();
                cbInterval.Text = devices[idev].interval.ToString();
                cbDSamp.Text = devices[idev].pnts.ToString();
                guiUpdate = false;
                rtxtConsole.Text = "";
                cbTopics.Text = devices[idev].topic;
                if (cbChart.Text == "")
                    cbChart.Text = cbChart.Items[0].ToString();
                UpdateChartType();
            }
            else
            {
                guiUpdate = true;
                lblFirmware.Text = "Firmware: ";
                lblIP.Text = "IP Address: ";
                cbAxis.SelectedIndex = 0;
                cbSamples.Text = "";
                cbRate.Text = "";
                cbScale.Text = "";
                cbInterval.Text = "";
                cbDSamp.Text = "";
                rtxtConsole.Text = "";
                guiUpdate = false;
            }
        }

        private void cbTopics_SelectedIndexChanged(object sender, EventArgs e)
        {

        }


        private void butReset_Click(object sender, EventArgs e)
        {
            if (!guiUpdate)
                client.Publish(devices[idev].topic.Substring(0, devices[idev].topic.LastIndexOf('/') + 1) + cbDevices.Text, Encoding.UTF8.GetBytes("{\"cmd\":\"rs0\"}"));
        }

        private void butUpdate_Click(object sender, EventArgs e)
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
                    System.Diagnostics.Process.Start("CMD.exe", "/C GPM_UpdateLite.exe " + devices[idev].ip + " " + openFileDialog.FileName);
                    //Update(devices[idev].ip, openFileDialog.FileName);
                }
            }
        }

        private void vibChart_PostPaint(object sender, ChartPaintEventArgs e)
        {
            if ((ichart == 2) || (ichart == 3))
            {
                Graphics g = e.ChartGraphics.Graphics;
                g.TextRenderingHint = System.Drawing.Text.TextRenderingHint.AntiAliasGridFit;

                ChartArea ca = vibChart.ChartAreas["octoChart"];

                Font font = ca.AxisX.LabelStyle.Font;
                Color col = ca.AxisX.MajorGrid.LineColor;
                int padding = 10; // pad the labels from the axis

                int y0 = (int)ca.AxisY.ValueToPixelPosition(ca.AxisY.Minimum);
                int y1 = (int)ca.AxisY.ValueToPixelPosition(ca.AxisY.Maximum);
                double[] stops = { 10, 20, 40, 80, 160, 320, 640 };
                foreach (int sx in stops)
                {
                    int x = (int)ca.AxisX.ValueToPixelPosition(sx);

                    using (Pen pen = new Pen(col))
                        g.DrawLine(pen, x, y0, x, y1);

                    string s = sx + "";
                    if (ca.AxisX.LabelStyle.Format != "")
                        s = string.Format(ca.AxisX.LabelStyle.Format, s);

                    SizeF sz = g.MeasureString(s, font, 999);
                    g.DrawString(s, font, Brushes.Black, (int)(x - sz.Width / 2), y0 + padding);
                }
            }
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

       
        private void Update(string ip, string fw)
        {
            try
            {
                using (Process p = new Process())
                {
                    // set start info
                    p.StartInfo = new ProcessStartInfo("GPM_UpdateLite.exe")
                    { 
                        UseShellExecute = false,
                        CreateNoWindow = true,
                        Arguments = ip + " " + fw,
                        RedirectStandardInput = true,
                        RedirectStandardOutput = true,
                        //WorkingDirectory = @"C:\Projects\GPS\UBlox\GPM_Monitor\Debug"
                    };
                    // event handlers for output & error
                    p.OutputDataReceived += p_OutputDataReceived;
                    p.ErrorDataReceived += p_ErrorDataReceived;

                    // start process
                    p.Start();
                    // send command to its input
                    //p.StandardInput.Write("GPM_UpdateLite.exe " + ip+" "+fw);
                    //wait
                    p.WaitForExit();
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex);
            }
        }

        static void p_ErrorDataReceived(object sender, DataReceivedEventArgs e)
        {
            Process p = sender as Process;
            if (p == null)
                return;
            Console.WriteLine(e.Data);
        }

        static void p_OutputDataReceived(object sender, DataReceivedEventArgs e)
        {
            Process p = sender as Process;
            if (p == null)
                return;
            Console.WriteLine(e.Data);
        }
        
    }

    /// <summary>
    /// The <see cref="GPM_MQTTClient2"/> namespace defines a Client Application to an MQTT broker for managing vibration 
    /// data acquisition devices.
    /// </summary>

    [System.Runtime.CompilerServices.CompilerGenerated]
    class NamespaceDoc
    {
    }
}

