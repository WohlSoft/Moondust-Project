using System;
using Gtk;
using System.Xml;
using PGEManager.Internal;
using System.Collections.Generic;
using PGEManager;
using System.Net;
using Newtonsoft.Json;
using System.IO;

public partial class MainWindow: Gtk.Window
{
    private List<WohlNews> PGENews = new List<WohlNews>();
    private List<ConfigPack> ConfigList = new List<ConfigPack>();

	public MainWindow () : base (Gtk.WindowType.Toplevel)
	{
		this.Build ();
        ReadConfigsIndex();
        InitializeConfigTreeView();
        GetLatestNews();
        InitializeNewsTreeView();
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
        Gtk.ListStore configPackModel = new ListStore(typeof(string), typeof(string), typeof(string));
        configListTreeview.AppendColumn("Pack Name", new CellRendererText(), "text", 0);
        configListTreeview.AppendColumn("Upload Date", new CellRendererText(), "text", 1);
        configListTreeview.AppendColumn("Installed", new CellRendererText(), "text", 2);
        foreach (var cfg in ConfigList)
        {
            configPackModel.AppendValues(
                cfg.FriendlyName, 
                ConfigPack.UnixTimeStampToDateTime((double)cfg.upd).ToString(), "No");
        }
        configListTreeview.Model = configPackModel;
    }

	protected void OnDeleteEvent (object sender, DeleteEventArgs a)
	{
        Program.SaveSettings();
		Application.Quit ();
		a.RetVal = true;
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
}
