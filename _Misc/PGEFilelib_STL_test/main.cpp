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
    cout<<"Variables: "<< lvl.variables.size() << "\n";
    if(lvl.scripts.size()>(size_t)0)
    {
        cout<< "Test of script printing" << "\n";
        for(size_t i=0; i<lvl.scripts.size(); i++)
        {
            cout<< "===========" << lvl.scripts[i].name << "============" << "\n";
            cout<<lvl.scripts[i].script << "\n";
        }
        cout<< "==================================" << "\n";
    }
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
    FileFormats::ReadSMBX64LvlFileHeader("test.lvl", level);
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
    FileFormats::OpenLevelFile("test.lvl", level);
    cout << level.filename << "\n";
    cout << level.path << "\n";
    if(!level.ReadFileValid)
    {
        cout << "Invalid file\n" << FileFormats::errorString;
    } else {
        printLevelInfo(level);
    }

    FileFormats::SaveLevelFile(level, "test_out_64.lvl", FileFormats::LVL_SMBX64, 64);
    FileFormats::SaveLevelFile(level, "test_out_45.lvl", FileFormats::LVL_SMBX64, 45);
    FileFormats::SaveLevelFile(level, "test_out_1.lvl", FileFormats::LVL_SMBX64, 1);


    printLine();
    cout << "\n\nSMBX65-38A Level Read Header test:" << endl;
    FileFormats::ReadSMBX38ALvlFileHeader("test_65-38a.lvl", level);
    cout << level.filename << "\n";
    cout << level.path << "\n";
    if(!level.ReadFileValid)
    {
        cout << "Invalid file\n" << FileFormats::errorString;
    } else {
        printLevelInfo(level);
    }



    printLine();
    cout << "\n\nSMBX65-38A Level Read test:" << endl;
    FileFormats::OpenLevelFile("test_65-38a.lvl", level);
    cout << level.filename << "\n";
    cout << level.path << "\n";
    if(!level.ReadFileValid)
    {
        cout << "Invalid file\n" << FileFormats::errorString;
    } else {
        printLevelInfo(level);
    }



    printLine();
    cout << "\n\nPGE-X Level Read Header test:" << endl;
    FileFormats::ReadExtendedLvlFileHeader("test.lvlx", level);
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
    FileFormats::OpenLevelFile("test.lvlx", level);
    cout << level.filename << "\n";
    cout << level.path << "\n";
    if(!level.ReadFileValid)
    {
        cout << "Invalid file\n" << FileFormats::errorString;
    } else {
        printLevelInfo(level);
    }

    FileFormats::smbx64LevelPrepare(level);
    FileFormats::smbx64LevelSortBlocks(level);
    FileFormats::smbx64LevelSortBGOs(level);

    FileFormats::WriteExtendedLvlFileF("test_out.lvlx", level);


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
    FileFormats::OpenWorldFile("test.wld", world);
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
    FileFormats::OpenWorldFile("test.wldx", world);
    cout << world.filename << "\n";
    cout << world.path << "\n";
    if(!world.ReadFileValid)
    {
        cout << "Invalid file\n" << FileFormats::errorString;
    } else {
        printWorldInfo(world);
    }

    FileFormats::WriteExtendedWldFileF("test_out.wldx", world);

    PGE_FileFormats_misc::FileInfo x("shit.txt");
    cout << "\n\n\n";
    cout << "Name: " << x.filename() << endl;
    cout << "FPat: " << x.fullPath() << endl;
    cout << "BNam: " << x.basename() << endl;
    cout << "Sufx: " << x.suffix() << endl;
    cout << "Dirt: " << x.dirpath() << endl;
    return 0;
}

