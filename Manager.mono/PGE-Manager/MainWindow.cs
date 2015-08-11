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
using System.Threading;
using System.Linq;

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

        Thread t = new System.Threading.Thread(CheckUpdatesCombined);
        t.Start();
	}

    private void CheckUpdatesCombined()
    {
        bool EditorUpdateAvail, EngineUpdateAvail;
        EditorUpdateAvail = CheckForEditorUpdates();
        EngineUpdateAvail = CheckForEngineUpdates();

        string message = "The following components have updates:\n";
        message += (EditorUpdateAvail) ? "-Editor\n" : "";
        message += (EngineUpdateAvail) ? "-Engine\n" : "";
        message += "\nPlease update them!";
        MessageDialog md = new MessageDialog(null, DialogFlags.Modal, MessageType.Info, ButtonsType.Ok, message);
        if (EditorUpdateAvail | EngineUpdateAvail)
            md.Run();
        md.Destroy();
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

    private Version GetVersionFromWohlString(string text)
    {
        Regex pattern = new Regex("\\d+(\\.\\d+)+");
        Match m = pattern.Match(text);
        return new Version(m.Value);
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

    private bool CheckForEditorUpdates()
    {
        try
        {
            using(WebClient wc = new WebClient())
            {
                string versTextFile = wc.DownloadString("http://download.gna.org/pgewohlstand/dev/win32/online-install/editor.txt");
                if(versTextFile == null || versTextFile.Trim() == "")
                    return false;
                Version LatestVersion = GetVersionFromWohlString(versTextFile);
                if(LatestVersion > EditorVersion)
                    return true;
            }
        }
        catch(Exception ex)
        {
            Console.WriteLine("ERROR: {0}", ex.Message);
        }
        return false;
    }

    private bool CheckForEngineUpdates()
    {
        try
        {
            using(WebClient wc = new WebClient())
            {
                string versTextFile = wc.DownloadString("http://download.gna.org/pgewohlstand/dev/win32/online-install/engine.txt");
                if(versTextFile == null || versTextFile.Trim() == "")
                    return false;
                Version LatestVersion = GetVersionFromWohlString(versTextFile);
                if(LatestVersion > EngineVersion)
                    return true;
            }
        }
        catch(Exception ex)
        {
            Console.WriteLine("ERROR: {0}", ex.Message);
        }
        return false;
    }

    private void InitializeConfigTreeView()
    {
        List<ConfigPack> PacksThatNeedUpdating = new List<ConfigPack>();
        Gtk.ListStore configPackModel = new ListStore(typeof(string), typeof(string), typeof(string));
        configListTreeview.AppendColumn("Pack Name", new CellRendererText(), "text", 0);
        configListTreeview.AppendColumn("Upload Date", new CellRendererText(), "text", 1);
        configListTreeview.AppendColumn("Installed", new CellRendererText(), "text", 2);
        foreach (var cfg in ConfigList)
        {
            bool exists = false;
            string pathToCheck = Program.ProgramSettings.PGEDirectory + System.IO.Path.DirectorySeparatorChar + "configs" + System.IO.Path.DirectorySeparatorChar + cfg.URL.Trim('/');
            long cfgPackVersion = 0;
            if (cfg.URL.ToLower() == "smbx13/")
            {
                if (Directory.Exists(Program.ProgramSettings.PGEDirectory + System.IO.Path.DirectorySeparatorChar + "configs" + System.IO.Path.DirectorySeparatorChar + "SMBX"))
                    exists = true;
                cfgPackVersion = GetPackVersion(cfg);
            }
            else if (cfg.URL == "SMBX_Redrawn/")
            {
                if (Directory.Exists(Program.ProgramSettings.PGEDirectory + System.IO.Path.DirectorySeparatorChar + "configs" + System.IO.Path.DirectorySeparatorChar + "SMBX_Redrawn"))
                    exists = true;
                cfgPackVersion = GetPackVersion(cfg);
            }
            else if (cfg.URL == "A2MBXT/")
            {
                if (Directory.Exists(Program.ProgramSettings.PGEDirectory + System.IO.Path.DirectorySeparatorChar + "configs" + System.IO.Path.DirectorySeparatorChar + "Raocow (A2MBXT)"))
                    exists = true;
                cfgPackVersion = GetPackVersion(cfg);
            }
            else if(Directory.Exists(pathToCheck))
            {
                exists = true;
                cfgPackVersion = GetPackVersion(cfg);
            }
            if(exists)
                if (cfg.upd > cfgPackVersion)
                    PacksThatNeedUpdating.Add(cfg);
            if(exists)
                configPackModel.AppendValues(
                    cfg.FriendlyName, 
                    ConfigPack.UnixTimeStampToDateTime((double)cfg.upd).ToString(), exists + " (" + ConfigPack.UnixTimeStampToDateTime((double)cfgPackVersion) + ")");
            else
                configPackModel.AppendValues(
                    cfg.FriendlyName, 
                    ConfigPack.UnixTimeStampToDateTime((double)cfg.upd).ToString(), exists.ToString());
        }
        configListTreeview.Model = configPackModel;
        if (PacksThatNeedUpdating.Count > 0)
            NotifyUserOfUpdates(PacksThatNeedUpdating);
    }

    private void NotifyUserOfUpdates(List<ConfigPack> list)
    {
        string message = "The following config packs are out of date:\n";
        foreach (var cfg in list)
            message += string.Format("-{0}\n", cfg.FriendlyName);
        message += "\nPlease update them!";
        MessageDialog md = new MessageDialog(null, DialogFlags.Modal, MessageType.Info, ButtonsType.Ok, message);
        md.Run();
        md.Destroy();
    }

    private void UpdateConfigPacks(List<ConfigPack> list)
    {
        string tempDir = Program.ProgramSettings.ConfigDirectory + System.IO.Path.DirectorySeparatorChar + "PGE Manager" + System.IO.Path.DirectorySeparatorChar + "temp";
        if (!Directory.Exists(tempDir))
            Directory.CreateDirectory(tempDir);

        foreach (var configPack in list)
        {
            ConfigPackInformation cpi = ReadPackInformation(configPack);
            //TODO: download pack
            //TODO: extract pack
            //TODO: write current pack verison to settings
            //TODO: coffee
        }

    }

    private long GetPackVersion(ConfigPack cfg)
    {
        foreach (var installed in Program.ProgramSettings.InstalledConfigs)
        {
            if (cfg.URL.Trim('/') == installed.Key)
                return installed.Value;
        }
        return 0L;
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

    #region Hackjob: The Game
    private static ConfigPackInformation ReadPackInformation(ConfigPack selected)
    {
        WebClient wc = new WebClient();
        string fullXmlText = wc.DownloadString(Program.ProgramSettings.ConfigsRepoURL + selected.URL + "pge_cpack.xml");
        XmlDocument doc = new XmlDocument();
        doc.LoadXml(fullXmlText);

        ConfigPackInformation cpi = new ConfigPackInformation();


        foreach (XmlNode node in doc.DocumentElement.ChildNodes)
        {
            if(cpi.PackName == null)
                cpi.PackName = node.ParentNode.Attributes["name"].Value;
            switch (node.Name)
            {
                case("head"):
                    cpi = IterateThroughHead(node, cpi);
                    break;
                case("files"):
                    if (node.HasChildNodes)
                    {
                        foreach (XmlNode fileNode in node.ChildNodes)
                        {
                            if (fileNode.Name == "file")
                            {
                                FilesStruct temp = new FilesStruct();
                                //temp.Platform = (fileNode.Attributes["platform"].Value);
                                temp.Folder = (fileNode.Attributes["folder"].Value == null) ? "" : fileNode.Attributes["folder"].Value;
                                temp.URL = fileNode.InnerText;

                                if (temp.URL != null && temp.Folder != null)
                                    cpi.FilesParts.Add(temp);
                            }
                        }
                    }
                    break;
            }
        }
        if (cpi.SplashURL != null && cpi.SplashURL != "")
            cpi.SplashURL = Program.ProgramSettings.ConfigsRepoURL + selected.URL + cpi.SplashURL;
        if (cpi.IconURL != null && cpi.IconURL != "")
            cpi.IconURL = Program.ProgramSettings.ConfigsRepoURL + selected.URL + cpi.IconURL;
        return cpi;
    }

    private static ConfigPackInformation IterateThroughHead(XmlNode node, ConfigPackInformation cpi)
    {
        if (node.HasChildNodes)
        {
            foreach (XmlNode child in node.ChildNodes)
            {
                switch (child.Name)
                {
                    case("config"):
                        cpi.PackName = child.Attributes["name"].Value;
                        break;
                    case ("description"):
                        cpi.Description = child.InnerText;
                        break;
                    case("icon"):
                        cpi.IconURL = child.Attributes["img"].Value;
                        break;
                    case("splash"):
                        cpi.SplashURL = child.Attributes["img"].Value;;
                        break;
                    case("smbx64"):
                        if (child.Attributes["true"] != null)
                            cpi.IsSMBX64 = (child.Attributes["true"].Value == "1") ? true : false;
                        break;
                    case("license"):
                        cpi.License = child.InnerText;
                        break;
                    case("credits"):
                        if (child.HasChildNodes)
                        {
                            foreach (XmlNode partNode in child.ChildNodes)
                            {
                                if (partNode.Name == "part")
                                {
                                    string key = "";
                                    List<AuthorStruct> values = new List<AuthorStruct>();

                                    //XmlNode partNode = node.ChildNodes;
                                    key = (partNode.Attributes["name"].Value == null) ? "" : partNode.Attributes["name"].Value;
                                    if (partNode.HasChildNodes)
                                    {
                                        foreach (XmlNode childAuthor in partNode.ChildNodes)
                                        {
                                            if (childAuthor.Name == "author")
                                            {
                                                AuthorStruct temp = new AuthorStruct();
                                                temp.Author = childAuthor.InnerText;
                                                temp.Comment = (childAuthor.Attributes["comment"] == null) ? "" : childAuthor.Attributes["comment"].Value;
                                                temp.Website = (childAuthor.Attributes["url"] == null) ? "" : childAuthor.Attributes["url"].Value;
                                                temp.Email = (childAuthor.Attributes["email"] == null) ? "" : childAuthor.Attributes["email"].Value;
                                                values.Add(temp);
                                            }
                                        }
                                    }
                                    if (values.Count > 0 && key != "")
                                    {
                                        KeyValuePair<string, AuthorStruct[]> k = new KeyValuePair<string, AuthorStruct[]>(key, values.ToArray());
                                        cpi.CreditsParts.Add(k);
                                    }
                                }
                            }
                        }
                        break;
                }
            }
        }
        return cpi;
    }
    #endregion



    protected void OnExitActionActivated (object sender, EventArgs e)
    {
        Program.SaveSettings();
        Application.Quit ();

        Environment.Exit(0);
    }

    [GLib.ConnectBeforeAttribute]
    protected void OnConfigListTreeviewButtonPressEvent (object o, ButtonPressEventArgs args)
    {
        if (args.Event.Button == 3)
        {
            Menu m = new Menu();
            MenuItem ViewInfo = new MenuItem("View Config Pack Information");
            ViewInfo.ButtonPressEvent += (sender, argss) => 
                {
                    TreeSelection selection = (o as TreeView).Selection;
                    TreeModel model;
                    TreeIter iter;
                    if(selection.GetSelected(out model, out iter))
                    {
                        var match = ConfigList.FirstOrDefault(x => x.FriendlyName.Equals(model.GetValue(iter, 0).ToString()));
                        ViewPackInfoWindow vpi = new ViewPackInfoWindow(ReadPackInformation(match));
                        vpi.Show();
                    }
                };
            m.Add(ViewInfo);
            m.ShowAll();
            m.Popup();
        }
    }
}
