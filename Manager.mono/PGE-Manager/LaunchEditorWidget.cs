using System;
using System.IO;
using System.Diagnostics;

namespace PGEManager
{
    [System.ComponentModel.ToolboxItem(true)]
    public partial class LaunchEditorWidget : Gtk.Bin
    {
        public LaunchEditorWidget()
        {
            this.Build();
        }

        public void SetEditorVersion(Version version)
        {
            editorVerLbl.Text = "PGE Editor Version: " + version.ToString();
        }

        public void SetEngineVersion(Version version)
        {
            engineVerLbl.Text = "PGE Editor Version: " + version.ToString();
        }

        protected void OnLaunchEditorBtnClicked (object sender, EventArgs e)
        {
            Process p = new Process();
            p.EnableRaisingEvents = true;

            switch (Internals.CurrentOS)
            {
                case(InternalOperatingSystem.Windows):
                    if(File.Exists(Program.ProgramSettings.PGEDirectory + System.IO.Path.DirectorySeparatorChar + "pge_editor.exe"))
                        p.StartInfo.FileName = (Program.ProgramSettings.PGEDirectory + System.IO.Path.DirectorySeparatorChar + "pge_editor.exe");
                    break;
                case(InternalOperatingSystem.Linux):
                    if (File.Exists(Program.ProgramSettings.PGEDirectory + System.IO.Path.DirectorySeparatorChar + "pge_editor"))
                        p.StartInfo.FileName = (Program.ProgramSettings.PGEDirectory + System.IO.Path.DirectorySeparatorChar + "pge_editor");
                    break;
            }

            p.Exited += (object senderr, EventArgs ee) => 
                {
                    launchEditorBtn.Sensitive = true;
                    launchEngineBtn.Sensitive = true;
                };
            if (p.StartInfo.FileName != null || p.StartInfo.FileName.Trim() != "")
            {
                p.Start();
                launchEditorBtn.Sensitive = false;
                launchEngineBtn.Sensitive = false;
            }

        }

        protected void OnLaunchEngineBtnClicked (object sender, EventArgs e)
        {
            Process p = new Process();
            p.EnableRaisingEvents = true;
            switch (Internals.CurrentOS)
            {
                case(InternalOperatingSystem.Windows):
                    if(File.Exists(Program.ProgramSettings.PGEDirectory + System.IO.Path.DirectorySeparatorChar + "pge_engine.exe"))
                        p.StartInfo.FileName = Program.ProgramSettings.PGEDirectory + System.IO.Path.DirectorySeparatorChar + "pge_engine.exe";
                    break;
                case(InternalOperatingSystem.Linux):
                    if (File.Exists(Program.ProgramSettings.PGEDirectory + System.IO.Path.DirectorySeparatorChar + "pge_engine"))
                        p.StartInfo.FileName = Program.ProgramSettings.PGEDirectory + System.IO.Path.DirectorySeparatorChar + "pge_engine";
                    break;
            }

            p.Exited += (object senderr, EventArgs ee) => 
                {
                    launchEditorBtn.Sensitive = true;
                    launchEngineBtn.Sensitive = true;
                };
            if (p.StartInfo.FileName != null || p.StartInfo.FileName.Trim() != "")
            {
                p.Start();
                launchEditorBtn.Sensitive = false;
                launchEngineBtn.Sensitive = false;
            }
        }
    }
}

