using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net.Sockets;
using System.IO;
using System.Threading;
using System.IO.Ports;

namespace TCPListener
{
    class Program
    {
        static void Main(string[] args)
        {
            
            Console.WriteLine("Select Serial Port");
            int count = 0;
            foreach (string s in SerialPort.GetPortNames())
            {
                Console.WriteLine($"{count}  {s}");
                count++;
            }

            SerialPort serialPort = new SerialPort(SerialPort.GetPortNames()[Convert.ToInt32(Console.ReadLine())], 9600);
            
            TcpListener listener = new TcpListener(1302);
            listener.Start();

            while (true)
            {
                Console.WriteLine("Waiting...");
                TcpClient client = listener.AcceptTcpClient();
                StreamReader sr = new StreamReader(client.GetStream());
                StreamWriter sw = new StreamWriter(client.GetStream());
                try
                {
                    string request = sr.ReadLine();
                    Console.WriteLine(request);
                    Console.WriteLine(request.Split('/')[1].Split(' ')[0]);
                    sw.WriteLine("HTTP/1.0 202 OK\n");
                    serialPort.WriteLine(request.Split('/')[1].Split(' ')[0]);
                    sw.Flush();
                }
                catch
                {

                }

            }

        }


    }
}
