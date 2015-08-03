// *******************************
// *** INIFile class V2.1      ***
// *******************************
// *** (C)2009-2013 S.T.A. snc ***
// *******************************
using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Text;

namespace STA.Settings
{

	internal class INIFile
	{

#region "Declarations"

		// *** Lock for thread-safe access to file and local cache ***
		private object m_Lock = new object();

		// *** File name ***
		private string m_FileName = null;
		internal string FileName
		{
			get
			{
				return m_FileName;
			}
		}

		// *** Lazy loading flag ***
		private bool m_Lazy = false;

        // *** Automatic flushing flag ***
        private bool m_AutoFlush = false;

		// *** Local cache ***
        private Dictionary<string, Dictionary<string, string>> m_Sections = new Dictionary<string, Dictionary<string, string>>();
        private Dictionary<string, Dictionary<string, string>> m_Modified = new Dictionary<string, Dictionary<string, string>>(); 

		// *** Local cache modified flag ***
		private bool m_CacheModified = false;

#endregion

#region "Methods"

		// *** Constructor ***
		public INIFile(string FileName)
		{
			Initialize(FileName, false, false);
		}

		public INIFile(string FileName, bool Lazy, bool AutoFlush)
		{
            Initialize(FileName, Lazy, AutoFlush);
		}

		// *** Initialization ***
        private void Initialize(string FileName, bool Lazy, bool AutoFlush)
		{
			m_FileName = FileName;
			m_Lazy = Lazy;
            m_AutoFlush = AutoFlush;
			if (!m_Lazy) Refresh();
		}

        // *** Parse section name ***
        private string ParseSectionName(string Line)
        {
            if (!Line.StartsWith("[")) return null;
            if (!Line.EndsWith("]")) return null;
            if (Line.Length < 3) return null;
            return Line.Substring(1, Line.Length - 2);
        }

        // *** Parse key+value pair ***
        private bool ParseKeyValuePair(string Line, ref string Key, ref string Value)
        {
            // *** Check for key+value pair ***
            int i;
            if ((i = Line.IndexOf('=')) <= 0) return false;
            
            int j = Line.Length - i - 1;
            Key = Line.Substring(0, i).Trim();
            if (Key.Length <= 0) return false;

            Value = (j > 0) ? (Line.Substring(i + 1, j).Trim()) : ("");
            return true;
        }

		// *** Read file contents into local cache ***
		internal void Refresh()
		{
			lock (m_Lock)
			{
				StreamReader sr = null;
				try
				{
					// *** Clear local cache ***
					m_Sections.Clear();
                    m_Modified.Clear();

					// *** Open the INI file ***
					try
					{
						sr = new StreamReader(m_FileName);
					}
					catch (FileNotFoundException)
					{
						return;
					}

					// *** Read up the file content ***
					Dictionary<string, string> CurrentSection = null;
					string s;
                    string SectionName;
                    string Key = null;
                    string Value = null;
					while ((s = sr.ReadLine()) != null)
					{
						s = s.Trim();
						
						// *** Check for section names ***
                        SectionName = ParseSectionName(s);
                        if (SectionName != null)
						{
						    // *** Only first occurrence of a section is loaded ***
						    if (m_Sections.ContainsKey(SectionName))
						    {
							    CurrentSection = null;
						    }
						    else
						    {
							    CurrentSection = new Dictionary<string, string>();
                                m_Sections.Add(SectionName, CurrentSection);
						    }
						}
						else if (CurrentSection != null)
						{
                            // *** Check for key+value pair ***
                            if (ParseKeyValuePair(s, ref Key, ref Value))
                            {
                                // *** Only first occurrence of a key is loaded ***
                                if (!CurrentSection.ContainsKey(Key))
                                {
                                    CurrentSection.Add(Key, Value);
                                }
                            }
						}
					}
				}
				finally
				{
					// *** Cleanup: close file ***
					if (sr != null) sr.Close();
					sr = null;
				}
			}
		}
		
		// *** Flush local cache content ***
		internal void Flush()
        {
            lock (m_Lock)
            {
                PerformFlush();
            }
        }

