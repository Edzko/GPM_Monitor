
namespace GPM_MQTTClient2
{
    partial class Dashboard
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
            this.dataGridView1 = new System.Windows.Forms.DataGridView();
            this.deviceDataBindingSource = new System.Windows.Forms.BindingSource(this.components);
            this.name = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.timestamp = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.rms = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Firmware = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.deviceDataBindingSource1 = new System.Windows.Forms.BindingSource(this.components);
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.deviceDataBindingSource)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.deviceDataBindingSource1)).BeginInit();
            this.SuspendLayout();
            // 
            // dataGridView1
            // 
            this.dataGridView1.AllowUserToAddRows = false;
            this.dataGridView1.AllowUserToDeleteRows = false;
            this.dataGridView1.AutoGenerateColumns = false;
            this.dataGridView1.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridView1.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.name,
            this.timestamp,
            this.rms,
            this.Firmware});
            this.dataGridView1.DataSource = this.deviceDataBindingSource1;
            this.dataGridView1.Location = new System.Drawing.Point(12, 12);
            this.dataGridView1.Name = "dataGridView1";
            this.dataGridView1.ReadOnly = true;
            this.dataGridView1.Size = new System.Drawing.Size(455, 353);
            this.dataGridView1.TabIndex = 0;
            // 
            // deviceDataBindingSource
            // 
            this.deviceDataBindingSource.DataSource = typeof(GPM_MQTTClient2.VibManager.DeviceData);
            // 
            // name
            // 
            this.name.HeaderText = "Name";
            this.name.Name = "name";
            this.name.ReadOnly = true;
            // 
            // timestamp
            // 
            this.timestamp.HeaderText = "Last Msg";
            this.timestamp.Name = "timestamp";
            this.timestamp.ReadOnly = true;
            // 
            // rms
            // 
            this.rms.HeaderText = "RMS";
            this.rms.Name = "rms";
            this.rms.ReadOnly = true;
            // 
            // Firmware
            // 
            this.Firmware.HeaderText = "firmware";
            this.Firmware.Name = "Firmware";
            this.Firmware.ReadOnly = true;
            // 
            // deviceDataBindingSource1
            // 
            this.deviceDataBindingSource1.DataSource = typeof(GPM_MQTTClient2.VibManager.DeviceData);
            // 
            // Dashboard
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 17F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(481, 377);
            this.Controls.Add(this.dataGridView1);
            this.Font = new System.Drawing.Font("Segoe UI", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.Name = "Dashboard";
            this.Text = "Dashboard";
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.deviceDataBindingSource)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.deviceDataBindingSource1)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.DataGridView dataGridView1;
        private System.Windows.Forms.BindingSource deviceDataBindingSource;
        private System.Windows.Forms.DataGridViewTextBoxColumn name;
        private System.Windows.Forms.DataGridViewTextBoxColumn timestamp;
        private System.Windows.Forms.DataGridViewTextBoxColumn rms;
        private System.Windows.Forms.DataGridViewTextBoxColumn Firmware;
        private System.Windows.Forms.BindingSource deviceDataBindingSource1;
    }
}