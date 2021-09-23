using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Linq;
using System.ServiceProcess;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Windows.Forms;
using Timer = System.Timers.Timer;
using System.Timers;
using System.Security.Cryptography.X509Certificates;

using Google.Apis;
using Google.Apis.Auth.OAuth2;
using Google.Apis.Services;
using Google.Apis.Pubsub.v1;
using Google.Apis.Pubsub.v1.Data;
using System.IO;

namespace GPM_TokenServer
{
    public partial class TokenServer : ServiceBase
    {
        Thread thread;
        public static String access_token = "no valid token received yet.";

        public TokenServer(string[] args)
        {
            InitializeComponent();

            eventLog = new System.Diagnostics.EventLog();
            if (!System.Diagnostics.EventLog.SourceExists("TokenServerSvc"))
            {
                System.Diagnostics.EventLog.CreateEventSource(
                    "TokenServerSvc", "TokenServerLog");
            }
            eventLog.Source = "TokenServerSvc";
            eventLog.Log = "TokenServerLog";


            // https://docs.microsoft.com/en-us/dotnet/api/system.serviceprocess.servicebase.onstart?view=dotnet-plat-ext-5.0
            // string[] imagePathArgs = Environment.GetCommandLineArgs();
            // (HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\<service name>)
            foreach (string a in args)
                eventLog.WriteEntry("Token Service Argument: " + a, EventLogEntryType.Information, 10);
        }

        protected override void OnStart(string[] args)
        {
            foreach (string a in args)
                eventLog.WriteEntry("Token Service Argument: " + a, EventLogEntryType.Information,10);

            // Set up a timer that triggers every minute.
            Timer timer = new Timer
            {
                Interval = 30 * 60000 // 30 minutes
            };
            timer.Elapsed += new ElapsedEventHandler(this.OnTimer);
            timer.Start();

            string output = "";
            var psi = new ProcessStartInfo
            {
                CreateNoWindow = true, //This hides the dos-style black window that the command prompt usually shows
                FileName = @"C:\Program Files (x86)\Google\Cloud SDK\google-cloud-sdk\bin\gcloud.cmd",
                Arguments = "auth activate-service-account curl-publisher@pm-devices.iam.gserviceaccount.com --key-file=\"C:\\Projects\\GPS\\UBlox\\GPM_Monitor\\GPM_Vibration\\pm-devices-20d93a2a3f55.json\"",
                RedirectStandardOutput = true,
                UseShellExecute = false
            };
            try
            {

                var process = new Process
                {
                    StartInfo = psi
                };
                process.Start();
                StreamReader reader = process.StandardOutput;
                output = reader.ReadToEnd();
                process.WaitForExit();
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex.Message);
            }
            eventLog.WriteEntry("Activated Service Account", EventLogEntryType.Information, 2);
            //Debug.WriteLine(output);
            
            thread = new Thread(new ThreadStart(tokenServerThread));
            thread.Start();

            Thread.Sleep(500);
            OnTimer(null, null);

            eventLog.WriteEntry("Token Service Started.", EventLogEntryType.Information);
        }

