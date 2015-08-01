using System;
using System.Xml;
using System.Collections.Generic;

namespace XmlTester
{
    class MainClass
    {
        private static string CONFIGSINDEX = "http://download.gna.org/pgewohlstand/configs/configs.index";
        static List<ConfigPack> ConfigList = new List<ConfigPack>();

        public static void Main(string[] args)
        {
            Console.WriteLine("Reading....\n\n");

            TestRead2();

            foreach (var cfg in ConfigList)
            {
                Console.WriteLine("-{0}, URL: {1}, upd: {2}", cfg.FriendlyName, cfg.URL, cfg.upd);
            }

            Console.WriteLine("\nDone :)");
            Console.ReadLine();
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
