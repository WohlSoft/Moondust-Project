
#include <QCoreApplication>
#include <QTextStream>
#include <PGE_File_Formats/file_formats.h>

void printLevelInfo(LevelData &lvl, QTextStream &cout)
{
    cout<<"Level title: "<< lvl.LevelName << "\n";
    cout<<"Num of stars: "<< lvl.stars << "\n";
    cout<<"Sections: "<< lvl.sections.size() << "\n";
    cout<<"Blocks: "<< lvl.blocks.size() << "\n";
    cout<<"BGO's: "<< lvl.bgo.size() << "\n";
    cout<<"NPC's: "<< lvl.npc.size() << "\n";
    cout<<"Warps: "<< lvl.doors.size() << "\n";
    cout<<"Physical EnvZones: "<< lvl.physez.size() << "\n";
    cout<<"Layers: "<< lvl.layers.size() << "\n";
    cout<<"Events: "<< lvl.events.size() << "\n";
}

void printWorldInfo(WorldData &lvl, QTextStream &cout)
{
    cout<<"Episode title: "<< lvl.EpisodeTitle<< "\n";
    cout<<"Num of stars: "<< lvl.stars << "\n";
    cout<<"Credits:\n"<< lvl.authors << "\n\n";
    cout<<"Tiles: "<< lvl.tiles.size() << "\n";
    cout<<"Sceneries: "<< lvl.scenery.size() << "\n";
    cout<<"Paths: "<< lvl.paths.size() << "\n";
    cout<<"Levels: "<< lvl.levels.size() << "\n";
    cout<<"Musicboxes: "<< lvl.music.size() << "\n\n";
}

void printLine(QTextStream &cout)
{
    cout <<"\n\n=============================\n";
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTextStream cout(stdout);
    char endl='\n';

    LevelData level;
    printLine(cout);
    cout << "\n\nSMBX64 Level Read Header test:" << endl;
    level=FileFormats::ReadSMBX64LvlFileHeader("test.lvl");
    cout << level.filename << "\n";
    cout << level.path << "\n";
    if(!level.ReadFileValid)
    {
        cout << "Invalid file\n" << FileFormats::errorString;
    } else {
        printLevelInfo(level,cout);
    }
    printLine(cout);

    cout << "\n\nSMBX64 Level Read test:" << endl;
    level = FileFormats::OpenLevelFile("test.lvl");
    cout << level.filename << "\n";
    cout << level.path << "\n";
    if(!level.ReadFileValid)
    {
        cout << "Invalid file\n" << FileFormats::errorString;
    } else {
        printLevelInfo(level,cout);
    }

    printLine(cout);
    cout << "\n\nPGE-X Level Read Header test:" << endl;
    level=FileFormats::ReadExtendedLvlFileHeader("test.lvlx");
    cout << level.filename << "\n";
    cout << level.path << "\n";
    if(!level.ReadFileValid)
    {
        cout << "Invalid file\n" << FileFormats::errorString;
    } else {
        printLevelInfo(level,cout);
    }

    printLine(cout);
    cout << "\n\nPGE-X Level Read test:" << endl;
    level = FileFormats::OpenLevelFile("test.lvlx");
    cout << level.filename << "\n";
    cout << level.path << "\n";
    if(!level.ReadFileValid)
    {
        cout << "Invalid file\n" << FileFormats::errorString;
    } else {
        printLevelInfo(level,cout);
    }

    WorldData world;
    printLine(cout);
    cout << "\n\nSMBX64 World Read Header test:" << endl;
    world=FileFormats::ReadSMBX64WldFileHeader("test.wld");
    cout << world.filename << "\n";
    cout << world.path << "\n";
    if(!world.ReadFileValid)
    {
        cout << "Invalid file\n" << FileFormats::errorString;
    } else {
        printWorldInfo(world,cout);
    }

    printLine(cout);
    cout << "\n\nSMBX64 World Read test:" << endl;
    world = FileFormats::OpenWorldFile("test.wld");
    cout << world.filename << "\n";
    cout << world.path << "\n";
    if(!world.ReadFileValid)
    {
        cout << "Invalid file\n" << FileFormats::errorString;
    } else {
        printWorldInfo(world,cout);
    }

    printLine(cout);
    cout << "\n\nPGE-X World Read Header test:" << endl;
    world=FileFormats::ReadExtendedWldFileHeader("test.wldx");
    cout << world.filename << "\n";
    cout << world.path << "\n";
    if(!world.ReadFileValid)
    {
        cout << "Invalid file\n" << FileFormats::errorString;
    } else {
        printWorldInfo(world,cout);
    }

    printLine(cout);
    cout << "\n\nPGE-X World Read test:" << endl;
    world = FileFormats::OpenWorldFile("test.wldx");
    cout << world.filename << "\n";
    cout << world.path << "\n";
    if(!world.ReadFileValid)
    {
        cout << "Invalid file\n" << FileFormats::errorString;
    } else {
        printWorldInfo(world,cout);
    }


    PGE_FileFormats_misc::FileInfo x("melw.shit.LvlVX");
    cout << "\n\n\n";
    cout << "Name: " << x.filename() << endl;
    cout << "FPat: " << x.fullPath() << endl;
    cout << "BNam: " << x.basename() << endl;
    cout << "Sufx: " << x.suffix() << endl;
    cout << "Dirt: " << x.dirpath() << endl;

    a.exit(0);
    return 0;
}

