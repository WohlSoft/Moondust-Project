using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;

namespace PGEManager
{
    public class Internals
    {
        public static InternalOperatingSystem CurrentOS {get;set;}
    }

    public enum InternalOperatingSystem
    {
        Windows, Linux, MacOSX
    }

    public class Settings
    {
        public string PGEDirectory { get; set;}
        public string ConfigDirectory {get;set;}
        public string ConfigsRepoURL { get; set; }
        public string ConfigsIndexURL {get{return ConfigsRepoURL + "configs.index";}}
        public List<KeyValuePair<string, long>> InstalledConfigs = new List<KeyValuePair<string, long>>();

        public bool ForcePortable {get;set;}

        public Settings()
        {
            DetectOperatingSystems();

            if (!System.IO.Directory.Exists(ConfigDirectory))
                System.IO.Directory.CreateDirectory(ConfigDirectory);

            ForcePortable = false;
            PGEDirectory = "/";
            ConfigsRepoURL = "http://download.gna.org/pgewohlstand/configs/";
        }

        private void DetectOperatingSystems()
        {
            OperatingSystem os = Environment.OSVersion;
            PlatformID pid = os.Platform;

            switch (pid)
            {
                case(PlatformID.MacOSX):
                    ConfigDirectory = Environment.GetFolderPath(Environment.SpecialFolder.Personal) + "/Library/Application Support/PGE Manager\t";
                    Internals.CurrentOS = InternalOperatingSystem.MacOSX;
                    break;
                case(PlatformID.Unix):
                    if (IsRunningOnMac())
                    {
                        ConfigDirectory = Environment.GetFolderPath(Environment.SpecialFolder.Personal) + "/Library/Application Support/PGE Manager\t";
                        Internals.CurrentOS = InternalOperatingSystem.MacOSX;
                    }
                    else
                    {
                        ConfigDirectory = Environment.GetFolderPath(Environment.SpecialFolder.Personal) + "/.pgemanager";
                        Internals.CurrentOS = InternalOperatingSystem.Linux;
                    }
                    break;
                case(PlatformID.Win32NT):
                    ConfigDirectory = Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData) + "\\PGE Manager";
                    Internals.CurrentOS = InternalOperatingSystem.Windows;
                    break;
                default:
                    Console.WriteLine("Invalid OS: {0}", pid.ToString());
                    Environment.Exit(-1);
                    break;
            }
        }

        [DllImport ("libc")]
        static extern int uname (IntPtr buf);
        static bool IsRunningOnMac ()
        {
            IntPtr buf = IntPtr.Zero;
            try 
            {
                buf = Marshal.AllocHGlobal (8192);
                // This is a hacktastic way of getting sysname from uname ()
                if (uname (buf) == 0) 
                {
                    string os = Marshal.PtrToStringAnsi (buf);
                    if (os == "Darwin")
                        return true;
                }
            } 
            catch {} 
            finally 
            {
                if (buf != IntPtr.Zero)
                    Marshal.FreeHGlobal (buf);
            }
            return false;
        }
    }
}

