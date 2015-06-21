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
using System.Net;
using System.Security.Cryptography;

namespace PastaCFW_Configurator
{
    public partial class Form1 : Form
    {
        char auto_boot;
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
            if (Directory.Exists(path))
            {
                label2.Text = "PastaCFW found!";
                label2.ForeColor = Color.Green;

                if (!File.Exists(path + "system.txt") || File.ReadAllText(path + "system.txt").Length != 2) prepareSettings(); //If settings.txt is not found or not complete, reset it
                settings = File.ReadAllText(path + "system.txt");
                type = settings[0];
                auto_boot = settings[1];

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
                if (auto_boot == '2') checkBox2.Checked = true;
                else checkBox2.Checked = false;
            }

            else
            {
                label2.Text = "PastaCFW not found!";
                label2.ForeColor = Color.Red;
                textBox2.Text = null;
                textBox1.Text = null;
                checkBox2.Checked = false;
            }
        }

        private void groupBox2_Enter(object sender, EventArgs e)
        {

        }

        private void checkBox2_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox2.Checked == true) auto_boot = '2';
            else auto_boot = '1';
            updateFile(type,auto_boot);
        }

        private void updateFile(char arg1,char arg2)
        {
            if (File.Exists(path + "system.txt"))File.WriteAllText(path + "system.txt", arg1.ToString() + arg2.ToString());
        }

        private void prepareSettings()
        {
            MessageBox.Show("Pasta CFW has been found but the system.txt file is not found or not complete. Press OK to create it.", "ERROR", MessageBoxButtons.OK, MessageBoxIcon.Error);
            File.WriteAllText(path + "system.txt", "00");
        }

        private void button1_Click(object sender, EventArgs e)
        {
            progressBar1.Value = 5;
            textBox3.Text = null;
            // ---------------- PART1: DOWNLOAD FILES ------------------
            WebClient downloader = new WebClient();
            string file1_url;
            string file2_url;
            string file3_url;
            string file4_url;
            if (comboBox2.SelectedIndex == 0) //oLD 3DS
            {
                file1_url = "http://albertosonic.bplaced.net/pasta/O3DS_file1.bin";
                file2_url = "http://albertosonic.bplaced.net/pasta/O3DS_file2.bin";
                file3_url = "http://nus.cdn.c.shop.nintendowifi.net/ccs/download/0004013800000002/00000049";
                file4_url = "http://nus.cdn.c.shop.nintendowifi.net/ccs/download/0004013800000002/cetk";
            }
            else //New 3DS
            {
                file1_url = "http://albertosonic.bplaced.net/pasta/N3DS_file1.bin";
                file2_url = "http://albertosonic.bplaced.net/pasta/N3DS_file2.bin";
                file3_url = "http://nus.cdn.c.shop.nintendowifi.net/ccs/download/0004013820000002/0000000f";
                file4_url = "http://nus.cdn.c.shop.nintendowifi.net/ccs/download/0004013820000002/cetk";
            }

            textBox3.AppendText("Downloading file1.bin");
            textBox3.AppendText(Environment.NewLine);
            downloader.DownloadFile(file1_url, "file1.bin");

            textBox3.AppendText("Downloading file2.bin");
            textBox3.AppendText(Environment.NewLine);
            downloader.DownloadFile(file2_url, "file2.bin");

            textBox3.AppendText("Downloading file3.bin");
            textBox3.AppendText(Environment.NewLine);
            downloader.DownloadFile(file3_url, "file3.bin");

            textBox3.AppendText("Downloading file4.bin");
            textBox3.AppendText(Environment.NewLine);
            downloader.DownloadFile(file4_url, "file4.bin");

            progressBar1.Value = 10;

            //------------------------ PART 2: READ CETK ------------------------
            textBox3.AppendText("Getting the key");
            textBox3.AppendText(Environment.NewLine);

            byte[] file4_array = new byte[0x10];
            FileStream file4_stream = new FileStream("file4.bin", FileMode.Open);
            file4_stream.Seek(0x1BF, SeekOrigin.Begin);
            file4_stream.Read(file4_array, 0, 0x10);

            byte[] file1_array= new byte[0x10];
            FileStream file1_stream = new FileStream("file1.bin", FileMode.Open);
            file1_stream.Seek(0, SeekOrigin.Begin);
            file1_stream.Read(file1_array, 0, 0x10);

            byte[] key = new byte[0x10];
            for (int i = 0; i < file4_array.Length; i++)
            {
                key[i] = (byte)(file4_array[i] ^ file1_array[i]);
            }

            progressBar1.Value = 30;
            //----------------------- PART 3: DECRYPT FILE3.BIN ------------------
            textBox3.AppendText("Decrypting");
            textBox3.AppendText(Environment.NewLine);

            byte[] file3_array = File.ReadAllBytes("file3.bin");
            byte[] file3_dec=AES_Decrypt(file3_array, key);

            progressBar1.Value = 50;
            //----------------------- PART 4: EXEFS ------------------------------
            textBox3.AppendText("Getting EXEFS info");
            textBox3.AppendText(Environment.NewLine);

            byte[] EXEFS_offset_array = new byte[4];
            EXEFS_offset_array[0] = file3_dec[0x1A3];
            EXEFS_offset_array[1] = file3_dec[0x1A2];
            EXEFS_offset_array[2] = file3_dec[0x1A1];
            EXEFS_offset_array[3] = file3_dec[0x1A0];
            var EXEFS_offset = EXEFS_offset_array[0] << 8 | EXEFS_offset_array[1]; //TODO: DO THIS BETTER
            EXEFS_offset = EXEFS_offset << 8 | EXEFS_offset_array[2];
            EXEFS_offset = EXEFS_offset << 8 | EXEFS_offset_array[3];
            EXEFS_offset *= 0x200;

            byte[] EXEFS_size_array = new byte[4];
            EXEFS_size_array[0] = file3_dec[0x1A7];
            EXEFS_size_array[1] = file3_dec[0x1A6];
            EXEFS_size_array[2] = file3_dec[0x1A5];
            EXEFS_size_array[3] = file3_dec[0x1A4];
            var EXEFS_size = EXEFS_size_array[0] << 8 | EXEFS_size_array[1]; //TODO: DO THIS BETTER
            EXEFS_size = EXEFS_size << 8 | EXEFS_size_array[2];
            EXEFS_size = EXEFS_size << 8 | EXEFS_size_array[3];
            EXEFS_size *= 0x200;

            progressBar1.Value = 70;
            //-------------------------- PART 5: FIRM --------------------------
            textBox3.AppendText("Extracting EXEFS");
            textBox3.AppendText(Environment.NewLine);

            byte[] file2_array = new byte[EXEFS_size];
            FileStream file2_stream = new FileStream("file2.bin", FileMode.Open);
            file2_stream.Seek(0, SeekOrigin.Begin);
            file2_stream.Read(file2_array, 0, EXEFS_size);

            byte[] firm = new byte[EXEFS_size];
            for (int i = 0; i < EXEFS_size; i++)
            {
                firm[i] = (byte)(file2_array[i] ^ file3_dec[EXEFS_offset+i]);
            }
            progressBar1.Value = 90;

            textBox3.AppendText("Saving the FIRM.bin");
            textBox3.AppendText(Environment.NewLine);

            FileStream firm_writer = new FileStream(comboBox1.SelectedItem + "3ds/PastaCFW/firm.bin", FileMode.Create);
            for (int i = 0; i < EXEFS_size-0x200; i++)
            {
                firm_writer.WriteByte(firm[0x200 + i]);
            }
            firm_writer.Close();

            file4_stream.Close();
            file2_stream.Close();
            file1_stream.Close();

            File.Delete("file1.bin");
            File.Delete("file2.bin");
            File.Delete("file3.bin");
            File.Delete("file4.bin");

            progressBar1.Value = 100;
            textBox3.AppendText("DONE!");
            textBox3.AppendText(Environment.NewLine);
            MessageBox.Show("FIRM.bin downloaded!", "YEAH");
        }

        private void comboBox2_SelectedIndexChanged(object sender, EventArgs e)
        {
            button1.Enabled = true;
        }

        public byte[] AES_Decrypt(byte[] bytesToBeDecrypted, byte[] key)
        {
            byte[] decryptedBytes = null;

            using (MemoryStream ms = new MemoryStream())
            {
                using (RijndaelManaged AES = new RijndaelManaged())
                {
                    AES.KeySize = 128;
                    AES.BlockSize = 128;

                    byte[] IV = new byte[16];
                    for (int i = 0; i < IV.Length; i++)
                    {
                        IV[i] = 0x00;
                    }
                    AES.Key = key;
                    AES.IV = IV;
                    AES.Padding = PaddingMode.Zeros;

                    AES.Mode = CipherMode.CBC;

                    using (var cs = new CryptoStream(ms, AES.CreateDecryptor(), CryptoStreamMode.Write))
                    {
                        cs.Write(bytesToBeDecrypted, 0, bytesToBeDecrypted.Length);
                        cs.Close();
                    }
                    decryptedBytes = ms.ToArray();
                }
            }

            return decryptedBytes;
        }

    }
}
