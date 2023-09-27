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
using System.Windows.Forms.DataVisualization.Charting;

using uPLibrary.Networking.M2Mqtt;
using uPLibrary.Networking.M2Mqtt.Messages;

namespace GPM_MQTTClient2
{
    public partial class VibManager : Form
    {
        public static String access_token = "no valid token received yet.";
        public static MqttClient client;
        public static string clientId;
        internal bool guiUpdate = false;
        internal int idev = 0;

        List<string> topiclist = new List<string>();
        List<string> devicelist = new List<string>();

        internal class DeviceData
        {
            internal string name;
            internal string topic;
            internal bool valid;
            internal string firmware;
            internal string timestamp;
            internal double[] rms;
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
        
        public VibManager()
        {
            InitializeComponent();

            client = new MqttClient(txtHost.Text, Convert.ToInt32(txtPort.Text), false, MqttSslProtocols.SSLv3, null, null);
            // register a callback-function (we have to implement, see below) which is called by the library when a message was received
            client.MqttMsgPublishReceived += client_MqttMsgPublishReceived;
            client.ConnectionClosed += Client_ConnectionClosed;
            client.MqttMsgSubscribed += Client_MqttMsgSubscribed;

            vibChart.Series["Series1"].ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;

        }

        private void butConnect_Click(object sender, EventArgs e)
        {
            if (client.IsConnected)
            {
                client.Disconnect();
                butConnect.Text = "Connect";

                topiclist.Clear();
                devicelist.Clear();
                cbTopics.Items.Clear();
                cbDevices.Items.Clear();
                txtRMS.Text = "";
            }
            else
            {
                // use a unique id as client id, each time we start the application
                clientId = Guid.NewGuid().ToString();

                client.Connect(clientId, txtUser.Text, txtPassword.Text);

                client.Subscribe(new string[] { "#" }, new byte[] { MqttMsgBase.QOS_LEVEL_AT_MOST_ONCE });

                butConnect.Text = "Disconnect";
            }
        }

        private void addDevice(string topic, string dev)
        {

            // Add the current device to the global Devices list
            if (devicelist.IndexOf(dev) == -1)
            {
                DeviceData dd = new DeviceData();
                dd.name = dev;
                dd.topic = topic;
                dd.valid = false;
                dd.rms = new double[0];
                dd.FFTmode = new double[7];
                dd.FFToct = new double[7];
                dd.FFTvalues = new double[100,128];
                devices.Add(dd);
                devicelist.Add(dev);
                cbDevices.Items.Add(dev);
                if (devicelist.Count == 1) cbDevices.Text = dev;

                // request parameters
                int it = topic.LastIndexOf('/');
                if ((it>0) && (topic.Length>0))
                {
                    string top = topic.Substring(0, it);
                    client.Publish(top + '/' + dev, Encoding.UTF8.GetBytes("{\"cmd\":\"sp*\"}"));
                }
            }
        }

        private void updateDev(string dev, double rms, string time)
        {
            int idev = devices.FindIndex(x => x.name == dev);
            if (idev>=0)
            {
                if (devices[idev].rms.Length < 1000)
                    devices[idev].rms.Append(rms);
                else
                {
                    for (int i = 0; i < 999; i++)
                        devices[idev].rms[i] = devices[idev].rms[i + 1];
                    devices[idev].rms[999] = rms;
                }
                devices[idev].timestamp = time;
            }

            if (cbDevices.Text == dev)
            {
                txtRMS.Text = rms.ToString();
                lblTime.Text = "Date and Time: " + time;
                if (cbChart.SelectedIndex == 0)
                {
                    vibChart.Series["RMS"].Points.AddY(rms);
                    if (vibChart.Series["RMS"].Points.Count > 10)
                        vibChart.Series["RMS"].Points.RemoveAt(0);
                }
            }
        }
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
                cbSamples.Text = samples.ToString();
                cbRate.Text = rate.ToString() + " kHz";
                cbScale.Text = scale.ToString();
                cbInterval.Text = interval.ToString();
                cbDSamp.Text = pnts.ToString();
                guiUpdate = false;
            }
        }

