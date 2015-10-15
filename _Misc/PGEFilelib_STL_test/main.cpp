#include <iostream>
#include <PGE_File_Formats/file_formats.h>
#include <PGE_File_Formats/pge_file_lib_globs.h>

using namespace std;

void printLevelInfo(LevelData &lvl)
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

void printWorldInfo(WorldData &lvl)
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

void printLine()
{
    cout <<"\n\n=============================\n";
}

int main()
{
    //cout << FileFormats::WriteExtendedLvlFile(FileFormats::dummyLvlDataArray()) << endl;
    LevelData level;
    std::ofstream fout;
    printLine();
    cout << "\n\nSMBX64 Level Read Header test:" << endl;
    level=FileFormats::ReadSMBX64LvlFileHeader("test.lvl");
    cout << level.filename << "\n";
    cout << level.path << "\n";
    if(!level.ReadFileValid)
    {
        cout << "Invalid file\n" << FileFormats::errorString;
    } else {
        printLevelInfo(level);
    }
    printLine();

    cout << "\n\nSMBX64 Level Read test:" << endl;
    level = FileFormats::OpenLevelFile("test.lvl");
    cout << level.filename << "\n";
    cout << level.path << "\n";
    if(!level.ReadFileValid)
    {
        cout << "Invalid file\n" << FileFormats::errorString;
    } else {
        printLevelInfo(level);
    }

    fout.open("test_out_64.lvl", std::ios::out);
    fout<<FileFormats::WriteSMBX64LvlFile(level, 64);
    fout.close();

    fout.open("test_out_45.lvl", std::ios::out);
    fout<<FileFormats::WriteSMBX64LvlFile(level, 45);
    fout.close();

    fout.open("test_out_1.lvl", std::ios::out);
    fout<<FileFormats::WriteSMBX64LvlFile(level, 1);
    fout.close();

    printLine();
    cout << "\n\nPGE-X Level Read Header test:" << endl;
    level=FileFormats::ReadExtendedLvlFileHeader("test.lvlx");
    cout << level.filename << "\n";
    cout << level.path << "\n";
    if(!level.ReadFileValid)
    {
        cout << "Invalid file\n" << FileFormats::errorString;
    } else {
        printLevelInfo(level);
    }

    printLine();
    cout << "\n\nPGE-X Level Read test:" << endl;
    level = FileFormats::OpenLevelFile("test.lvlx");
    cout << level.filename << "\n";
    cout << level.path << "\n";
    if(!level.ReadFileValid)
    {
        cout << "Invalid file\n" << FileFormats::errorString;
    } else {
        printLevelInfo(level);
    }

    fout.open("test_out.lvlx", std::ios::out);
    fout<<FileFormats::WriteExtendedLvlFile(level);
    fout.close();


    WorldData world;
    printLine();
    cout << "\n\nSMBX64 World Read Header test:" << endl;
    world=FileFormats::ReadSMBX64WldFileHeader("test.wld");
    cout << world.filename << "\n";
    cout << world.path << "\n";
    if(!world.ReadFileValid)
    {
        cout << "Invalid file\n" << FileFormats::errorString;
    } else {
        printWorldInfo(world);
    }

    printLine();
    cout << "\n\nSMBX64 World Read test:" << endl;
    world = FileFormats::OpenWorldFile("test.wld");
    cout << world.filename << "\n";
    cout << world.path << "\n";
    if(!world.ReadFileValid)
    {
        cout << "Invalid file\n" << FileFormats::errorString;
    } else {
        printWorldInfo(world);
    }

    fout.open("test_out_64.wld", std::ios::out);
    fout<<FileFormats::WriteSMBX64WldFile(world, 64);
    fout.close();

    printLine();
    cout << "\n\nPGE-X World Read Header test:" << endl;
    world=FileFormats::ReadExtendedWldFileHeader("test.wldx");
    cout << world.filename << "\n";
    cout << world.path << "\n";
    if(!world.ReadFileValid)
    {
        cout << "Invalid file\n" << FileFormats::errorString;
    } else {
        printWorldInfo(world);
    }

    printLine();
    cout << "\n\nPGE-X World Read test:" << endl;
    world = FileFormats::OpenWorldFile("test.wldx");
    cout << world.filename << "\n";
    cout << world.path << "\n";
    if(!world.ReadFileValid)
    {
        cout << "Invalid file\n" << FileFormats::errorString;
    } else {
        printWorldInfo(world);
    }

    fout.open("test_out.wldx", std::ios::out);
    fout<<FileFormats::WriteExtendedWldFile(world);
    fout.close();


    PGE_FileFormats_misc::FileInfo x("melw.shit.LvlVX");
    cout << "\n\n\n";
    cout << "Name: " << x.filename() << endl;
    cout << "FPat: " << x.fullPath() << endl;
    cout << "BNam: " << x.basename() << endl;
    cout << "Sufx: " << x.suffix() << endl;
    cout << "Dirt: " << x.dirpath() << endl;
    return 0;
}

