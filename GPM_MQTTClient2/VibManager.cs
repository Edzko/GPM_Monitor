﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;
using System.Windows.Forms;

using uPLibrary.Networking.M2Mqtt;
using uPLibrary.Networking.M2Mqtt.Messages;

namespace GPM_MQTTClient2
{
    public partial class VibManager : Form
    {
        public static String access_token = "no valid token received yet.";
        public static MqttClient client;
        public static string clientId;
        int selectedChart = -1;

        List<string> topiclist = new List<string>();
        List<string> devicelist = new List<string>();

        internal class DeviceData
        {
            internal string name;
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
                dd.valid = false;
                dd.rms = new double[0];
                dd.FFTmode = new double[7];
                dd.FFToct = new double[7];
                dd.FFTvalues = new double[100,128];
                devices.Add(dd);
                devicelist.Add(dev);
                cbDevices.Items.Add(dev);

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

            if ((cbDevices.Text == dev) && (cbChart.SelectedIndex == 0))
            {
                txtRMS.Text = rms.ToString();
                lblTime.Text = "Date and Time: " + time;
                vibChart.Series["Series1"].Points.AddY(rms);
                if (vibChart.Series["Series1"].Points.Count > 10)
                    vibChart.Series["Series1"].Points.RemoveAt(0);
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
                lblFirmware.Text = "Firmware: " + fw;
                lblIP.Text = "IP Address: " + ip;
                cbAxis.SelectedIndex = axis;
                cbWindow.SelectedIndex = window;
                cbSamples.Text = samples.ToString();
                cbRate.Text = rate.ToString() + " kHz";
                cbScale.Text = scale.ToString();
                cbInterval.Text = interval.ToString();
                cbDSamp.Text = pnts.ToString();
            }
        }

