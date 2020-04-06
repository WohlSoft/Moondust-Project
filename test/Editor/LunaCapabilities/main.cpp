#include <QCoreApplication>
#include <QTextStream>

#include "luna_capabilities.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTextStream out(stdout);
    QTextStream err(stderr);

    LunaLuaCapabilities caps;

    QStringList filezToChek;
    //!!!! CHANGE THIS LIST UNTIL BUILD THIS TEST !!!!
    filezToChek << "/home/VMs2/SMBX2-roots/SMBX2-Beta3/SMBX2-Beta3/data/LunaDll.dll"
                << "/home/VMs2/SMBX2-roots/SMBX2-MAGLX3/data/LunaDll.dll"
                << "/home/VMs2/SMBX2-roots/SMBX2-PAL/data/LunaDll.dll"
                << "/home/VMs2/SMBX2-roots/SMBX2-Beta4/data/LunaDll.dll"
                << "/home/VMs2/SMBX2-roots/SMBX2-Beta4p1/data/LunaDll.dll"
                << "/home/vitaly/_git_repos/SMBX-Revived/data/LunaDll.dll";

    for(auto &file : filezToChek)
    {
        out << "--------------- " << file << " ---------------\n";
        if(!getLunaCapabilities(caps, file))
        {
            out << "Can't open file!\n";
            out.flush();
            return 1;
        }

        out << "Type: " << caps.type << "\n";

        out << "\n";
        out << "Features:\n";
        for(auto &q : caps.features)
            out << " - " << q << "\n";

        out << "\n";
        out << "IPC Commands:\n";
        for(auto &q : caps.ipcCommands)
            out << " - " << q << "\n";

        out << "\n";
        out << "Command-line arguments:\n";
        for(auto &q : caps.args)
            out << " --" << q << "\n";

        out << "\n";
        out.flush();
    }

    out << "Done!\n";
    out.flush();

    return 0;
}
