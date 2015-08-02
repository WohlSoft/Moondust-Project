using System;
using System.Collections.Generic;

namespace PGEManager.InternalStructs
{
    public enum FilesOS
    {
        win32, mac, linux, All
    }

    public class FilesStruct
    {
        public string Folder {get;set;}
        public string URL {get;set;}
        public string Platform {get;set;}

        public FilesOS StringToFilesOS(string conv)
        {
            switch (conv.ToLower())
            {
                case("win32"):
                    return FilesOS.win32;
                case("macosx"):
                    return FilesOS.mac;
                case("linux"):
                    return FilesOS.linux;
            }
            return FilesOS.All;
        }
    }

    public class AuthorStruct
    {
        public string Author { get; set;}
        public string Email { get; set;}
        public string Website {get;set;}
        public string Comment {get;set;}


        public AuthorStruct()
        {
        }
    }

    public class ConfigPackInformation
    {
        public string PackName {get;set;}
        public string Description {get;set;}
        public string IconURL { get; set;}
        public string SplashURL { get; set;}
        public bool IsSMBX64 { get; set;}
        public string License {get;set;}

        public List<KeyValuePair<string, AuthorStruct[]>> CreditsParts = new List<KeyValuePair<string, AuthorStruct[]>>();

        public List<FilesStruct> FilesParts = new List<FilesStruct>();


        public bool IsNull()
        {
            if (Description == null && IconURL == null && SplashURL == null && License == null)
                return true;
            else
                return false;
        }
    }
}

