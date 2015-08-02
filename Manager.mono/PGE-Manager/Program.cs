using System;
using Gtk;
using System.Runtime.InteropServices;
using System.IO;
using Microsoft.Win32;
using Newtonsoft.Json;

namespace PGEManager
{
	class MainClass
	{
        public static Settings ProgramSettings = new Settings();

        [DllImport ("libx11.so.6")] //necessary for init threads
        static extern int XInitThreads();
		public static void Main (string[] args)
		{
            if (Internals.CurrentOS == InternalOperatingSystem.Linux)
            {
                MessageDialog md = new MessageDialog(null, DialogFlags.Modal, MessageType.Error, ButtonsType.Ok, 
                    "Linux support is coming soon!");
            }
            if (Internals.CurrentOS == InternalOperatingSystem.Linux)
                XInitThreads();

            ProgramSettings.ForcePortable = IsPortable();

            Application.Init();
            if (File.Exists(ProgramSettings.ConfigDirectory + System.IO.Path.DirectorySeparatorChar + "Settings.json"))
            {
                //TODO: load settings
                MainWindow win = new MainWindow ();
                win.Show ();
            }
            else
            {
                if (args.Length > 1)
                {
                    //--install "C:\aoisdf"
                    if (args[0] == "--install")
                        ProgramSettings.PGEDirectory = args[1].Trim('"');
                }
                else
                {
                    if (Internals.CurrentOS == InternalOperatingSystem.Windows)
                    {
                        try
                        {
                            RegistryKey rk = Registry.CurrentUser.OpenSubKey("Software\\Wohlhabend Team\\PGE Project");
                            Console.WriteLine("PGE Location: " + rk.GetValue("InstallLocation").ToString());
                        }
                        catch
                        {
                        }
                    }
                    else
                    {
                        //TODO: Initial setup
                    }
                }
            }

			Application.Run ();
		}

        public static bool SaveSettings()
        {
            JsonSerializer js = new JsonSerializer();
            using (StreamWriter sw = new StreamWriter(ProgramSettings.ConfigDirectory + System.IO.Path.DirectorySeparatorChar + "Settings.json"))
            {
                using (JsonWriter jsw = new JsonTextWriter(sw))
                {
                    js.Serialize(jsw, ProgramSettings);
                    return true;
                }
            }
            return false;
        }

        private bool LoadSettings()
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
