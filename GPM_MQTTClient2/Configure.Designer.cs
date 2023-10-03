
namespace GPM_MQTTClient2
{
    partial class Configure
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
            this.butRetrieve = new System.Windows.Forms.Button();
            this.butStore = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.textBox2 = new System.Windows.Forms.TextBox();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.txtTopic = new System.Windows.Forms.TextBox();
            this.txtName = new System.Windows.Forms.TextBox();
            this.label22 = new System.Windows.Forms.Label();
            this.label21 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.txtIP = new System.Windows.Forms.TextBox();
            this.txtSSIDpassword = new System.Windows.Forms.TextBox();
            this.txtSSID = new System.Windows.Forms.TextBox();
            this.cbSSIDchanged = new System.Windows.Forms.CheckBox();
            this.label20 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.cbInterval = new System.Windows.Forms.ComboBox();
            this.cbAxis = new System.Windows.Forms.ComboBox();
            this.cbScale = new System.Windows.Forms.ComboBox();
            this.cbDSampl = new System.Windows.Forms.ComboBox();
            this.cbWindow = new System.Windows.Forms.ComboBox();
            this.cbPnts = new System.Windows.Forms.ComboBox();
            this.cbFreq = new System.Windows.Forms.ComboBox();
            this.label11 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label12 = new System.Windows.Forms.Label();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.cbCOMList = new System.Windows.Forms.ComboBox();
            this.label19 = new System.Windows.Forms.Label();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.label18 = new System.Windows.Forms.Label();
            this.label17 = new System.Windows.Forms.Label();
            this.label16 = new System.Windows.Forms.Label();
            this.label15 = new System.Windows.Forms.Label();
            this.label14 = new System.Windows.Forms.Label();
            this.txtMQTTQoS = new System.Windows.Forms.TextBox();
            this.txtMQTThost = new System.Windows.Forms.TextBox();
            this.txtMQTTport = new System.Windows.Forms.TextBox();
            this.txtPassword = new System.Windows.Forms.TextBox();
            this.txtUsername = new System.Windows.Forms.TextBox();
            this.label13 = new System.Windows.Forms.Label();
            this.butClose = new System.Windows.Forms.Button();
            this.butHelp = new System.Windows.Forms.Button();
            this.butSave = new System.Windows.Forms.Button();
            this.butLoad = new System.Windows.Forms.Button();
            this.butReset = new System.Windows.Forms.Button();
            this.toolTip1 = new System.Windows.Forms.ToolTip(this.components);
            this.butUpdate = new System.Windows.Forms.Button();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.SuspendLayout();
            // 
            // butRetrieve
            // 
            this.butRetrieve.Font = new System.Drawing.Font("Segoe UI", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.butRetrieve.Location = new System.Drawing.Point(12, 445);
            this.butRetrieve.Name = "butRetrieve";
            this.butRetrieve.Size = new System.Drawing.Size(75, 30);
            this.butRetrieve.TabIndex = 0;
            this.butRetrieve.Text = "Retrieve";
            this.butRetrieve.UseVisualStyleBackColor = true;
            this.butRetrieve.Click += new System.EventHandler(this.butRetrieve_Click);
            // 
            // butStore
            // 
            this.butStore.Font = new System.Drawing.Font("Segoe UI", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.butStore.Location = new System.Drawing.Point(93, 445);
            this.butStore.Name = "butStore";
            this.butStore.Size = new System.Drawing.Size(75, 30);
            this.butStore.TabIndex = 1;
            this.butStore.Text = "Store";
            this.butStore.UseVisualStyleBackColor = true;
            this.butStore.Click += new System.EventHandler(this.butStore_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(31, 30);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(75, 13);
            this.label1.TabIndex = 2;
            this.label1.Text = "Device Name:";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.textBox2);
            this.groupBox1.Controls.Add(this.textBox1);
            this.groupBox1.Controls.Add(this.txtTopic);
            this.groupBox1.Controls.Add(this.txtName);
            this.groupBox1.Controls.Add(this.label22);
            this.groupBox1.Controls.Add(this.label21);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Location = new System.Drawing.Point(424, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(406, 201);
            this.groupBox1.TabIndex = 3;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Device Configuration:";
            // 
            // textBox2
            // 
            this.textBox2.Location = new System.Drawing.Point(112, 122);
            this.textBox2.Name = "textBox2";
            this.textBox2.Size = new System.Drawing.Size(189, 20);
            this.textBox2.TabIndex = 4;
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(112, 96);
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(189, 20);
            this.textBox1.TabIndex = 4;
            // 
            // txtTopic
            // 
            this.txtTopic.Location = new System.Drawing.Point(112, 53);
            this.txtTopic.Name = "txtTopic";
            this.txtTopic.Size = new System.Drawing.Size(189, 20);
            this.txtTopic.TabIndex = 4;
            // 
            // txtName
            // 
            this.txtName.Location = new System.Drawing.Point(112, 27);
            this.txtName.Name = "txtName";
            this.txtName.Size = new System.Drawing.Size(189, 20);
            this.txtName.TabIndex = 4;
            // 
            // label22
            // 
            this.label22.AutoSize = true;
            this.label22.Location = new System.Drawing.Point(53, 125);
            this.label22.Name = "label22";
            this.label22.Size = new System.Drawing.Size(53, 13);
            this.label22.TabIndex = 2;
            this.label22.Text = "NTP port:";
            // 
            // label21
            // 
            this.label21.AutoSize = true;
            this.label21.Location = new System.Drawing.Point(48, 99);
            this.label21.Name = "label21";
            this.label21.Size = new System.Drawing.Size(57, 13);
            this.label21.TabIndex = 2;
            this.label21.Text = "NTP Host:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(35, 56);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(71, 13);
            this.label2.TabIndex = 2;
            this.label2.Text = "MQTT Topic:";
            // 
            // txtIP
            // 
            this.txtIP.Location = new System.Drawing.Point(129, 142);
            this.txtIP.Name = "txtIP";
            this.txtIP.Size = new System.Drawing.Size(189, 20);
            this.txtIP.TabIndex = 4;
            this.toolTip1.SetToolTip(this.txtIP, "When Changing WiFi: Set Static IP. Leave blanc for dynamic IP");
            // 
            // txtSSIDpassword
            // 
            this.txtSSIDpassword.Location = new System.Drawing.Point(129, 93);
            this.txtSSIDpassword.Name = "txtSSIDpassword";
            this.txtSSIDpassword.Size = new System.Drawing.Size(189, 20);
            this.txtSSIDpassword.TabIndex = 4;
            // 
            // txtSSID
            // 
            this.txtSSID.Location = new System.Drawing.Point(129, 67);
            this.txtSSID.Name = "txtSSID";
            this.txtSSID.Size = new System.Drawing.Size(189, 20);
            this.txtSSID.TabIndex = 4;
            // 
            // cbSSIDchanged
            // 
            this.cbSSIDchanged.AutoSize = true;
            this.cbSSIDchanged.Location = new System.Drawing.Point(129, 119);
            this.cbSSIDchanged.Name = "cbSSIDchanged";
            this.cbSSIDchanged.Size = new System.Drawing.Size(93, 17);
            this.cbSSIDchanged.TabIndex = 3;
            this.cbSSIDchanged.Text = "WiFi Changed";
            this.toolTip1.SetToolTip(this.cbSSIDchanged, "Check this box to re-configure the WiFi settings on the device");
            this.cbSSIDchanged.UseVisualStyleBackColor = true;
            // 
            // label20
            // 
            this.label20.AutoSize = true;
            this.label20.Location = new System.Drawing.Point(73, 145);
            this.label20.Name = "label20";
            this.label20.Size = new System.Drawing.Size(50, 13);
            this.label20.TabIndex = 2;
            this.label20.Text = "Static IP:";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(43, 96);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(80, 13);
            this.label4.TabIndex = 2;
            this.label4.Text = "WiFi Password:";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(64, 70);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(59, 13);
            this.label3.TabIndex = 2;
            this.label3.Text = "WiFi SSID:";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.cbInterval);
            this.groupBox2.Controls.Add(this.cbAxis);
            this.groupBox2.Controls.Add(this.cbScale);
            this.groupBox2.Controls.Add(this.cbDSampl);
            this.groupBox2.Controls.Add(this.cbWindow);
            this.groupBox2.Controls.Add(this.cbPnts);
            this.groupBox2.Controls.Add(this.cbFreq);
            this.groupBox2.Controls.Add(this.label11);
            this.groupBox2.Controls.Add(this.label10);
            this.groupBox2.Controls.Add(this.label9);
            this.groupBox2.Controls.Add(this.label8);
            this.groupBox2.Controls.Add(this.label7);
            this.groupBox2.Controls.Add(this.label6);
            this.groupBox2.Controls.Add(this.label5);
            this.groupBox2.Location = new System.Drawing.Point(12, 199);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(405, 240);
            this.groupBox2.TabIndex = 4;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Data Acquisition Settings:";
            // 
            // cbInterval
            // 
            this.cbInterval.FormattingEnabled = true;
            this.cbInterval.Location = new System.Drawing.Point(116, 195);
            this.cbInterval.Name = "cbInterval";
            this.cbInterval.Size = new System.Drawing.Size(121, 21);
            this.cbInterval.TabIndex = 5;
            // 
            // cbAxis
            // 
            this.cbAxis.FormattingEnabled = true;
            this.cbAxis.Location = new System.Drawing.Point(116, 168);
            this.cbAxis.Name = "cbAxis";
            this.cbAxis.Size = new System.Drawing.Size(121, 21);
            this.cbAxis.TabIndex = 5;
            // 
            // cbScale
            // 
            this.cbScale.FormattingEnabled = true;
            this.cbScale.Location = new System.Drawing.Point(116, 141);
            this.cbScale.Name = "cbScale";
            this.cbScale.Size = new System.Drawing.Size(121, 21);
            this.cbScale.TabIndex = 5;
            // 
            // cbDSampl
            // 
            this.cbDSampl.FormattingEnabled = true;
            this.cbDSampl.Location = new System.Drawing.Point(116, 114);
            this.cbDSampl.Name = "cbDSampl";
            this.cbDSampl.Size = new System.Drawing.Size(121, 21);
            this.cbDSampl.TabIndex = 5;
            // 
            // cbWindow
            // 
            this.cbWindow.FormattingEnabled = true;
            this.cbWindow.Location = new System.Drawing.Point(116, 87);
            this.cbWindow.Name = "cbWindow";
            this.cbWindow.Size = new System.Drawing.Size(121, 21);
            this.cbWindow.TabIndex = 5;
            // 
            // cbPnts
            // 
            this.cbPnts.FormattingEnabled = true;
            this.cbPnts.Location = new System.Drawing.Point(116, 60);
            this.cbPnts.Name = "cbPnts";
            this.cbPnts.Size = new System.Drawing.Size(121, 21);
            this.cbPnts.TabIndex = 5;
            // 
            // cbFreq
            // 
            this.cbFreq.FormattingEnabled = true;
            this.cbFreq.Location = new System.Drawing.Point(116, 33);
            this.cbFreq.Name = "cbFreq";
            this.cbFreq.Size = new System.Drawing.Size(121, 21);
            this.cbFreq.TabIndex = 5;
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(43, 198);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(67, 13);
            this.label11.TabIndex = 4;
            this.label11.Text = "MQTT Rate:";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(51, 171);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(59, 13);
            this.label10.TabIndex = 3;
            this.label10.Text = "Accel Axis:";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(35, 144);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(75, 13);
            this.label9.TabIndex = 3;
            this.label9.Text = "Scaling factor:";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(28, 117);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(82, 13);
            this.label8.TabIndex = 3;
            this.label8.Text = "Down sampling:";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(39, 90);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(71, 13);
            this.label7.TabIndex = 2;
            this.label7.Text = "FFT Window:";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(56, 63);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(54, 13);
            this.label6.TabIndex = 1;
            this.label6.Text = "Data size:";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(51, 36);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(60, 13);
            this.label5.TabIndex = 0;
            this.label5.Text = "Frequency:";
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(48, 34);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(58, 13);
            this.label12.TabIndex = 5;
            this.label12.Text = "Username:";
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.txtIP);
            this.groupBox3.Controls.Add(this.cbCOMList);
            this.groupBox3.Controls.Add(this.txtSSIDpassword);
            this.groupBox3.Controls.Add(this.label19);
            this.groupBox3.Controls.Add(this.txtSSID);
            this.groupBox3.Controls.Add(this.label3);
            this.groupBox3.Controls.Add(this.label4);
            this.groupBox3.Controls.Add(this.cbSSIDchanged);
            this.groupBox3.Controls.Add(this.label20);
            this.groupBox3.Location = new System.Drawing.Point(13, 12);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(405, 174);
            this.groupBox3.TabIndex = 5;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "USB Connection / WiFi Configuration:";
            // 
            // cbCOMList
            // 
            this.cbCOMList.FormattingEnabled = true;
            this.cbCOMList.Location = new System.Drawing.Point(68, 27);
            this.cbCOMList.Name = "cbCOMList";
            this.cbCOMList.Size = new System.Drawing.Size(250, 21);
            this.cbCOMList.TabIndex = 1;
            this.toolTip1.SetToolTip(this.cbCOMList, "Select USB COM port to device");
            // 
            // label19
            // 
            this.label19.AutoSize = true;
            this.label19.Location = new System.Drawing.Point(7, 30);
            this.label19.Name = "label19";
            this.label19.Size = new System.Drawing.Size(55, 13);
            this.label19.TabIndex = 0;
            this.label19.Text = "COM port:";
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.label18);
            this.groupBox4.Controls.Add(this.label17);
            this.groupBox4.Controls.Add(this.label16);
            this.groupBox4.Controls.Add(this.label15);
            this.groupBox4.Controls.Add(this.label14);
            this.groupBox4.Controls.Add(this.txtMQTTQoS);
            this.groupBox4.Controls.Add(this.txtMQTThost);
            this.groupBox4.Controls.Add(this.txtMQTTport);
            this.groupBox4.Controls.Add(this.txtPassword);
            this.groupBox4.Controls.Add(this.txtUsername);
            this.groupBox4.Controls.Add(this.label13);
            this.groupBox4.Controls.Add(this.label12);
            this.groupBox4.Location = new System.Drawing.Point(424, 219);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(406, 220);
            this.groupBox4.TabIndex = 6;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "MQTT Device Configuration:";
            // 
            // label18
            // 
            this.label18.AutoSize = true;
            this.label18.Location = new System.Drawing.Point(247, 170);
            this.label18.Name = "label18";
            this.label18.Size = new System.Drawing.Size(54, 13);
            this.label18.TabIndex = 5;
            this.label18.Text = "Certificate";
            // 
            // label17
            // 
            this.label17.AutoSize = true;
            this.label17.Location = new System.Drawing.Point(109, 170);
            this.label17.Name = "label17";
            this.label17.Size = new System.Drawing.Size(57, 13);
            this.label17.TabIndex = 5;
            this.label17.Text = "Encryption";
            // 
            // label16
            // 
            this.label16.AutoSize = true;
            this.label16.Location = new System.Drawing.Point(77, 138);
            this.label16.Name = "label16";
            this.label16.Size = new System.Drawing.Size(31, 13);
            this.label16.TabIndex = 5;
            this.label16.Text = "QoS:";
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.Location = new System.Drawing.Point(74, 112);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(32, 13);
            this.label15.TabIndex = 5;
            this.label15.Text = "Host:";
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Location = new System.Drawing.Point(77, 86);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(29, 13);
            this.label14.TabIndex = 5;
            this.label14.Text = "Port:";
            // 
            // txtMQTTQoS
            // 
            this.txtMQTTQoS.Location = new System.Drawing.Point(112, 135);
            this.txtMQTTQoS.Name = "txtMQTTQoS";
            this.txtMQTTQoS.Size = new System.Drawing.Size(189, 20);
            this.txtMQTTQoS.TabIndex = 4;
            // 
            // txtMQTThost
            // 
            this.txtMQTThost.Location = new System.Drawing.Point(112, 109);
            this.txtMQTThost.Name = "txtMQTThost";
            this.txtMQTThost.Size = new System.Drawing.Size(189, 20);
            this.txtMQTThost.TabIndex = 4;
            // 
            // txtMQTTport
            // 
            this.txtMQTTport.Location = new System.Drawing.Point(112, 83);
            this.txtMQTTport.Name = "txtMQTTport";
            this.txtMQTTport.Size = new System.Drawing.Size(189, 20);
            this.txtMQTTport.TabIndex = 4;
            // 
            // txtPassword
            // 
            this.txtPassword.Location = new System.Drawing.Point(112, 57);
            this.txtPassword.Name = "txtPassword";
            this.txtPassword.Size = new System.Drawing.Size(189, 20);
            this.txtPassword.TabIndex = 4;
            // 
            // txtUsername
            // 
            this.txtUsername.Location = new System.Drawing.Point(112, 31);
            this.txtUsername.Name = "txtUsername";
            this.txtUsername.Size = new System.Drawing.Size(189, 20);
            this.txtUsername.TabIndex = 4;
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(50, 60);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(56, 13);
            this.label13.TabIndex = 5;
            this.label13.Text = "Password:";
            // 
            // butClose
            // 
            this.butClose.Font = new System.Drawing.Font("Segoe UI", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.butClose.Location = new System.Drawing.Point(755, 445);
            this.butClose.Name = "butClose";
            this.butClose.Size = new System.Drawing.Size(75, 30);
            this.butClose.TabIndex = 1;
            this.butClose.Text = "Close";
            this.butClose.UseVisualStyleBackColor = true;
            this.butClose.Click += new System.EventHandler(this.butClose_Click);
            // 
            // butHelp
            // 
            this.butHelp.Font = new System.Drawing.Font("Segoe UI", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.butHelp.Location = new System.Drawing.Point(674, 445);
            this.butHelp.Name = "butHelp";
            this.butHelp.Size = new System.Drawing.Size(75, 30);
            this.butHelp.TabIndex = 1;
            this.butHelp.Text = "Help";
            this.butHelp.UseVisualStyleBackColor = true;
            this.butHelp.Click += new System.EventHandler(this.butHelp_Click);
            // 
            // butSave
            // 
            this.butSave.Font = new System.Drawing.Font("Segoe UI", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.butSave.Location = new System.Drawing.Point(337, 445);
            this.butSave.Name = "butSave";
            this.butSave.Size = new System.Drawing.Size(75, 30);
            this.butSave.TabIndex = 1;
            this.butSave.Text = "Save";
            this.butSave.UseVisualStyleBackColor = true;
            this.butSave.Click += new System.EventHandler(this.butSave_Click);
            // 
            // butLoad
            // 
            this.butLoad.Font = new System.Drawing.Font("Segoe UI", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.butLoad.Location = new System.Drawing.Point(256, 445);
            this.butLoad.Name = "butLoad";
            this.butLoad.Size = new System.Drawing.Size(75, 30);
            this.butLoad.TabIndex = 1;
            this.butLoad.Text = "Load";
            this.butLoad.UseVisualStyleBackColor = true;
            this.butLoad.Click += new System.EventHandler(this.butLoad_Click);
            // 
            // butReset
            // 
            this.butReset.Font = new System.Drawing.Font("Segoe UI", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.butReset.Location = new System.Drawing.Point(174, 445);
            this.butReset.Name = "butReset";
            this.butReset.Size = new System.Drawing.Size(75, 30);
            this.butReset.TabIndex = 1;
            this.butReset.Text = "Reset";
            this.butReset.UseVisualStyleBackColor = true;
            // 
            // butUpdate
            // 
            this.butUpdate.Font = new System.Drawing.Font("Segoe UI", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.butUpdate.Location = new System.Drawing.Point(424, 445);
            this.butUpdate.Name = "butUpdate";
            this.butUpdate.Size = new System.Drawing.Size(152, 30);
            this.butUpdate.TabIndex = 1;
            this.butUpdate.Text = "Update Firmware";
            this.butUpdate.UseVisualStyleBackColor = true;
            this.butUpdate.Click += new System.EventHandler(this.butUpdate_Click);
            // 
            // Configure
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(843, 483);
            this.Controls.Add(this.groupBox4);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.butHelp);
            this.Controls.Add(this.butClose);
            this.Controls.Add(this.butReset);
            this.Controls.Add(this.butLoad);
            this.Controls.Add(this.butUpdate);
            this.Controls.Add(this.butSave);
            this.Controls.Add(this.butStore);
            this.Controls.Add(this.butRetrieve);
            this.Name = "Configure";
            this.Text = "Configure Device";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.Configure_FormClosed);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button butRetrieve;
        private System.Windows.Forms.Button butStore;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.TextBox txtSSIDpassword;
        private System.Windows.Forms.TextBox txtSSID;
        private System.Windows.Forms.TextBox txtTopic;
        private System.Windows.Forms.TextBox txtName;
        private System.Windows.Forms.CheckBox cbSSIDchanged;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox txtIP;
        private System.Windows.Forms.Label label20;
        private System.Windows.Forms.ComboBox cbInterval;
        private System.Windows.Forms.ComboBox cbAxis;
        private System.Windows.Forms.ComboBox cbScale;
        private System.Windows.Forms.ComboBox cbDSampl;
        private System.Windows.Forms.ComboBox cbWindow;
        private System.Windows.Forms.ComboBox cbPnts;
        private System.Windows.Forms.ComboBox cbFreq;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.ComboBox cbCOMList;
        private System.Windows.Forms.Label label19;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.Label label18;
        private System.Windows.Forms.Label label17;
        private System.Windows.Forms.Label label16;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.TextBox txtMQTTQoS;
        private System.Windows.Forms.TextBox txtMQTThost;
        private System.Windows.Forms.TextBox txtMQTTport;
        private System.Windows.Forms.TextBox txtPassword;
        private System.Windows.Forms.TextBox txtUsername;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.Button butClose;
        private System.Windows.Forms.Button butHelp;
        private System.Windows.Forms.Button butSave;
        private System.Windows.Forms.Button butLoad;
        private System.Windows.Forms.Button butReset;
        private System.Windows.Forms.TextBox textBox2;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Label label22;
        private System.Windows.Forms.Label label21;
        private System.Windows.Forms.ToolTip toolTip1;
        private System.Windows.Forms.Button butUpdate;
    }
}