        private void updateFFT(string dev, double[] values, double fmax)
        {
            int idev = devices.FindIndex(x => x.name == dev);
            if (idev >= 0) 
            {
                for (int i = 0; i < values.Length; i++)
                    devices[idev].FFTvalues[0, i] = values[i];
            }
            if ((dev == cbDevices.Text) && (cbChart.SelectedIndex == 4))
            {
                vibChart.Series["FFT"].Points.Clear();
                for (int i = 0; i < values.Length; i++)
                {
                    vibChart.Series["FFT"].Points.AddXY(i * fmax / 128, values[i]);
                }
                vibChart.ChartAreas[0].AxisX.Minimum = 0;
                vibChart.ChartAreas[0].AxisX.Maximum = fmax;
                vibChart.ChartAreas[0].AxisX.Interval = 25;
            }
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
            if (dev == cbDevices.Text)
            {
                if (cbChart.SelectedIndex == 1)
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
                    vibChart.ChartAreas[0].AxisX.Minimum = 30;
                    vibChart.ChartAreas[0].AxisX.Maximum = 210;
                    vibChart.ChartAreas[0].AxisX.Interval = 30;
                    vibChart.Update();
                }
                if (cbChart.SelectedIndex == 2)
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
                    vibChart.ChartAreas[0].AxisX.Minimum = 0;
                    vibChart.ChartAreas[0].AxisX.Maximum = 650;
                    vibChart.ChartAreas[0].AxisX.Interval = 50;
                    //vibChart.ResetAutoValues();
                    vibChart.Update();
                }
            }
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
                    int pnts = 0;// (int)root.GetProperty("pnts").GetInt32();

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
                if (values.Length==7)
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
                if (values.Length==128)
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
            vibChart.ResetAutoValues();
            vibChart.Series.Clear();
            switch (cbChart.SelectedIndex)
            {
                case 0:   // RMS
                    vibChart.Series.Add("RMS");
                    vibChart.Series["RMS"].ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
                    //chart title  
                    vibChart.Titles.Clear();
                    vibChart.Titles.Add("RMS Trend");
                    break;
                case 1:   // 30 Hz Modes
                    vibChart.Series.Add("modeBins");
                    vibChart.Series["modeBins"].ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
                    vibChart.Titles.Clear();
                    vibChart.Titles.Add("30 Hz Modes");                    
                    break;
                case 2:  // 30 Hz Octaves
                    vibChart.Series.Add("octoBins");
                    vibChart.Series["octoBins"].ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
                    vibChart.Titles.Clear();
                    vibChart.Titles.Add("30 Hz Octaves");
                    break;
                case 3:  // 30 Hz Modes and Octaves
                    vibChart.Titles.Clear();
                    vibChart.Series["octoBins"].ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
                    vibChart.Titles.Add("30 Hz Modes & Octaves");
                    break;
                case 4:  // FFT
                    vibChart.Series.Add("FFT");
                    vibChart.Series["FFT"].ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
                    vibChart.Titles.Clear();
                    vibChart.Titles.Add("Frequency Spectrum");
                    
                    break;
                case 5:  // Waterfall
                    break;
                case 6:  // FFT and Waterfall
                    break;
                default:
                    break;
            }
        }

        private void cbAxis_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (!guiUpdate)
                client.Publish(devices[idev].topic.Substring(0,devices[idev].topic.LastIndexOf('/')+1) + cbDevices.Text, Encoding.UTF8.GetBytes("{\"cmd\":\"sp88," + cbAxis.SelectedIndex.ToString() + "\"}"));
        }

        private void cbScale_SelectedIndexChanged(object sender, EventArgs e)
        {
            int scale = (int)(1000.0 * Convert.ToDouble(cbScale.Text));
            if (!guiUpdate)
                client.Publish(devices[idev].topic.Substring(0, devices[idev].topic.LastIndexOf('/') + 1) + cbDevices.Text, Encoding.UTF8.GetBytes("{\"cmd\":\"sp80," + scale.ToString() + "\"}"));
        }

        private void cbDSamp_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (!guiUpdate)
                client.Publish(devices[idev].topic.Substring(0, devices[idev].topic.LastIndexOf('/') + 1) + cbDevices.Text, Encoding.UTF8.GetBytes("{\"cmd\":\"sp85," + cbDSamp.SelectedIndex.ToString() + "\"}"));
        }

        private void cbRate_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (!guiUpdate)
                client.Publish(devices[idev].topic.Substring(0, devices[idev].topic.LastIndexOf('/') + 1) + cbDevices.Text, Encoding.UTF8.GetBytes("{\"cmd\":\"sp88," + (cbRate.SelectedIndex+1).ToString() + "\"}"));
        }

        private void cbSamples_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (!guiUpdate)
                client.Publish(devices[idev].topic.Substring(0, devices[idev].topic.LastIndexOf('/') + 1) + cbDevices.Text, Encoding.UTF8.GetBytes("{\"cmd\":\"sp84," + (1024*(cbSamples.SelectedIndex+1)).ToString() + "\"}"));
        }

        private void cbFmt_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (!guiUpdate)
                client.Publish(devices[idev].topic.Substring(0, devices[idev].topic.LastIndexOf('/') + 1) + cbDevices.Text, Encoding.UTF8.GetBytes("{\"cmd\":\"wf53," + (cbFmt.SelectedIndex+2).ToString() + "\"}"));
        }

        private void cbWindow_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (!guiUpdate)
                client.Publish(devices[idev].topic.Substring(0, devices[idev].topic.LastIndexOf('/') + 1) + cbDevices.Text, Encoding.UTF8.GetBytes("{\"cmd\":\"sp83," + cbWindow.SelectedIndex.ToString() + "\"}"));
        }

        private void cbInterval_SelectedIndexChanged(object sender, EventArgs e)
        {
            int iv = (int)(1000.0 * Convert.ToDouble(cbInterval.Text));
            if (!guiUpdate)
                client.Publish(devices[idev].topic.Substring(0, devices[idev].topic.LastIndexOf('/') + 1) + cbDevices.Text, Encoding.UTF8.GetBytes("{\"cmd\":\"sp95," + iv.ToString() + "\"}"));
        }

        private void VibManager_FormClosed(object sender, FormClosedEventArgs e)
        {
            if (client.IsConnected)
            {
                client.Unsubscribe(new string[] { "#" });
                client.Disconnect();
            }
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
                cbAxis.SelectedIndex = devices[idev].axes   ;
                cbSamples.Text = devices[idev].samples.ToString();
                cbRate.Text = devices[idev].rate.ToString() + " kHz";
                cbScale.Text = devices[idev].scale.ToString();
                cbInterval.Text = devices[idev].interval.ToString();
                cbDSamp.Text = devices[idev].pnts.ToString();
                guiUpdate = false;
                string currentchart = cbChart.Text;
                cbChart.Text = currentchart;
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
                    System.Diagnostics.Process.Start("CMD.exe", "GPM_UpdateLite.exe " + devices[idev].ip + " " + openFileDialog.FileName);
                }
            }
        }
    }
}