        private void PerformFlush()
        {
            // *** If local cache was not modified, exit ***
            if (!m_CacheModified) return;
            m_CacheModified = false;

            // *** Check if original file exists ***
            bool OriginalFileExists = File.Exists(m_FileName);

            // *** Get temporary file name ***
            string TmpFileName = Path.ChangeExtension(m_FileName, "$n$");

            // *** Copy content of original file to temporary file, replace modified values ***
            StreamWriter sw = null;

            // *** Create the temporary file ***
            sw = new StreamWriter(TmpFileName);

            try
            {
                Dictionary<string, string> CurrentSection = null;
                if (OriginalFileExists)
                {
                    StreamReader sr = null;
                    try
                    {
                        // *** Open the original file ***
                        sr = new StreamReader(m_FileName);

                        // *** Read the file original content, replace changes with local cache values ***
                        string s;
                        string SectionName;
                        string Key = null;
                        string Value = null;
                        bool Unmodified;
                        bool Reading = true;
                        while (Reading)
                        {
                            s = sr.ReadLine();
                            Reading = (s != null);

                            // *** Check for end of file ***
                            if (Reading)
                            {
                                Unmodified = true;
                                s = s.Trim();
                                SectionName = ParseSectionName(s);
                            }
                            else
                            {
                                Unmodified = false;
                                SectionName = null;
                            }

                            // *** Check for section names ***
                            if ((SectionName != null) || (!Reading))
                            {
                                if (CurrentSection != null)
                                {
                                    // *** Write all remaining modified values before leaving a section ****
                                    if (CurrentSection.Count > 0)
                                    {
                                        foreach (string fkey in CurrentSection.Keys)
                                        {
                                            if (CurrentSection.TryGetValue(fkey, out Value))
                                            {
                                                sw.Write(fkey);
                                                sw.Write('=');
                                                sw.WriteLine(Value);
                                            }
                                        }
                                        sw.WriteLine();
                                        CurrentSection.Clear();
                                    }
                                }

                                if (Reading)
                                {
                                    // *** Check if current section is in local modified cache ***
                                    if (!m_Modified.TryGetValue(SectionName, out CurrentSection))
                                    {
                                        CurrentSection = null;
                                    }
                                }
                            }
                            else if (CurrentSection != null)
                            {
                                // *** Check for key+value pair ***
                                if (ParseKeyValuePair(s, ref Key, ref Value))
                                {
                                    if (CurrentSection.TryGetValue(Key, out Value))
                                    {
                                        // *** Write modified value to temporary file ***
                                        Unmodified = false;
                                        CurrentSection.Remove(Key);

                                        sw.Write(Key);
                                        sw.Write('=');
                                        sw.WriteLine(Value);
                                    }
                                }
                            }

                            // *** Write unmodified lines from the original file ***
                            if (Unmodified)
                            {
                                sw.WriteLine(s);
                            }
                        }

                        // *** Close the original file ***
                        sr.Close();
                        sr = null;
                    }
                    finally
                    {
                        // *** Cleanup: close files ***                  
                        if (sr != null) sr.Close();
                        sr = null;
                    }
                }

                // *** Cycle on all remaining modified values ***
                foreach (KeyValuePair<string, Dictionary<string, string>> SectionPair in m_Modified)
                {
                    CurrentSection = SectionPair.Value;
                    if (CurrentSection.Count > 0)
                    {
                        sw.WriteLine();

                        // *** Write the section name ***
                        sw.Write('[');
                        sw.Write(SectionPair.Key);
                        sw.WriteLine(']');

                        // *** Cycle on all key+value pairs in the section ***
                        foreach (KeyValuePair<string, string> ValuePair in CurrentSection)
                        {
                            // *** Write the key+value pair ***
                            sw.Write(ValuePair.Key);
                            sw.Write('=');
                            sw.WriteLine(ValuePair.Value);
                        }
                        CurrentSection.Clear();
                    }
                }
                m_Modified.Clear();

                // *** Close the temporary file ***
                sw.Close();
                sw = null;

                // *** Rename the temporary file ***
                File.Copy(TmpFileName, m_FileName, true);

                // *** Delete the temporary file ***
                File.Delete(TmpFileName);
            }
            finally
            {
                // *** Cleanup: close files ***                  
                if (sw != null) sw.Close();
                sw = null;
            }
        }
	
		// *** Read a value from local cache ***
		internal string GetValue(string SectionName, string Key, string DefaultValue)
		{
			// *** Lazy loading ***
			if (m_Lazy)
			{
				m_Lazy = false;
				Refresh();
			}

			lock (m_Lock)
			{
				// *** Check if the section exists ***
				Dictionary<string, string> Section;
				if (!m_Sections.TryGetValue(SectionName, out Section)) return DefaultValue;

				// *** Check if the key exists ***
				string Value;
				if (!Section.TryGetValue(Key, out Value)) return DefaultValue;
			
				// *** Return the found value ***
				return Value;
			}
		}

		// *** Insert or modify a value in local cache ***
		internal void SetValue(string SectionName, string Key, string Value)
		{
			// *** Lazy loading ***
			if (m_Lazy)
			{
				m_Lazy = false;
				Refresh();
			}

			lock (m_Lock)
			{
				// *** Flag local cache modification ***
				m_CacheModified = true;

				// *** Check if the section exists ***
				Dictionary<string, string> Section;
				if (!m_Sections.TryGetValue(SectionName, out Section))
				{
					// *** If it doesn't, add it ***
					Section = new Dictionary<string, string>();
					m_Sections.Add(SectionName,Section);
				}

				// *** Modify the value ***
				if (Section.ContainsKey(Key)) Section.Remove(Key);
				Section.Add(Key, Value);

                // *** Add the modified value to local modified values cache ***
                if (!m_Modified.TryGetValue(SectionName, out Section))
                {
                    Section = new Dictionary<string, string>();
                    m_Modified.Add(SectionName, Section);
                }

                if (Section.ContainsKey(Key)) Section.Remove(Key);
                Section.Add(Key, Value);

                // *** Automatic flushing : immediately write any modification to the file ***
                if (m_AutoFlush) PerformFlush();
			}
		}

