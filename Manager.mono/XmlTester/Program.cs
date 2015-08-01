using System;
using System.Xml;
using System.Collections.Generic;

namespace XmlTester
{
    class MainClass
    {
        private static string CONFIGDIR = "http://download.gna.org/pgewohlstand/configs/";
        private static string CONFIGSINDEX = "http://download.gna.org/pgewohlstand/configs/configs.index";
        static List<ConfigPack> ConfigList = new List<ConfigPack>();

        public static void Main(string[] args)
        {
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

            ConfigPackInformation cpi = TryReadPackInformation(ConfigList[selection]);
            PrintInformation(cpi);

            Console.WriteLine("\nDone :)");
            Console.ReadLine();
        }

        private static void PrintInformation(ConfigPackInformation cp)
        {
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
                }
            }

            Console.WriteLine(cp.IconURL + " & " + cp.SplashURL);
            Console.WriteLine("SMBX64: " + cp.IsSMBX64);

            Console.WriteLine("\n\nDone:)");
            Console.ReadLine();
        }

        private static ConfigPackInformation TryReadPackInformation(ConfigPack selected)
        {
            XmlTextReader reader = new XmlTextReader(CONFIGDIR + selected.URL + "pge_cpack.xml");

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
                            PackInformation.License = reader.Value;
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
                                        auth.Author = reader.Value;

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
                                fs.URL = reader.Value;
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
