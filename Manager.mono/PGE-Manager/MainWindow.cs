using System;
using Gtk;
using System.Xml;
using PGEManager.Internal;
using System.Collections.Generic;
using PGEManager;
using System.Net;
using Newtonsoft.Json;
using System.IO;
using System.Diagnostics;
using System.Text.RegularExpressions;
using STA.Settings;

public partial class MainWindow: Gtk.Window
{
    private List<WohlNews> PGENews = new List<WohlNews>();
    private List<ConfigPack> ConfigList = new List<ConfigPack>();
    private string PGEEditorPath, PGEEnginePath;
    private Version EditorVersion, EngineVersion;

    public static bool RunningPortable;

	public MainWindow () : base (Gtk.WindowType.Toplevel)
	{
		this.Build ();
        this.ShowAll();
        ReadConfigsIndex();
        InitializeConfigTreeView();
        GetLatestNews();
        InitializeNewsTreeView();

        RunningPortable = DetectRunningPortable();
        this.Title = "PGE Manager - Portable: " + RunningPortable;
        DoPGEVersioning();
	}

    private bool DetectRunningPortable()
    {
        PGEEditorPath = Environment.CurrentDirectory + System.IO.Path.DirectorySeparatorChar;
        PGEEnginePath = Environment.CurrentDirectory + System.IO.Path.DirectorySeparatorChar;
        if (Internals.CurrentOS == InternalOperatingSystem.Windows)
        {
            PGEEditorPath += "pge_editor.exe";
            PGEEnginePath += "pge_engine.exe";
        }
        else if (Internals.CurrentOS == InternalOperatingSystem.Linux)
        {
            PGEEditorPath += "pge_editor";
            PGEEnginePath += "pge_engine";
        }
        if (File.Exists(PGEEditorPath) && File.Exists(PGEEnginePath))
        {
            if (File.Exists(Environment.CurrentDirectory + System.IO.Path.DirectorySeparatorChar + "pge_editor.ini"))
            {
                INIFile editorConfig = new INIFile(Environment.CurrentDirectory + System.IO.Path.DirectorySeparatorChar + "pge_editor.ini");
                if(editorConfig.GetValue("Main", "force-portable", "false").ToString().ToLower() == "true")
                    return true;
                else
                    return false;
            }
            else
                return false;
        }
        return false;
    }

    private void DoPGEVersioning()
    {
        PGEEditorPath = Program.ProgramSettings.PGEDirectory + System.IO.Path.DirectorySeparatorChar;
        PGEEnginePath = Program.ProgramSettings.PGEDirectory + System.IO.Path.DirectorySeparatorChar;
        if (Internals.CurrentOS == InternalOperatingSystem.Windows)
        {
            PGEEditorPath += "pge_editor.exe";
            PGEEnginePath += "pge_engine.exe";
        }
        else if (Internals.CurrentOS == InternalOperatingSystem.Linux)
        {
            PGEEditorPath += "pge_editor";
            PGEEnginePath += "pge_engine";
        }
        EditorVersion = ExtractVersionFromWohlString(PGEEditorPath);
        EngineVersion = ExtractVersionFromWohlString(PGEEnginePath);

        launcheditorwidget1.SetEditorVersion(EditorVersion);
        launcheditorwidget1.SetEngineVersion(EngineVersion);
    }


    private Version ExtractVersionFromWohlString(string pathToExe)
    {
        Process p = new Process();
        p.StartInfo.Arguments = "--version";
        p.StartInfo.RedirectStandardOutput = true;
        p.StartInfo.UseShellExecute = false;
        p.StartInfo.FileName = pathToExe;
        p.Start();
        string output = "";
        while (!p.StandardOutput.EndOfStream)
            output += p.StandardOutput.ReadLine();

        if (output == null || output.Trim() == "")
            return new Version("0.0.0.0");
        else
        {
            Regex pattern = new Regex("\\d+(\\.\\d+)+");
            Match m = pattern.Match(output);
            return new Version(m.Value);
        }
    }