		// *** Encode byte array ***
        private string EncodeByteArray(byte[] Value)
        {
            if (Value == null) return null;

            StringBuilder sb = new StringBuilder();
            foreach (byte b in Value)
            {
                string hex = Convert.ToString(b, 16);
                int l = hex.Length;
                if (l > 2)
                {
                    sb.Append(hex.Substring(l - 2, 2));
                }
                else
                {
                    if (l < 2) sb.Append("0");
                    sb.Append(hex);
                }
            }
            return sb.ToString();
        }

		// *** Decode byte array ***
		private byte[] DecodeByteArray(string Value)
		{
			if (Value == null) return null;

			int l = Value.Length;
			if (l < 2) return new byte[] { };
			
			l /= 2;
			byte[] Result = new byte[l];
            for (int i = 0; i < l; i++) Result[i] = Convert.ToByte(Value.Substring(i * 2, 2), 16);
			return Result;
		}

		// *** Getters for various types ***
		internal bool GetValue(string SectionName, string Key, bool DefaultValue)
		{
            string StringValue = GetValue(SectionName, Key, DefaultValue.ToString(System.Globalization.CultureInfo.InvariantCulture));
			int Value;
			if (int.TryParse(StringValue, out Value)) return (Value != 0);
			return DefaultValue;
		}

		internal int GetValue(string SectionName, string Key, int DefaultValue)
		{
            string StringValue = GetValue(SectionName, Key, DefaultValue.ToString(CultureInfo.InvariantCulture));
			int Value;
			if (int.TryParse(StringValue, NumberStyles.Any, CultureInfo.InvariantCulture, out Value)) return Value;
            return DefaultValue;
		}

        internal long GetValue(string SectionName, string Key, long DefaultValue)
        {
            string StringValue = GetValue(SectionName, Key, DefaultValue.ToString(CultureInfo.InvariantCulture));
            long Value;
            if (long.TryParse(StringValue, NumberStyles.Any, CultureInfo.InvariantCulture, out Value)) return Value;
            return DefaultValue;
        }

		internal double GetValue(string SectionName, string Key, double DefaultValue)
		{
            string StringValue = GetValue(SectionName, Key, DefaultValue.ToString(CultureInfo.InvariantCulture));
			double Value;
			if (double.TryParse(StringValue, NumberStyles.Any, CultureInfo.InvariantCulture, out Value)) return Value;
			return DefaultValue;
		}

		internal byte[] GetValue(string SectionName, string Key, byte[] DefaultValue)
		{
			string StringValue = GetValue(SectionName, Key, EncodeByteArray(DefaultValue));
			try
			{
				return DecodeByteArray(StringValue);
			}
			catch (FormatException)
			{
				return DefaultValue;
			}
		}

        internal DateTime GetValue(string SectionName, string Key, DateTime DefaultValue)
        {
            string StringValue = GetValue(SectionName, Key, DefaultValue.ToString(CultureInfo.InvariantCulture));
            DateTime Value;
            if (DateTime.TryParse(StringValue, CultureInfo.InvariantCulture, DateTimeStyles.AllowWhiteSpaces | DateTimeStyles.NoCurrentDateDefault | DateTimeStyles.AssumeLocal, out Value)) return Value;
            return DefaultValue;
        }

		// *** Setters for various types ***
		internal void SetValue(string SectionName, string Key, bool Value)
		{
			SetValue(SectionName, Key, (Value) ? ("1") : ("0"));
		}

        internal void SetValue(string SectionName, string Key, int Value)
        {
            SetValue(SectionName, Key, Value.ToString(CultureInfo.InvariantCulture));
        }

        internal void SetValue(string SectionName, string Key, long Value)
        {
            SetValue(SectionName, Key, Value.ToString(CultureInfo.InvariantCulture));
        }

		internal void SetValue(string SectionName, string Key, double Value)
		{
			SetValue(SectionName, Key, Value.ToString(CultureInfo.InvariantCulture));
		}

		internal void SetValue(string SectionName, string Key, byte[] Value)
		{
			SetValue(SectionName, Key, EncodeByteArray(Value));
		}

        internal void SetValue(string SectionName, string Key, DateTime Value)
        {
            SetValue(SectionName, Key, Value.ToString(CultureInfo.InvariantCulture));
        }

#endregion

	}

}
