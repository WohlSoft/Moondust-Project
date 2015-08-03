using System;
using System.Xml;
using System.Collections.Generic;
using System.IO;
using System.Net;
using Microsoft.Win32;

namespace XmlTester
{
    class MainClass
    {
        private static string CONFIGDIR = "http://download.gna.org/pgewohlstand/configs/";
        private static string CONFIGSINDEX = "http://download.gna.org/pgewohlstand/configs/configs.index";
        static List<ConfigPack> ConfigList = new List<ConfigPack>();

        public static void Main(string[] args)
        {
            try
            {
                RegistryKey rk = Registry.CurrentUser.OpenSubKey("Software\\Wohlhabend Team\\PGE Project");
                Console.WriteLine("PGE Location: " + rk.GetValue("InstallLocation").ToString());
            }
            catch{
            }

            Console.WriteLine("Reading....\n\n");

            TestRead2();

            Console.WriteLine("Please choose the number of the config pack you want to view");
    AskAgain:
            for (int i = 0; i < ConfigList.Count; i++)
            {
                Console.WriteLine("({0}) {1}", i, ConfigList[i].FriendlyName, ConfigList[i].URL, ConfigList[i].upd);
            }

            int selection = int.Parse(Console.ReadLine());

            if (selection > ConfigList.Count)
                goto AskAgain;

            //ConfigPackInformation cpi = TryReadPackInformation(ConfigList[selection]);
            //PrintInformation(cpi);
            ConfigPackInformation cpi = ReadPackInformation(ConfigList[selection]);
            PrintInformation(cpi);

            Console.WriteLine("\nDone :)");
            Console.ReadLine();
        }

        private static void PrintInformation(ConfigPackInformation cp)
        {
            Console.Clear();

            Console.WriteLine(cp.PackName);
            Console.WriteLine(cp.Description);
            Console.WriteLine(cp.License);
            foreach (var thing in cp.CreditsParts)
            {
                Console.WriteLine("---{0}---", thing.Key);
                foreach (var author in thing.Value)
                {
                    string print = "-" + author.Author;

                    if (author.Email != null)
                        print += ", Email: " + author.Email;
                    if (author.Website != null)
                        print += ", Website: " + author.Website;
                    if (author.Comment != null)
                        print += "(" + author.Comment + ")";
                    Console.WriteLine(print);
                }

            }

            Console.WriteLine(cp.IconURL + " & " + cp.SplashURL);
            Console.WriteLine("SMBX64: " + cp.IsSMBX64);

            Console.WriteLine("\n\nDone:)");
            Console.ReadLine();
        }

        private static ConfigPackInformation ReadPackInformation(ConfigPack selected)
        {
            WebClient wc = new WebClient();
            string fullXmlText = wc.DownloadString(CONFIGDIR + selected.URL + "pge_cpack.xml");
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
                cpi.SplashURL = CONFIGDIR + selected.URL + cpi.SplashURL;
            if (cpi.IconURL != null && cpi.IconURL != "")
                cpi.IconURL = CONFIGDIR + selected.URL + cpi.IconURL;
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

        private static ConfigPackInformation TryReadPackInformation(ConfigPack selected)
        {
            XmlTextReader rreader = new XmlTextReader(CONFIGDIR + selected.URL + "pge_cpack.xml");

            string fullXmlText = "";
            while (rreader.Read())
                fullXmlText = rreader.ReadOuterXml();
            string ffullXmlText = fullXmlText.Trim(new char[]{ '\r', '\n', '\t', ' '});


            var reader = XmlReader.Create(new StringReader(ffullXmlText.Trim()));

            ConfigPackInformation PackInformation = new ConfigPackInformation();

            while (reader.Read())
            {
                switch (reader.NodeType)
                {
                    case XmlNodeType.Element:
                        
                        if (reader.Name == "config")
                            PackInformation.PackName = reader.GetAttribute("name");
                        if (reader.Name == "description")
                            PackInformation.Description = reader.Value;
                        if (reader.Name == "icon")
                            PackInformation.IconURL = CONFIGDIR + selected.URL + "/" + reader.GetAttribute("img");
                        if (reader.Name == "splash")
                            PackInformation.SplashURL = CONFIGDIR + selected.URL + "/" + reader.GetAttribute("img");
                        if (reader.Name == "smbx64")
                            PackInformation.IsSMBX64 = (reader.GetAttribute("true") == "1") ? true : false;
                        if (reader.Name == "license")
                            reader.MoveToContent(); PackInformation.License = reader.Value;
                        if (reader.Name == "part")
                        {
                            string key = reader.GetAttribute("name");
                            List<AuthorStruct> values = new List<AuthorStruct>();
                            if (reader.ReadToDescendant("author"))
                            {
                                do
                                {
                                    if (reader.Name == "author")
                                    {
                                        AuthorStruct auth = new AuthorStruct();

                                        if (reader.GetAttribute("email") != null)
                                            auth.Email = reader.GetAttribute("email");
                                        if (reader.GetAttribute("url") != null)
                                            auth.Website = reader.GetAttribute("url");
                                        if (reader.GetAttribute("comment") != null)
                                            auth.Comment = reader.GetAttribute("comment");
                                        reader.MoveToContent(); auth.Author = reader.Value;

                                        values.Add(auth);
                                    }
                                }
                                while(reader.ReadToNextSibling("author"));
                            }


                            PackInformation.CreditsParts.Add(new KeyValuePair<string, AuthorStruct[]>(key, values.ToArray()));
                        }
                        if (reader.Name == "files")
                        {
                            List<FilesStruct> files = new List<FilesStruct>();
                            if (reader.ReadToDescendant("file"))
                            {
                                FilesStruct fs = new FilesStruct();
                                if (reader.GetAttribute("folder") != null)
                                    fs.Folder = reader.GetAttribute("folder");
                                reader.MoveToContent(); fs.URL = reader.Value;
                            }
                        }
                        break;

                }
            }
            return PackInformation;
        }

        private static void TestRead2()
        {
            XmlTextReader reader = new XmlTextReader(CONFIGSINDEX);

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

        private static void ReadConfigIndex()
        {
            XmlTextReader reader = new XmlTextReader(CONFIGSINDEX);

            bool getValueFromNext = true;
            //string print = "";
            ConfigPack temp = new ConfigPack();
            while (reader.Read())
            {
                if (reader.Value.StartsWith("version=\"1.0\""))
                    continue;
                if (reader.Value.StartsWith("\r\n"))
                    continue;
                if (reader.Value.Trim() == "")
                    continue;

                if (getValueFromNext)
                {
                    temp.URL = reader.Value;
                    //print += reader.Value;
                    getValueFromNext = false;
                    //Console.WriteLine (print);
                    ConfigList.Add(temp);
                }

                if (reader.GetAttribute("name") != null)
                    getValueFromNext = true;

                temp.FriendlyName = reader.GetAttribute("name");
                //temp.upd = int.Parse(reader.GetAttribute ("upd"));
                //print = reader.GetAttribute ("name") + " - ";
                //Console.WriteLine ("{0} - {1}", reader.GetAttribute("name"), val);
            }
        }
    }
}