    private void GetLatestNews()
    {
        using (WebClient wc = new WebClient()) //for testing purposes
        {
            string jsonText = wc.DownloadString("http://engine.wohlnet.ru/news/dummy.php");
            if (jsonText != null)
                PGENews = JsonConvert.DeserializeObject<List<WohlNews>>(jsonText);

            Console.WriteLine("News is okay!");
        }

    }

    private void InitializeNewsTreeView()
    {
        using (WebClient wc = new WebClient())
        {
            Gtk.ListStore model = new ListStore(typeof(Gdk.Pixbuf), typeof(string), typeof(string));
            pgeNewsTree.AppendColumn("Thumbnail", new CellRendererPixbuf(), "pixbuf", 0);
            pgeNewsTree.AppendColumn("Title", new CellRendererText(), "text", 1);
            pgeNewsTree.AppendColumn("Date", new CellRendererText(), "text", 2);
            for (int i = 0; i < PGENews.Count; i++)
            {
                wc.DownloadFile(PGENews[i].picture, Program.ProgramSettings.ConfigDirectory + System.IO.Path.DirectorySeparatorChar + i + ".png");
                Gdk.Pixbuf pix = new Gdk.Pixbuf(Program.ProgramSettings.ConfigDirectory + System.IO.Path.DirectorySeparatorChar + i + ".png");
                model.AppendValues(pix.ScaleSimple(50, 50, Gdk.InterpType.Bilinear), PGENews[i].title, PGENews[i].posted);
            }

            pgeNewsTree.Model = model;
        }
    }

    private void InitializeConfigTreeView()
    {
        Gtk.ListStore configPackModel = new ListStore(typeof(string), typeof(string), typeof(bool));
        configListTreeview.AppendColumn("Pack Name", new CellRendererText(), "text", 0);
        configListTreeview.AppendColumn("Upload Date", new CellRendererText(), "text", 1);
        configListTreeview.AppendColumn("Installed", new CellRendererText(), "text", 2);
        foreach (var cfg in ConfigList)
        {
            bool exists = false;
            string pathToCheck = Program.ProgramSettings.PGEDirectory + System.IO.Path.DirectorySeparatorChar + cfg.URL.Trim('/');
            if(Directory.Exists(pathToCheck))
            {
                exists = true;
            }
            configPackModel.AppendValues(
                cfg.FriendlyName, 
                ConfigPack.UnixTimeStampToDateTime((double)cfg.upd).ToString(), exists);
        }
        configListTreeview.Model = configPackModel;
    }

	protected void OnDeleteEvent (object sender, DeleteEventArgs a)
	{
        Program.SaveSettings();
		Application.Quit ();
		a.RetVal = true;

        Environment.Exit(0);
	}

    private void ReadConfigsIndex()
    {
        XmlTextReader reader = new XmlTextReader(Program.ProgramSettings.ConfigsIndexURL);

        ConfigPack temp = new ConfigPack();
        while (reader.Read())
        {
            if (temp.FriendlyName != null && temp.URL != null && temp.upd != 0)
                temp = new ConfigPack();
            switch (reader.NodeType)
            {
                case XmlNodeType.Element:
                    if (reader.Name == "cpack-index-sheet")
                        continue;
                    //Console.Write("<{0}", reader.Name);
                    while (reader.MoveToNextAttribute())
                    {
                        //Console.Write(" {0}='{1}'", reader.Name, reader.Value);
                        switch (reader.Name)
                        {
                            case "name":
                                temp.FriendlyName = reader.Value;
                                break;
                            case "upd":
                                temp.upd = int.Parse(reader.Value);
                                break;
                        }

                    }
                    //Console.Write(">");
                    break;
                case XmlNodeType.Text:
                    //Console.WriteLine(reader.Value);
                    temp.URL = reader.Value;
                    if (temp.URL != null && temp.FriendlyName != null)
                        ConfigList.Add(temp);
                    break;
                case XmlNodeType.EndElement:
                    //Console.WriteLine("</{0}>", reader.Name);
                    if (temp.URL != null && temp.FriendlyName != null)
                        ConfigList.Add(temp);
                    break;
            }

        }
    }
    protected void OnExitActionActivated (object sender, EventArgs e)
    {
        Program.SaveSettings();
        Application.Quit ();

        Environment.Exit(0);
    }
}