        private void updateFFT(string dev, double[] values)
        {
            int idev = devices.FindIndex(x => x.name == dev);
            if (idev >= 0) 
            {
                for (int i = 0; i < values.Length; i++)
                    devices[idev].FFTvalues[0, i] = values[i];
            }
            if ((dev == cbTopics.Text) && (cbChart.SelectedIndex == 4))
            {
                for (int i = 0; i < values.Length; i++)
                    vibChart.Series["Series1"].Points[i].YValues[0] = values[i];
            }
        }
        private void updateBins(string dev, double[] values)
        {
            int idev = devices.FindIndex(x => x.name == dev);
            if (idev >= 0) 
            {
                for (int i = 0; i < values.Length; i++)
                    devices[idev].FFTmode[i] = values[i];
            }
            if ((dev == cbDevices.Text) && (cbChart.SelectedIndex == 1))
            {
                vibChart.Series["Series1"].Points.Clear();
                double[] x = { 30, 60, 90, 120, 150, 180, 210 };
                for (int i = 0; i < values.Length; i++)
                    vibChart.Series["Series1"].Points.AddXY(x[i], values[i]);
                vibChart.Update();
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
                { cbTopics.Items.Add(e.Topic); };

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

                    // then update bins

                    MethodInvoker fftDelegate = delegate ()
                    { updateBins(dev, values); };
                    //This will be true if Current thread is not UI thread.
                    if (this.InvokeRequired) this.Invoke(fftDelegate); else fftDelegate();
                }
                if (values.Length==128)
                {
                    MethodInvoker fftDelegate = delegate ()
                    { updateFFT(dev, values); };
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
            switch (cbChart.SelectedIndex)
            {
                case 0:
                    System.Diagnostics.Debug.WriteLine("RMS Trend");
                    vibChart.Series["Series1"].Points.Clear();
                    vibChart.Series["Series1"].ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
                    //chart title  
                    vibChart.Titles.Clear();
                    vibChart.Titles.Add("RMS Trend");
                    selectedChart = 0;
                    break;
                case 1:
                    System.Diagnostics.Debug.WriteLine("30 Hz Modes");
                    vibChart.Series["Series1"].Points.Dispose();
                    vibChart.Titles.Clear();
                    vibChart.Titles.Add("30 Hz Modes");
                    selectedChart = 0;
                    for (int i = 0; i < 7; i++)
                        vibChart.Series["Series1"].Points.AddXY(i, 0);
                    break;
                case 2:
                    System.Diagnostics.Debug.WriteLine("30 Hz octaves");
                    break;
                case 3:
                    System.Diagnostics.Debug.WriteLine("Mode & Octaves");
                    break;
                case 4:
                    System.Diagnostics.Debug.WriteLine("FFT");
                    vibChart.Series["Series1"].Points.Dispose();
                    vibChart.Titles.Clear();
                    vibChart.Titles.Add("Frequency Spectrum");
                    selectedChart = 0;
                    for (int i = 0; i < 128; i++)
                        vibChart.Series["Series1"].Points.AddXY(i,0);
                    break;
                case 5:
                    System.Diagnostics.Debug.WriteLine("Waterfall");
                    break;
                case 6:
                    System.Diagnostics.Debug.WriteLine("FFT & Waterfall");
                    break;
                default:
                    break;
            }
        }

        private void cbAxis_SelectedIndexChanged(object sender, EventArgs e)
        {
            client.Publish("Test/DataAnalysis/Vibration/" + cbDevices.Text, Encoding.UTF8.GetBytes("{\"cmd\":\"sp88," + cbAxis.SelectedIndex.ToString() + "\"}"));
        }

        private void cbScale_SelectedIndexChanged(object sender, EventArgs e)
        {
            int scale = (int)(1000.0 * Convert.ToDouble(cbScale.Text));
            client.Publish("Test/DataAnalysis/Vibration/" + cbDevices.Text, Encoding.UTF8.GetBytes("{\"cmd\":\"sp80," + scale.ToString() + "\"}"));
        }

        private void cbDSamp_SelectedIndexChanged(object sender, EventArgs e)
        {
            client.Publish("Test/DataAnalysis/Vibration/" + cbDevices.Text, Encoding.UTF8.GetBytes("{\"cmd\":\"sp85," + cbDSamp.SelectedIndex.ToString() + "\"}"));
        }

        private void cbRate_SelectedIndexChanged(object sender, EventArgs e)
        {
            client.Publish("Test/DataAnalysis/Vibration/" + cbDevices.Text, Encoding.UTF8.GetBytes("{\"cmd\":\"sp88," + (cbRate.SelectedIndex+1).ToString() + "\"}"));
        }

        private void cbSamples_SelectedIndexChanged(object sender, EventArgs e)
        {
            client.Publish("Test/DataAnalysis/Vibration/" + cbDevices.Text, Encoding.UTF8.GetBytes("{\"cmd\":\"sp84," + (1024*(cbSamples.SelectedIndex+1)).ToString() + "\"}"));
        }

        private void cbFmt_SelectedIndexChanged(object sender, EventArgs e)
        {
            client.Publish("Test/DataAnalysis/Vibration/" + cbDevices.Text, Encoding.UTF8.GetBytes("{\"cmd\":\"wf53," + (cbFmt.SelectedIndex+2).ToString() + "\"}"));
        }

        private void cbWindow_SelectedIndexChanged(object sender, EventArgs e)
        {
            client.Publish("Test/DataAnalysis/Vibration/" + cbDevices.Text, Encoding.UTF8.GetBytes("{\"cmd\":\"sp83," + cbWindow.SelectedIndex.ToString() + "\"}"));
        }

        private void cbInterval_SelectedIndexChanged(object sender, EventArgs e)
        {
            int iv = (int)(1000.0 * Convert.ToDouble(cbInterval.Text));
            client.Publish("Test/DataAnalysis/Vibration/" + cbDevices.Text, Encoding.UTF8.GetBytes("{\"cmd\":\"sp95," + iv.ToString() + "\"}"));
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

            int idev = devices.FindIndex(x => x.name == dev);
            if (idev >= 0)
            {
                lblFirmware.Text = "Firmware: " + devices[idev].firmware;
                lblIP.Text = "IP Address: " + devices[idev].ip;
                cbAxis.SelectedIndex = devices[idev].axes   ;
                cbSamples.Text = devices[idev].samples.ToString();
                cbRate.Text = devices[idev].rate.ToString() + " kHz";
                cbScale.Text = devices[idev].scale.ToString();
                cbInterval.Text = devices[idev].interval.ToString();
                cbDSamp.Text = devices[idev].pnts.ToString();

                vibChart.Series["Series1"].Points.Clear();
                foreach (double rms in devices[idev].rms)
                    vibChart.Series["Series1"].Points.AddY(rms);
            }
            else
            {
                lblFirmware.Text = "Firmware: ";
                lblIP.Text = "IP Address: ";
                cbAxis.SelectedIndex = 0;
                cbSamples.Text = "";
                cbRate.Text = "";
                cbScale.Text = "";
                cbInterval.Text = "";
                cbDSamp.Text = "";
                vibChart.Series["Series1"].Points.Clear();
            }
        }
    }
}
