using System;

namespace PGEManager.Internal
{
	public class ConfigPack
	{
		public string FriendlyName {get;set;}
		public string URL { get; set;}
		public int upd { get; set; }

		public ConfigPack ()
		{
		}

        public static DateTime UnixTimeStampToDateTime( double unixTimeStamp )
        {
            // Unix timestamp is seconds past epoch
            System.DateTime dtDateTime = new DateTime(1970,1,1,0,0,0,0,System.DateTimeKind.Utc);
            dtDateTime = dtDateTime.AddSeconds( unixTimeStamp ).ToLocalTime();
            return dtDateTime;
        }
	}
}

