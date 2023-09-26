
namespace GPM_MQTTClient2
{
    partial class VibManager
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea9 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Series series9 = new System.Windows.Forms.DataVisualization.Charting.Series();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.cbDevices = new System.Windows.Forms.ComboBox();
            this.cbTopics = new System.Windows.Forms.ComboBox();
            this.butConnect = new System.Windows.Forms.Button();
            this.txtPort = new System.Windows.Forms.TextBox();
            this.txtPassword = new System.Windows.Forms.TextBox();
            this.txtUser = new System.Windows.Forms.TextBox();
            this.txtHost = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.label11 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.label12 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.cbChart = new System.Windows.Forms.ComboBox();
            this.comboBox6 = new System.Windows.Forms.ComboBox();
            this.cbSamples = new System.Windows.Forms.ComboBox();
            this.cbRate = new System.Windows.Forms.ComboBox();
            this.cbInterval = new System.Windows.Forms.ComboBox();
            this.cbWindow = new System.Windows.Forms.ComboBox();
            this.cbFmt = new System.Windows.Forms.ComboBox();
            this.button1 = new System.Windows.Forms.Button();
            this.lblFirmware = new System.Windows.Forms.Label();
            this.txtRMS = new System.Windows.Forms.TextBox();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.butCmd = new System.Windows.Forms.Button();
            this.txtCmd = new System.Windows.Forms.TextBox();
            this.rtxtConsole = new System.Windows.Forms.RichTextBox();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.vibChart = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.toolTip1 = new System.Windows.Forms.ToolTip(this.components);
            this.cbAxis = new System.Windows.Forms.ComboBox();
            this.cbScale = new System.Windows.Forms.ComboBox();
            this.cbDSamp = new System.Windows.Forms.ComboBox();
            this.label13 = new System.Windows.Forms.Label();
            this.label14 = new System.Windows.Forms.Label();
            this.label15 = new System.Windows.Forms.Label();
            this.lblIP = new System.Windows.Forms.Label();
            this.lblTime = new System.Windows.Forms.Label();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.groupBox4.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.vibChart)).BeginInit();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.cbDevices);
            this.groupBox1.Controls.Add(this.cbTopics);
            this.groupBox1.Controls.Add(this.butConnect);
            this.groupBox1.Controls.Add(this.txtPort);
            this.groupBox1.Controls.Add(this.txtPassword);
            this.groupBox1.Controls.Add(this.txtUser);
            this.groupBox1.Controls.Add(this.txtHost);
            this.groupBox1.Controls.Add(this.label4);
            this.groupBox1.Controls.Add(this.label3);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Location = new System.Drawing.Point(14, 16);
            this.groupBox1.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Padding = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.groupBox1.Size = new System.Drawing.Size(461, 271);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "MQTT Broker:";
            // 
            // cbDevices
            // 
            this.cbDevices.FormattingEnabled = true;
            this.cbDevices.Location = new System.Drawing.Point(76, 156);
            this.cbDevices.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.cbDevices.Name = "cbDevices";
            this.cbDevices.Size = new System.Drawing.Size(213, 25);
            this.cbDevices.TabIndex = 3;
            // 
            // cbTopics
            // 
            this.cbTopics.FormattingEnabled = true;
            this.cbTopics.Location = new System.Drawing.Point(76, 120);
            this.cbTopics.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.cbTopics.Name = "cbTopics";
            this.cbTopics.Size = new System.Drawing.Size(352, 25);
            this.cbTopics.TabIndex = 3;
            // 
            // butConnect
            // 
            this.butConnect.Location = new System.Drawing.Point(296, 156);
            this.butConnect.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.butConnect.Name = "butConnect";
            this.butConnect.Size = new System.Drawing.Size(132, 27);
            this.butConnect.TabIndex = 2;
            this.butConnect.Text = "Connect";
            this.butConnect.UseVisualStyleBackColor = true;
            this.butConnect.Click += new System.EventHandler(this.butConnect_Click);
            // 
            // txtPort
            // 
            this.txtPort.Location = new System.Drawing.Point(360, 39);
            this.txtPort.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.txtPort.Name = "txtPort";
            this.txtPort.Size = new System.Drawing.Size(67, 25);
            this.txtPort.TabIndex = 1;
            this.txtPort.Text = "1883";
            // 
            // txtPassword
            // 
            this.txtPassword.Location = new System.Drawing.Point(238, 73);
            this.txtPassword.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.txtPassword.Name = "txtPassword";
            this.txtPassword.PasswordChar = '*';
            this.txtPassword.Size = new System.Drawing.Size(189, 25);
            this.txtPassword.TabIndex = 1;
            this.txtPassword.Text = "user12345";
            // 
            // txtUser
            // 
            this.txtUser.Location = new System.Drawing.Point(76, 73);
            this.txtUser.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.txtUser.Name = "txtUser";
            this.txtUser.Size = new System.Drawing.Size(154, 25);
            this.txtUser.TabIndex = 1;
            this.txtUser.Text = "anonymous";
            // 
            // txtHost
            // 
            this.txtHost.Location = new System.Drawing.Point(76, 39);
            this.txtHost.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.txtHost.Name = "txtHost";
            this.txtHost.Size = new System.Drawing.Size(277, 25);
            this.txtHost.TabIndex = 1;
            this.txtHost.Text = "oakmonte.homedns.org";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(15, 160);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(52, 17);
            this.label4.TabIndex = 0;
            this.label4.Text = "Devices";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(23, 124);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(45, 17);
            this.label3.TabIndex = 0;
            this.label3.Text = "Topics";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(35, 77);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(35, 17);
            this.label2.TabIndex = 0;
            this.label2.Text = "User";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(35, 43);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(35, 17);
            this.label1.TabIndex = 0;
            this.label1.Text = "Host";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.lblIP);
            this.groupBox2.Controls.Add(this.label11);
            this.groupBox2.Controls.Add(this.label15);
            this.groupBox2.Controls.Add(this.label7);
            this.groupBox2.Controls.Add(this.label14);
            this.groupBox2.Controls.Add(this.label6);
            this.groupBox2.Controls.Add(this.label10);
            this.groupBox2.Controls.Add(this.label9);
            this.groupBox2.Controls.Add(this.label12);
            this.groupBox2.Controls.Add(this.label8);
            this.groupBox2.Controls.Add(this.label13);
            this.groupBox2.Controls.Add(this.label5);
            this.groupBox2.Controls.Add(this.cbChart);
            this.groupBox2.Controls.Add(this.cbDSamp);
            this.groupBox2.Controls.Add(this.comboBox6);
            this.groupBox2.Controls.Add(this.cbScale);
            this.groupBox2.Controls.Add(this.cbSamples);
            this.groupBox2.Controls.Add(this.cbAxis);
            this.groupBox2.Controls.Add(this.cbRate);
            this.groupBox2.Controls.Add(this.cbInterval);
            this.groupBox2.Controls.Add(this.cbWindow);
            this.groupBox2.Controls.Add(this.cbFmt);
            this.groupBox2.Controls.Add(this.button1);
            this.groupBox2.Controls.Add(this.lblTime);
            this.groupBox2.Controls.Add(this.lblFirmware);
            this.groupBox2.Controls.Add(this.txtRMS);
            this.groupBox2.Location = new System.Drawing.Point(482, 16);
            this.groupBox2.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Padding = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.groupBox2.Size = new System.Drawing.Size(687, 271);
            this.groupBox2.TabIndex = 0;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Device Manager:";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(225, 236);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(39, 17);
            this.label11.TabIndex = 4;
            this.label11.Text = "Chart";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(215, 172);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(78, 17);
            this.label7.TabIndex = 4;
            this.label7.Text = "Down Samp";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(236, 137);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(57, 17);
            this.label6.TabIndex = 4;
            this.label6.Text = "Acq Size";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(453, 172);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(80, 17);
            this.label10.TabIndex = 4;
            this.label10.Text = "Msg Interval";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(455, 136);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(78, 17);
            this.label9.TabIndex = 4;
            this.label9.Text = "FFT Window";
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(522, 69);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(35, 17);
            this.label12.TabIndex = 4;
            this.label12.Text = "RMS";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(454, 101);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(79, 17);
            this.label8.TabIndex = 4;
            this.label8.Text = "Msg Format";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(233, 101);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(60, 17);
            this.label5.TabIndex = 4;
            this.label5.Text = "Acq Rate";
            // 
            // cbChart
            // 
            this.cbChart.FormattingEnabled = true;
            this.cbChart.Items.AddRange(new object[] {
            "RMS trend",
            "30 Hz Modes (fmt 3)",
            "30 Hz Octaves (fmt 3)",
            "Modes & Octaves (fmt 3)",
            "FFT (fmt 4)",
            "Waterfall (fmt 4)",
            "FFT & Waterfall (fmt 3)"});
            this.cbChart.Location = new System.Drawing.Point(270, 233);
            this.cbChart.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.cbChart.Name = "cbChart";
            this.cbChart.Size = new System.Drawing.Size(140, 25);
            this.cbChart.TabIndex = 3;
            this.cbChart.SelectedIndexChanged += new System.EventHandler(this.chChart_SelectedIndexChanged);
            // 
            // comboBox6
            // 
            this.comboBox6.FormattingEnabled = true;
            this.comboBox6.Location = new System.Drawing.Point(299, 169);
            this.comboBox6.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.comboBox6.Name = "comboBox6";
            this.comboBox6.Size = new System.Drawing.Size(111, 25);
            this.comboBox6.TabIndex = 3;
            // 
            // cbSamples
            // 
            this.cbSamples.FormattingEnabled = true;
            this.cbSamples.Items.AddRange(new object[] {
            "1024",
            "2048",
            "4096"});
            this.cbSamples.Location = new System.Drawing.Point(299, 133);
            this.cbSamples.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.cbSamples.Name = "cbSamples";
            this.cbSamples.Size = new System.Drawing.Size(111, 25);
            this.cbSamples.TabIndex = 3;
            this.cbSamples.SelectedIndexChanged += new System.EventHandler(this.cbSamples_SelectedIndexChanged);
            // 
            // cbRate
            // 
            this.cbRate.FormattingEnabled = true;
            this.cbRate.Items.AddRange(new object[] {
            "1 kHz",
            "2 kHz",
            "3 kHz",
            "4 kHz"});
            this.cbRate.Location = new System.Drawing.Point(299, 98);
            this.cbRate.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.cbRate.Name = "cbRate";
            this.cbRate.Size = new System.Drawing.Size(111, 25);
            this.cbRate.TabIndex = 3;
            this.cbRate.SelectedIndexChanged += new System.EventHandler(this.cbRate_SelectedIndexChanged);
            // 
            // cbInterval
            // 
            this.cbInterval.FormattingEnabled = true;
            this.cbInterval.Items.AddRange(new object[] {
            "1",
            "2",
            "3",
            "4",
            "5",
            "10",
            "30",
            "60"});
            this.cbInterval.Location = new System.Drawing.Point(539, 169);
            this.cbInterval.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.cbInterval.Name = "cbInterval";
            this.cbInterval.Size = new System.Drawing.Size(140, 25);
            this.cbInterval.TabIndex = 3;
            this.cbInterval.SelectedIndexChanged += new System.EventHandler(this.cbInterval_SelectedIndexChanged);
            // 
            // cbWindow
            // 
            this.cbWindow.FormattingEnabled = true;
            this.cbWindow.Location = new System.Drawing.Point(539, 133);
            this.cbWindow.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.cbWindow.Name = "cbWindow";
            this.cbWindow.Size = new System.Drawing.Size(140, 25);
            this.cbWindow.TabIndex = 3;
            this.cbWindow.SelectedIndexChanged += new System.EventHandler(this.cbWindow_SelectedIndexChanged);
            // 
            // cbFmt
            // 
            this.cbFmt.FormattingEnabled = true;
            this.cbFmt.Items.AddRange(new object[] {
            "8 (2)",
            "4+4 (3)",
            "FFT (4)"});
            this.cbFmt.Location = new System.Drawing.Point(539, 98);
            this.cbFmt.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.cbFmt.Name = "cbFmt";
            this.cbFmt.Size = new System.Drawing.Size(140, 25);
            this.cbFmt.TabIndex = 3;
            this.cbFmt.SelectedIndexChanged += new System.EventHandler(this.cbFmt_SelectedIndexChanged);
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(593, 233);
            this.button1.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(87, 30);
            this.button1.TabIndex = 2;
            this.button1.Text = "Reset";
            this.button1.UseVisualStyleBackColor = true;
            // 
            // lblFirmware
            // 
            this.lblFirmware.AutoSize = true;
            this.lblFirmware.Location = new System.Drawing.Point(29, 69);
            this.lblFirmware.Name = "lblFirmware";
            this.lblFirmware.Size = new System.Drawing.Size(64, 17);
            this.lblFirmware.TabIndex = 1;
            this.lblFirmware.Text = "Firmware:";
            // 
            // txtRMS
            // 
            this.txtRMS.Enabled = false;
            this.txtRMS.Location = new System.Drawing.Point(563, 65);
            this.txtRMS.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.txtRMS.Name = "txtRMS";
            this.txtRMS.Size = new System.Drawing.Size(116, 25);
            this.txtRMS.TabIndex = 0;
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.butCmd);
            this.groupBox3.Controls.Add(this.txtCmd);
            this.groupBox3.Controls.Add(this.rtxtConsole);
            this.groupBox3.Location = new System.Drawing.Point(14, 294);
            this.groupBox3.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Padding = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.groupBox3.Size = new System.Drawing.Size(461, 302);
            this.groupBox3.TabIndex = 0;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Console:";
            // 
            // butCmd
            // 
            this.butCmd.Location = new System.Drawing.Point(360, 38);
            this.butCmd.Name = "butCmd";
            this.butCmd.Size = new System.Drawing.Size(95, 23);
            this.butCmd.TabIndex = 2;
            this.butCmd.Text = "Enter";
            this.butCmd.UseVisualStyleBackColor = true;
            this.butCmd.Click += new System.EventHandler(this.butCmd_Click);
            // 
            // txtCmd
            // 
            this.txtCmd.Location = new System.Drawing.Point(7, 37);
            this.txtCmd.Name = "txtCmd";
            this.txtCmd.Size = new System.Drawing.Size(346, 25);
            this.txtCmd.TabIndex = 1;
            // 
            // rtxtConsole
            // 
            this.rtxtConsole.Location = new System.Drawing.Point(6, 68);
            this.rtxtConsole.Name = "rtxtConsole";
            this.rtxtConsole.Size = new System.Drawing.Size(449, 226);
            this.rtxtConsole.TabIndex = 0;
            this.rtxtConsole.Text = "";
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.vibChart);
            this.groupBox4.Location = new System.Drawing.Point(482, 294);
            this.groupBox4.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Padding = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.groupBox4.Size = new System.Drawing.Size(687, 302);
            this.groupBox4.TabIndex = 0;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Data Visualization:";
            // 
            // vibChart
            // 
            chartArea9.Name = "ChartArea1";
            this.vibChart.ChartAreas.Add(chartArea9);
            this.vibChart.Location = new System.Drawing.Point(7, 25);
            this.vibChart.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.vibChart.Name = "vibChart";
            series9.ChartArea = "ChartArea1";
            series9.Name = "Series1";
            this.vibChart.Series.Add(series9);
            this.vibChart.Size = new System.Drawing.Size(673, 269);
            this.vibChart.TabIndex = 0;
            this.vibChart.Text = "vibChart";
            // 
            // cbAxis
            // 
            this.cbAxis.FormattingEnabled = true;
            this.cbAxis.Items.AddRange(new object[] {
            "Acc X",
            "Acc Y",
            "Acc Z"});
            this.cbAxis.Location = new System.Drawing.Point(99, 101);
            this.cbAxis.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.cbAxis.Name = "cbAxis";
            this.cbAxis.Size = new System.Drawing.Size(105, 25);
            this.cbAxis.TabIndex = 3;
            this.cbAxis.SelectedIndexChanged += new System.EventHandler(this.cbAxis_SelectedIndexChanged);
            // 
            // cbScale
            // 
            this.cbScale.FormattingEnabled = true;
            this.cbScale.Items.AddRange(new object[] {
            "0.05",
            "0.06",
            "0.07",
            "0.08",
            "0.09",
            "0.1",
            "0.15",
            "0.2",
            "0.25",
            "0.3"});
            this.cbScale.Location = new System.Drawing.Point(99, 136);
            this.cbScale.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.cbScale.Name = "cbScale";
            this.cbScale.Size = new System.Drawing.Size(105, 25);
            this.cbScale.TabIndex = 3;
            this.cbScale.SelectedIndexChanged += new System.EventHandler(this.cbScale_SelectedIndexChanged);
            // 
            // cbDSamp
            // 
            this.cbDSamp.FormattingEnabled = true;
            this.cbDSamp.Items.AddRange(new object[] {
            "1",
            "2",
            "3",
            "4",
            "5",
            "6",
            "7",
            "8"});
            this.cbDSamp.Location = new System.Drawing.Point(99, 172);
            this.cbDSamp.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.cbDSamp.Name = "cbDSamp";
            this.cbDSamp.Size = new System.Drawing.Size(105, 25);
            this.cbDSamp.TabIndex = 3;
            this.cbDSamp.SelectedIndexChanged += new System.EventHandler(this.cbDSamp_SelectedIndexChanged);
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(33, 104);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(31, 17);
            this.label13.TabIndex = 4;
            this.label13.Text = "Axis";
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Location = new System.Drawing.Point(36, 140);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(38, 17);
            this.label14.TabIndex = 4;
            this.label14.Text = "Scale";
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.Location = new System.Drawing.Point(15, 175);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(78, 17);
            this.label15.TabIndex = 4;
            this.label15.Text = "Down Samp";
            // 
            // lblIP
            // 
            this.lblIP.AutoSize = true;
            this.lblIP.Location = new System.Drawing.Point(236, 68);
            this.lblIP.Name = "lblIP";
            this.lblIP.Size = new System.Drawing.Size(72, 17);
            this.lblIP.TabIndex = 5;
            this.lblIP.Text = "IP address:";
            // 
            // lblTime
            // 
            this.lblTime.AutoSize = true;
            this.lblTime.Location = new System.Drawing.Point(29, 42);
            this.lblTime.Name = "lblTime";
            this.lblTime.Size = new System.Drawing.Size(96, 17);
            this.lblTime.TabIndex = 1;
            this.lblTime.Text = "Date and Time:";
            // 
            // VibManager
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 17F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1183, 612);
            this.Controls.Add(this.groupBox4);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Font = new System.Drawing.Font("Segoe UI", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.Name = "VibManager";
            this.Text = "Vibration Acquisition Manager";
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.groupBox4.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.vibChart)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.ComboBox cbDevices;
        private System.Windows.Forms.ComboBox cbTopics;
        private System.Windows.Forms.Button butConnect;
        private System.Windows.Forms.TextBox txtPort;
        private System.Windows.Forms.TextBox txtPassword;
        private System.Windows.Forms.TextBox txtUser;
        private System.Windows.Forms.TextBox txtHost;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label lblFirmware;
        private System.Windows.Forms.TextBox txtRMS;
        private System.Windows.Forms.DataVisualization.Charting.Chart vibChart;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.ComboBox comboBox6;
        private System.Windows.Forms.ComboBox cbSamples;
        private System.Windows.Forms.ComboBox cbRate;
        private System.Windows.Forms.ComboBox cbInterval;
        private System.Windows.Forms.ComboBox cbWindow;
        private System.Windows.Forms.ComboBox cbFmt;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.ToolTip toolTip1;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.ComboBox cbChart;
        private System.Windows.Forms.Button butCmd;
        private System.Windows.Forms.TextBox txtCmd;
        private System.Windows.Forms.RichTextBox rtxtConsole;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.ComboBox cbDSamp;
        private System.Windows.Forms.ComboBox cbScale;
        private System.Windows.Forms.ComboBox cbAxis;
        private System.Windows.Forms.Label lblIP;
        private System.Windows.Forms.Label lblTime;
    }
}

