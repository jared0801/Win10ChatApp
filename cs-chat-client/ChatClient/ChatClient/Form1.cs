using System;
using System.Text;
using System.Net.Sockets;
using System.Windows.Forms;

namespace WindowsFormsApp1
{
    public partial class Form1 : Form
    {
        TcpClient _client;
        byte[] _buffer = new byte[4096];

        public Form1()
        {
            InitializeComponent();

            _client = new TcpClient();
        }

        protected override void OnShown(EventArgs e)
        {
            base.OnShown(e);

            _client.Connect("127.0.0.1", 54000);

            _client.GetStream().BeginRead(_buffer, 0, _buffer.Length, Server_MessageReceived, null);
        }

        private void Server_MessageReceived(IAsyncResult ar)
        {
            if(ar.IsCompleted)
            {
                // TODO: Receive message
                var bytesIn = _client.GetStream().EndRead(ar);
                if(bytesIn > 0)
                {
                    var tmp = new byte[bytesIn];
                    Array.Copy(_buffer, 0, tmp, 0, bytesIn);
                    var str = Encoding.ASCII.GetString(tmp);
                    // TODO: Make this a delegate
                    BeginInvoke((Action)(() => {
                        listBox1.Items.Add(str);
                        listBox1.SelectedIndex = listBox1.Items.Count - 1; // 'Scroll' to the last item
                    }));
                }

                Array.Clear(_buffer, 0, _buffer.Length);
                _client.GetStream().BeginRead(_buffer, 0, _buffer.Length, Server_MessageReceived, null);

            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            var msg = Encoding.ASCII.GetBytes(textBox1.Text);
            _client.GetStream().Write(msg, 0, msg.Length);

            textBox1.Text = "";
            textBox1.Focus();
        }
    }
}
