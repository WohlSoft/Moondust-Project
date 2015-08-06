using System;
using Microsoft.Win32;
using Gtk;
using System.IO;
using System.Diagnostics;

namespace PGEManager
{
    public partial class PrettySetupWindow : Gtk.Window
    {
        public PrettySetupWindow()
            : base(Gtk.WindowType.Toplevel)
        {
            this.Build();
            Setup();
        }

        private void Setup()
        {
            if (Internals.CurrentOS == InternalOperatingSystem.Windows)
            {
                if (GetPGEFromRegistry() != null || GetPGEFromRegistry().Trim() != "")
                    winRegKeyPath.Label = "From Registry: " + GetPGEFromRegistry();
                else
                {
                    winRegKeyPath.Label = "Not available!";
                    winRegKeyPath.Visible = false;
                    winRegKeyPath.Sensitive = false;
                }
            }
            else
            {
                winRegKeyPath.Label = "No registry on Linux!";
                winRegKeyPath.Visible = false;
                winRegKeyPath.Sensitive = false;
            }
        }

        public string GetPGEFromRegistry()
        {
            RegistryKey rk = Registry.CurrentUser.OpenSubKey("Software\\Wohlhabend Team\\PGE Project");
            return rk.GetValue("InstallLocation").ToString();
        }

        protected void OnBrowseButtonClicked (object sender, EventArgs e)
        {
            FileChooserDialog fcd = new FileChooserDialog("Select PGE Directory", null, FileChooserAction.SelectFolder);
            fcd.AddButton(Stock.Cancel, ResponseType.Cancel);
            fcd.AddButton(Stock.Ok, ResponseType.Ok);
            int ret = fcd.Run();
            if (ret == (int)ResponseType.Ok)
            {
                entry1.Text = fcd.Filename;

            }
            fcd.Destroy();
        }

        protected void OnNextButtonClicked (object sender, EventArgs e)
        {
            if (winRegKeyPath.Active)
            {
                if(Directory.Exists(GetPGEFromRegistry()))
                {
                    Program.ProgramSettings.PGEDirectory = GetPGEFromRegistry();
                    Program.SaveSettings();
                    MainWindow mw = new MainWindow();
                    mw.Show();
                    this.Destroy();
                }
            }
            else
            {
                if (entry1.Text.Trim() != "")
                {
                    Program.ProgramSettings.PGEDirectory = entry1.Text;
                    Program.SaveSettings();
                    MainWindow mw = new MainWindow();
                    mw.Show();
                    this.Destroy();
                }
            }
        }

        protected void OnWinRegKeyPathToggled (object sender, EventArgs e)
        {
            if (winRegKeyPath.Active)
            {
                entry1.Sensitive = false;
                browseButton.Sensitive = false;

                winRegKeyPath.Label = String.Format("From Registry: {0}", GetPGEFromRegistry());
                CheckIfPGE(GetPGEFromRegistry());
            }
            else
            {
                entry1.Sensitive = true;
                browseButton.Sensitive = true;
                CheckIfPGE(entry1.Text);
            }
        }

        private bool CheckIfPGE(string path)
        {
            if (Internals.CurrentOS == InternalOperatingSystem.Windows)
                return CheckPGEWin32(path);
            else if (Internals.CurrentOS == InternalOperatingSystem.Linux)
                return CheckPGELinux(path);

            return false;
        }

        private bool CheckPGEWin32(string path)
        {
            string editorPath = path + System.IO.Path.DirectorySeparatorChar + "pge_editor.exe";
            string enginePath = path + System.IO.Path.DirectorySeparatorChar + "pge_engine.exe";
            if (File.Exists(editorPath))
            {
                string tooltip = "";
                tooltip += GetVersionNumberFromProcess(editorPath) + "\n";
                if (File.Exists(enginePath))
                {
                    lblWhereIsPGE.Text = "Found editor and engine! (Mouse over for version info)";
                    tooltip += GetVersionNumberFromProcess(enginePath);
                }
                else
                {
                    lblWhereIsPGE.Text = "Found editor! (Mouse over for version info)";
                }
                lblWhereIsPGE.TooltipText = tooltip;
                return true;
            }
            else
            {
                lblWhereIsPGE.Text = "Editor or engine not found!";
                return false;
            }
        }
        private bool CheckPGELinux(string path)
        {
            string editorPath = path + System.IO.Path.DirectorySeparatorChar + "pge_editor";
            string enginePath = path + System.IO.Path.DirectorySeparatorChar + "pge_engine";
            return false;
        }

        public static string GetVersionNumberFromProcess(string path)
        {
            Process p = new Process();
            p.StartInfo.Arguments = "--version";
            p.StartInfo.CreateNoWindow = true;
            p.StartInfo.FileName = path;
            p.StartInfo.RedirectStandardOutput = true;
            p.StartInfo.UseShellExecute = false;
            p.Start();
            string output = "";
            while (!p.StandardOutput.EndOfStream)
                output += p.StandardOutput.ReadLine() + "\n";

            return output;
        }

        protected void OnEntry1Changed (object sender, EventArgs e)
        {
            CheckIfPGE(entry1.Text);
        }
    }
}

