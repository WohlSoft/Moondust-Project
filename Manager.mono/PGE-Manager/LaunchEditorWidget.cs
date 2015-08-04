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

        protected void OnLaunchEditorBtnClicked (object sender, EventArgs e)
        {
            switch (Internals.CurrentOS)
            {
                case(InternalOperatingSystem.Windows):
                    if(File.Exists(Program.ProgramSettings.PGEDirectory + System.IO.Path.DirectorySeparatorChar + "pge_editor.exe"))
                        Process.Start(Program.ProgramSettings.PGEDirectory + System.IO.Path.DirectorySeparatorChar + "pge_editor.exe");
                    break;
                case(InternalOperatingSystem.Linux):
                    if (File.Exists(Program.ProgramSettings.PGEDirectory + System.IO.Path.DirectorySeparatorChar + "pge_editor"))
                        Process.Start(Program.ProgramSettings.PGEDirectory + System.IO.Path.DirectorySeparatorChar + "pge_editor");
                    break;
            }
        }

        protected void OnLaunchEngineBtnClicked (object sender, EventArgs e)
        {
            switch (Internals.CurrentOS)
            {
                case(InternalOperatingSystem.Windows):
                    if(File.Exists(Program.ProgramSettings.PGEDirectory + System.IO.Path.DirectorySeparatorChar + "pge_engine.exe"))
                        Process.Start(Program.ProgramSettings.PGEDirectory + System.IO.Path.DirectorySeparatorChar + "pge_engine.exe");
                    break;
                case(InternalOperatingSystem.Linux):
                    if (File.Exists(Program.ProgramSettings.PGEDirectory + System.IO.Path.DirectorySeparatorChar + "pge_engine"))
                        Process.Start(Program.ProgramSettings.PGEDirectory + System.IO.Path.DirectorySeparatorChar + "pge_engine");
                    break;
            }
        }
    }
}

