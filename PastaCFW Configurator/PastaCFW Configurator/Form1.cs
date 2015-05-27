using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Management;
using System.IO;

namespace PastaCFW_Configurator
{
    public partial class Form1 : Form
    {
        char auto_boot;
        char dump_arm9;
        char type;
        string path;

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            string[] drives = System.IO.Directory.GetLogicalDrives();

            foreach (string item in drives)
            {
                comboBox1.Items.Add(item);
            }
        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            string settings;
            path = comboBox1.SelectedItem + "3ds/PastaCFW/";
            type = '0';
            auto_boot = '0';
            dump_arm9 = '0';
            if (Directory.Exists(path))
            {
                label2.Text = "PastaCFW found!";
                label2.ForeColor = Color.Green;

                if (!File.Exists(path + "system.txt") || File.ReadAllText(path + "system.txt").Length != 3) prepareSettings(); //If settings.txt is not found or not complete, reset it
                settings = File.ReadAllText(path + "system.txt");
                type = settings[0];
                auto_boot = settings[1];
                dump_arm9 = settings[2];

                textBox2.Text = type.ToString();
                switch (type)
                {
                    case '1': // 4.x
                        textBox1.Text = "Old 3DS V. 4.1 - 4.5";
                        break;
                    case '2': // 5.0
                        textBox1.Text = "Old 3DS V. 5.0";
                        break;
                    case '3': // 5.1
                        textBox1.Text = "Old 3DS V. 5.1";
                        break;
                    case '4': // 6.0
                        textBox1.Text = "Old 3DS V. 6.0";
                        break;
                    case '5': // 6.1 - 6.3
                        textBox1.Text = "Old 3DS V. 6.1 - 6.3";
                        break;
                    case '6': // 7.0-7.1
                        textBox1.Text = "Old 3DS V. 7.0 - 7.1";
                        break;
                    case '7': // 7.2
                        textBox1.Text = "Old 3DS V. 7.2";
                        break;
                    case '8': // 8.x
                        textBox1.Text = "Old 3DS V. 8.0 - 8.1";
                        break;
                    case '9': // 9.x
                        textBox1.Text = "Old 3DS V. 9.0 - 9.2";
                        break;
                    case 'a': // 8.x
                        textBox1.Text = "New 3DS V. 8.1";
                        break;
                    case 'b': // 9.x
                        textBox1.Text = "New 3DS V. 9.0 - 9.2";
                        break;
                }
                //autoboot checkbox
                if (auto_boot == '1') checkBox2.Checked = true;
                else checkBox2.Checked = false;

                //dump arm9 checkbox
                if (dump_arm9 == '1') checkBox1.Checked = true;
                else checkBox1.Checked = false;
            }

            else
            {
                label2.Text = "PastaCFW not found!";
                label2.ForeColor = Color.Red;
                textBox2.Text = null;
                textBox1.Text = null;
                checkBox2.Checked = false;
                checkBox1.Checked = false;
            }
        }

        private void groupBox2_Enter(object sender, EventArgs e)
        {

        }

        private void checkBox2_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox2.Checked == true) auto_boot = '1';
            else auto_boot = '0';
            updateFile(type,auto_boot,dump_arm9);
        }

        private void updateFile(char arg1,char arg2,char arg3)
        {
            if (File.Exists(path + "system.txt"))File.WriteAllText(path + "system.txt", arg1.ToString() + arg2.ToString() + arg3.ToString());
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox1.Checked == true) dump_arm9 = '1';
            else auto_boot = '0';
            updateFile(type, auto_boot, dump_arm9);
        }

        private void prepareSettings()
        {
            MessageBox.Show("Pasta CFW has been found but the system.txt file is not found or not complete. Press OK to create it.", "ERROR", MessageBoxButtons.OK, MessageBoxIcon.Error);
            updateFile('0', '0', '0');
        }

    }
}
