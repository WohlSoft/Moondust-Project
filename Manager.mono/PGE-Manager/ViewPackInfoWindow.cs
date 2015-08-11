using System;
using PGEManager.Internal;

namespace PGEManager
{
    public partial class ViewPackInfoWindow : Gtk.Window
    {
        private ConfigPackInformation _configPackInformation;

        public ViewPackInfoWindow(ConfigPackInformation cpi)
            : base(Gtk.WindowType.Toplevel)
        {
            _configPackInformation = cpi;
            this.Build();
            this.HideAll(); this.ShowAll();

            InitializeTreeView();
        }

        private void InitializeTreeView()
        {
            Gtk.ListStore model = new Gtk.ListStore(typeof(string));
            treeview1.AppendColumn("", new Gtk.CellRendererText(), "text", 0);

            model.AppendValues("Pack Name: {0}", _configPackInformation.PackName);
            model.AppendValues(_configPackInformation.Description);

            Gtk.TreeIter iter = model.AppendValues("Credits");
            foreach (var thing in _configPackInformation.CreditsParts)
            {
                iter = model.AppendValues(thing.Key);
                foreach (var author in thing.Value)
                {
                    model.AppendValues(iter, author.Author);
                }
            }
        }
    }
}