        protected override void OnStop()
        {
            thread.Abort();  // causes an Exception in the thread
            AsynchronousSocketListener.lstDone = true;
            AsynchronousSocketListener.allDone.Set();
            System.Threading.Thread.Sleep(1000);
            eventLog.WriteEntry("Token Service Stopped.");
        }

        
        public void OnTimer(object sender, ElapsedEventArgs args)
        {
            string output = "";
            var psi = new ProcessStartInfo
            {
                CreateNoWindow = true, //This hides the dos-style black window that the command prompt usually shows
                FileName = @"C:\Program Files (x86)\Google\Cloud SDK\google-cloud-sdk\bin\gcloud.cmd",
                Arguments = "auth print-access-token",
                RedirectStandardOutput = true,
                UseShellExecute = false
            };
            try
            {
                var process = new Process
                {
                    StartInfo = psi
                };
                process.Start();
                StreamReader reader = process.StandardOutput;
                output = reader.ReadToEnd();
                process.WaitForExit();
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex.Message);
            }
            if (output.Length > 10)
            {
                eventLog.WriteEntry("Acquired new access token: " + output.Substring(0, 10), EventLogEntryType.Information, 2); ;
                access_token = output.Substring(0,output.Length-2);
            }
        }

       

        public void tokenServerThread()
        {
            Thread.Sleep(10000); // 10 sec
            AsynchronousSocketListener.StartListening();
        }


    }
    // State object for reading client data asynchronously  
    public class StateObject
    {
        // Size of receive buffer.  
        public const int BufferSize = 1024;

        // Receive buffer.  
        public byte[] buffer = new byte[BufferSize];

        // Received data string.
        public StringBuilder sb = new StringBuilder();

        // Client socket.
        public Socket workSocket = null;

        public EventLog log = null;

        public bool TokenRequest = false;
    }


    public class AsynchronousSocketListener
    {
        public static EventLog log;
        public static bool lstDone = false;
        // Thread signal.  
        public static ManualResetEvent allDone = new ManualResetEvent(false);

        public AsynchronousSocketListener(EventLog svclog)
        {
        }

        public static void StartListening()
        {
            //IPHostEntry ipHostInfo = Dns.GetHostEntry(Dns.GetHostName());
            //IPAddress ipAddress = ipHostInfo.AddressList[5];

            IPAddress ipAddress = IPAddress.Parse("192.168.10.120");
            IPEndPoint localEndPoint = new IPEndPoint(ipAddress, 11000);

            log = new System.Diagnostics.EventLog();
            if (!System.Diagnostics.EventLog.SourceExists("TokenServerListener"))
            {
                System.Diagnostics.EventLog.CreateEventSource(
                    "TokenServerListener", "TokenServerLog");
            }
            log.Source = "TokenServerListener";
            log.Log = "TokenServerLog";

            // Create a TCP/IP socket.  
            Socket listener = new Socket(ipAddress.AddressFamily,
                SocketType.Stream, ProtocolType.Tcp);

            log.WriteEntry("Server created on port 11000.",EventLogEntryType.Information,1);

            // Bind the socket to the local endpoint and listen for incoming connections.  
            try
            {
                listener.Bind(localEndPoint);
                listener.Listen(100);

                while (!lstDone)
                {
                    // Set the event to nonsignaled state.  
                    allDone.Reset();

                    // Start an asynchronous socket to listen for connections.  
                    //Console.WriteLine("Waiting for a connection...");
                    listener.BeginAccept(
                        new AsyncCallback(AcceptCallback),
                        listener);

                    // Wait until a connection is made before continuing.  
                    allDone.WaitOne();
                }

            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex.Message);
            }

            log.WriteEntry("Server Stopped.", EventLogEntryType.Information,1);

        }

        public static void ClosedCallback(IAsyncResult ar)
        {

        }

        public static void AcceptCallback(IAsyncResult ar)
        {
            // Signal the main thread to continue.  
            allDone.Set();

            // Get the socket that handles the client request.  
            Socket listener = (Socket)ar.AsyncState;
            Socket handler = listener.EndAccept(ar);

            // Create the state object.  
            StateObject state = new StateObject
            {
                workSocket = handler,
                log = new System.Diagnostics.EventLog()
            };
            if (!System.Diagnostics.EventLog.SourceExists("TokenServerClient"))
            {
                System.Diagnostics.EventLog.CreateEventSource(
                    "TokenServerClient", "TokenServerLog");
            }
            state.log.Source = "TokenServerClient";
            state.log.Log = "TokenServerLog"; handler.BeginReceive(state.buffer, 0, StateObject.BufferSize, 0,
                new AsyncCallback(ReadCallback), state);
            state.TokenRequest = true;
            log.WriteEntry("Client Accepted.", EventLogEntryType.Information,3);
        }

        public static void ReadCallback(IAsyncResult ar)
        {
            String content = String.Empty;

            // Retrieve the state object and the handler socket  
            // from the asynchronous state object.  
            StateObject state = (StateObject)ar.AsyncState;
            Socket handler = state.workSocket;

            // Read data from the client socket.
            int bytesRead = handler.EndReceive(ar);

            if (bytesRead > 0)
            {
                state.sb.Clear();
                // There  might be more data, so store the data received so far.  
                state.sb.Append(Encoding.ASCII.GetString(
                    state.buffer, 0, bytesRead));

                // Check for end-of-file tag. If it is not there, read
                // more data.  
                content = state.sb.ToString();
                Console.WriteLine("Read {0} bytes from socket. \n Data : {1}",
                        content.Length, content);

                int iPM = content.IndexOf("PM-");
                if (iPM >= 0)
                {
                    string output = "Failed.";
                    state.TokenRequest = false;
                    int icolon = content.IndexOf(":",iPM) + 1;
                    if (icolon > 0)
                    {
                        string message = content.Substring(icolon, content.Length - icolon);
                        string project = "pm-devices";
                        string topic = "pm_telemetry";
                        string msg = "{'messages': [{'data': '" + System.Convert.ToBase64String(Encoding.ASCII.GetBytes(message)) + "'}]}";

                        var psi = new ProcessStartInfo
                        {
                            CreateNoWindow = true, //This hides the dos-style black window that the command prompt usually shows
                            FileName = @"C:\Program Files\Utils\bin\curl.exe",
                            Arguments = "-H \"content-type: application/json\" -H \"Authorization: Bearer " +
                            TokenServer.access_token + "\" -X POST --data \"" + msg + "\" " +
                            "https://pubsub.googleapis.com/v1/projects/" +
                            project + "/topics/" +
                            topic + ":publish",
                            RedirectStandardOutput = true,
                            UseShellExecute = false
                        };
                        try
                        {
                            var process = new Process
                            {
                                StartInfo = psi
                            };
                            process.Start();
                            StreamReader reader = process.StandardOutput;
                            output = reader.ReadToEnd();
                            process.WaitForExit();
                        }
                        catch (Exception ex)
                        {
                            Debug.WriteLine(ex.Message);
                        }
                        if (output.Length > 10)
                        {
                            //eventLog.WriteEntry("Acquired new access token: " + output.Substring(0, 10), EventLogEntryType.Information, 2); ;
                            Debug.WriteLine(output);
                        }
                    }
                    Send(handler, output);
                }
                else
                {
                    Send(handler, TokenServer.access_token);
                    state.TokenRequest = true;
                }
            }
        }

        private static void Send(Socket handler, String data)
        {
            // Convert the string data to byte data using ASCII encoding.  
            byte[] byteData = Encoding.ASCII.GetBytes(data);

            // Begin sending the data to the remote device.  
            handler.BeginSend(byteData, 0, byteData.Length, 0,
                new AsyncCallback(SendCallback), handler);
        }

        private static void SendCallback(IAsyncResult ar)
        {
            try
            {
                // Retrieve the socket from the state object.  
                Socket handler = (Socket)ar.AsyncState;

                // Complete sending the data to the remote device.  
                int bytesSent = handler.EndSend(ar);
                Console.WriteLine("Sent {0} bytes to client.", bytesSent);

                handler.Shutdown(SocketShutdown.Both);
                handler.Close();

            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }
    }
}
