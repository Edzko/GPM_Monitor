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

        List<string> topics = new List<string>();
        List<string> devices = new List<string>();

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

                topics.Clear();
                devices.Clear();
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

        private void updateDev(string dev, double rms, string time)
        {
            if (cbDevices.Text == dev)
            {
                txtRMS.Text = rms.ToString();
                lblTime.Text = "Date and Time: " + time;
            }
        }
        private void updateSettings(string fw, string ip, double scale, int window, int axis, int rate, int samples, int interval, int pnts)
        {
            lblFirmware.Text = "Firmware: " + fw;
            lblIP.Text = "IP Address: " + ip;
            cbAxis.SelectedIndex = axis;
            cbSamples.Text = samples.ToString();
            cbRate.Text = rate.ToString() + " kHz";
            cbScale.Text = scale.ToString();
            cbInterval.Text = interval.ToString();
            cbDSamp.Text = pnts.ToString();
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
            if (topics.IndexOf(e.Topic) == -1)
            {
                topics.Add(e.Topic);
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
                try
                {
                    rms = root.GetProperty("rms").GetDouble();
                    if (selectedChart == 0)
                    {
                        MethodInvoker cbchartDelegate = delegate ()
                        { vibChart.Series["Series1"].Points.AddY(rms); };

                        //This will be true if Current thread is not UI thread.
                        if (this.InvokeRequired) this.Invoke(cbchartDelegate); else cbchartDelegate();
                        
                    }
                    time = root.GetProperty("timestamp").GetString();
                }
                catch (Exception) { }
                var dev = root.GetProperty("device").GetString();

                if (devices.IndexOf(dev) == -1)
                {
                    devices.Add(dev);
                    MethodInvoker cbdevDelegate = delegate ()
                    { cbDevices.Items.Add(dev); };

                    //This will be true if Current thread is not UI thread.
                    if (this.InvokeRequired) this.Invoke(cbdevDelegate); else cbdevDelegate();
                }

                MethodInvoker updateDelegate = delegate ()
                { updateDev(dev, rms, time); };

                //This will be true if Current thread is not UI thread.
                if (this.InvokeRequired) this.Invoke(updateDelegate); else updateDelegate();

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
                    { updateSettings(fw,ip,scale,window,axes,rate,samples,interval,pnts); };

                    //This will be true if Current thread is not UI thread.
                    if (this.InvokeRequired) this.Invoke(spDelegate); else spDelegate();
                }
            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.WriteLine(ex.ToString());
            }
        }

        private void butCmd_Click(object sender, EventArgs e)
        {
            client.Publish("Test/DataAnalysis/Vibration/" + cbDevices.Text, Encoding.UTF8.GetBytes("{\"cmd\":\"" + txtCmd.Text + "\"}"));
        }

        private void chChart_SelectedIndexChanged(object sender, EventArgs e)
        {
            switch (cbChart.SelectedIndex)
            {
                case 0:
                    System.Diagnostics.Debug.WriteLine("RMS Trend");
                    vibChart.Series["Series1"].Points.Dispose();
                    vibChart.Series["Series1"].ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
                    //chart title  
                    vibChart.Titles.Add("RMS Trend");
                    selectedChart = 0;
                    break;
                case 1:
                    System.Diagnostics.Debug.WriteLine("30 Hz Modes");
                    break;
                case 2:
                    System.Diagnostics.Debug.WriteLine("30 Hz octaves");
                    break;
                case 3:
                    System.Diagnostics.Debug.WriteLine("Mode & Octaves");
                    break;
                case 4:
                    System.Diagnostics.Debug.WriteLine("FFT");
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
    }
}
