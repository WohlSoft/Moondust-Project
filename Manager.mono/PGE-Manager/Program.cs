using System;
using Gtk;
using System.Runtime.InteropServices;
using System.IO;
using Microsoft.Win32;
using Newtonsoft.Json;

namespace PGEManager
{
	public class Program
	{
        public static Settings ProgramSettings = new Settings();

        [DllImport ("libX11.so.6")] //necessary for init threads
        static extern int XInitThreads();
		public static void Main (string[] args)
		{
            Application.Init();
            if (Internals.CurrentOS == InternalOperatingSystem.Linux)
            {
                MessageDialog md = new MessageDialog(null, DialogFlags.Modal, MessageType.Error, ButtonsType.Ok, 
                    "Linux support is coming soon!");
                //md.Run();
                //md.Destroy();
                //Environment.Exit(0);
            }
            try{
            if (Internals.CurrentOS == InternalOperatingSystem.Linux)
                XInitThreads();
            }
            catch{
            }
            

            ProgramSettings.ForcePortable = IsPortable();

            if (File.Exists(ProgramSettings.ConfigDirectory + System.IO.Path.DirectorySeparatorChar + "Settings.json"))
            {
                LoadSettings();
                MainWindow win = new MainWindow ();
                win.Show ();
            }
            else
            {
                if (args.Length > 1)
                {
                    //--install "C:\aoisdf"
                    if (args[0] == "--install")
                    {
                        ProgramSettings.PGEDirectory = args[1].Trim('"');
                        Console.WriteLine(ProgramSettings.PGEDirectory);
                        SaveSettings();
                        MainWindow win = new MainWindow ();
                        win.Show ();
                    }
                }
                else
                {
                    PrettySetupWindow psw = new PrettySetupWindow();
                    if (Internals.CurrentOS == InternalOperatingSystem.Windows)
                    {
                        try
                        {
                            //RegistryKey rk = Registry.CurrentUser.OpenSubKey("Software\\Wohlhabend Team\\PGE Project");
                            //Console.WriteLine("PGE Location: " + rk.GetValue("InstallLocation").ToString());
                            //Console.WriteLine("Show the PGE Manager main window");
                            psw.Show();
                        }
                        catch
                        {
                            psw.Show();
                        }
                    }
                    else
                    {
                        psw.Show();
                    }
                }
            }

			Application.Run ();
		}

        public static bool SaveSettings()
        {
            JsonSerializer js = new JsonSerializer();
            js.Formatting = Formatting.Indented;
            using (StreamWriter sw = new StreamWriter(ProgramSettings.ConfigDirectory + System.IO.Path.DirectorySeparatorChar + "Settings.json"))
            {
                using (JsonWriter jsw = new JsonTextWriter(sw))
                {
                    js.Serialize(jsw, ProgramSettings);
                    return true;
                }
            }

        }

        private static bool LoadSettings()
        {
            if (File.Exists(ProgramSettings.ConfigDirectory + System.IO.Path.DirectorySeparatorChar + "Settings.json"))
            {
                JsonSerializer js = new JsonSerializer();
                js.Formatting = Formatting.Indented;
                using (StreamReader sr = new StreamReader(ProgramSettings.ConfigDirectory + System.IO.Path.DirectorySeparatorChar + "Settings.json"))
                {
                    using (JsonReader jsr = new JsonTextReader(sr))
                    {
                        ProgramSettings = js.Deserialize<PGEManager.Settings>(jsr);
                        return true;
                    }
                }
            }
            return false;
        }

        private static bool IsPortable()
        {
            return false;
        }
	}
}
