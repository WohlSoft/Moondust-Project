#include <iostream>
#include <PGE_File_Formats/file_formats.h>
#include <PGE_File_Formats/pge_file_lib_globs.h>
#include "dirent/dirent.h"
#include <time.h>
#include <chrono>

using namespace std;

class ElapsedTimer
{
public:
    typedef std::chrono::nanoseconds TimeT;
    ElapsedTimer() {}
    void start()
    {
        recent = std::chrono::high_resolution_clock::now();
    }
    void restart()
    {
        recent = std::chrono::high_resolution_clock::now();
    }
    int64_t elapsed()
    {
        using std::chrono::nanoseconds;
        using std::chrono::duration_cast;
        return duration_cast<nanoseconds>(std::chrono::high_resolution_clock::now() - recent).count();
    }
    std::chrono::high_resolution_clock::time_point recent;
};

class TheDir
{
public:
    TheDir() {}
    TheDir(std::string path) : m_path(path) {}
    int mkdir(std::string where)
    {
        std::string fullPath = m_path + "/" + where;
        #ifdef _WIN32
        return ::mkdir(fullPath.c_str());
        #else
        return ::mkdir(fullPath.c_str(), 0755);
        #endif
    }

    static bool stringCompare(const string &left, const string &right)
    {
        for(string::const_iterator lit = left.begin(), rit = right.begin(); lit != left.end() && rit != right.end(); ++lit, ++rit)
            if(tolower(*lit) < tolower(*rit))
                return true;
            else if(tolower(*lit) > tolower(*rit))
                return false;
        if(left.size() < right.size())
            return true;
        return false;
    }

    std::vector<std::string > entryList()
    {
        std::vector<std::string > list;
        DIR *dir;
        struct dirent *ent;
        dir = opendir(m_path.c_str());

        if(dir != NULL)
        {
            /* print all the files and directories within directory */
            while((ent = readdir(dir)) != NULL)
            {
                switch(ent->d_type)
                {
                case DT_REG:
                    //printf ("%*.*s\n", ent->d_namlen, ent->d_namlen, ent->d_name);
                {
                    std::string file = ent->d_name;
                    list.push_back(file);
                }
                break;

                case DT_DIR:
                    //printf ("%s (dir)\n", ent->d_name);
                    break;
                default:
                {
                    std::string file = ent->d_name;
                    list.push_back(file);
                }
                }
            }
            closedir(dir);
        }
        else
            std::cout << "\n" << "Directory not found! " << m_path << "\n";
        std::sort(list.begin(), list.end(), stringCompare);
        return list;
    }
    std::string m_path;
};

std::string flString(std::string str, int lenght)
{
    std::string fn;
    fn.resize(lenght);
    for(int i = 0; i < lenght; i++)
        fn[i] = ' ';
    for(int i = 0; (i < (signed)str.size()) && (i < (signed)str.size()); i++)
        fn[i] = str[i];
    return fn;
}

void printLevelInfo(LevelData &lvl)
{
    cout << "Level title: " << lvl.LevelName << "\n";
    cout << "Num of stars: " << lvl.stars << "\n";
    cout << "Sections: " << lvl.sections.size() << "\n";
    cout << "Blocks: " << lvl.blocks.size() << "\n";
    cout << "BGO's: " << lvl.bgo.size() << "\n";
    cout << "NPC's: " << lvl.npc.size() << "\n";
    cout << "Warps: " << lvl.doors.size() << "\n";
    cout << "Physical EnvZones: " << lvl.physez.size() << "\n";
    cout << "Layers: " << lvl.layers.size() << "\n";
    cout << "Events: " << lvl.events.size() << "\n";
    cout << "Variables: " << lvl.variables.size() << "\n";
    if(lvl.scripts.size() > (size_t)0)
    {
        cout << "Test of script printing" << "\n";
        for(size_t i = 0; i < lvl.scripts.size(); i++)
        {
            cout << "===========" << lvl.scripts[i].name << "============" << "\n";
            cout << lvl.scripts[i].script << "\n";
        }
        cout << "==================================" << "\n";
    }
}

void printWorldInfo(WorldData &lvl)
{
    cout << "Episode title: " << lvl.EpisodeTitle << "\n";
    cout << "Num of stars: " << lvl.stars << "\n";
    cout << "Credits:\n" << lvl.authors << "\n\n";
    cout << "Tiles: " << lvl.tiles.size() << "\n";
    cout << "Sceneries: " << lvl.scenery.size() << "\n";
    cout << "Paths: " << lvl.paths.size() << "\n";
    cout << "Levels: " << lvl.levels.size() << "\n";
    cout << "Musicboxes: " << lvl.music.size() << "\n\n";
}

void printLine()
{
    cout << "\n\n=============================\n";
}

int main()
{
    //cout << FileFormats::WriteExtendedLvlFile(FileFormats::dummyLvlDataArray()) << endl;
    LevelData level;
    printLine();
    cout << "\n\nSMBX64 Level Read Header test:" << endl;
    FileFormats::ReadSMBX64LvlFileHeader("test.lvl", level);
    cout << level.meta.filename << "\n";
    cout << level.meta.path << "\n";
    if(!level.meta.ReadFileValid)
        cout << "Invalid file\n" << FileFormats::errorString;
    else
        printLevelInfo(level);
    printLine();

    cout << "\n\nSMBX64 Level Read test:" << endl;
    FileFormats::OpenLevelFile("test.lvl", level);
    cout << level.meta.filename << "\n";
    cout << level.meta.path << "\n";
    if(!level.meta.ReadFileValid)
        cout << "Invalid file\n" << FileFormats::errorString;
    else
        printLevelInfo(level);

    FileFormats::SaveLevelFile(level, "test_out_64.lvl", FileFormats::LVL_SMBX64, 64);
    FileFormats::SaveLevelFile(level, "test_out_45.lvl", FileFormats::LVL_SMBX64, 45);
    FileFormats::SaveLevelFile(level, "test_out_1.lvl", FileFormats::LVL_SMBX64, 1);


    printLine();
    cout << "\n\nSMBX65-38A Level Read Header test:" << endl;
    FileFormats::ReadSMBX38ALvlFileHeader("test_65-38a.lvl", level);
    cout << level.meta.filename << "\n";
    cout << level.meta.path << "\n";
    if(!level.meta.ReadFileValid)
        cout << "Invalid file\n" << FileFormats::errorString;
    else
        printLevelInfo(level);



    printLine();
    cout << "\n\nSMBX65-38A Level Read test:" << endl;
    FileFormats::OpenLevelFile("test_65-38a.lvl", level);
    cout << level.meta.filename << "\n";
    cout << level.meta.path << "\n";
    if(!level.meta.ReadFileValid)
        cout << "Invalid file\n" << FileFormats::errorString;
    else
        printLevelInfo(level);



    printLine();
    cout << "\n\nPGE-X Level Read Header test:" << endl;
    FileFormats::ReadExtendedLvlFileHeader("test.lvlx", level);
    cout << level.meta.filename << "\n";
    cout << level.meta.path << "\n";
    if(!level.meta.ReadFileValid)
        cout << "Invalid file\n" << FileFormats::errorString;
    else
        printLevelInfo(level);

    printLine();
    cout << "\n\nPGE-X Level Read test:" << endl;
    FileFormats::OpenLevelFile("test.lvlx", level);
    cout << level.meta.filename << "\n";
    cout << level.meta.path << "\n";
    if(!level.meta.ReadFileValid)
        cout << "Invalid file\n" << FileFormats::errorString;
    else
        printLevelInfo(level);

    FileFormats::smbx64LevelPrepare(level);
    FileFormats::smbx64LevelSortBlocks(level);
    FileFormats::smbx64LevelSortBGOs(level);

    FileFormats::WriteExtendedLvlFileF("test_out.lvlx", level);


    WorldData world;
    printLine();
    cout << "\n\nSMBX64 World Read Header test:" << endl;
    FileFormats::ReadSMBX64WldFileHeader("test.wld", world);
    cout << world.meta.filename << "\n";
    cout << world.meta.path << "\n";
    if(!world.meta.ReadFileValid)
        cout << "Invalid file\n" << FileFormats::errorString;
    else
        printWorldInfo(world);

    printLine();
    cout << "\n\nSMBX64 World Read test:" << endl;
    FileFormats::OpenWorldFile("test.wld", world);
    cout << world.meta.filename << "\n";
    cout << world.meta.path << "\n";
    if(!world.meta.ReadFileValid)
        cout << "Invalid file\n" << FileFormats::errorString;
    else
        printWorldInfo(world);

    FileFormats::WriteSMBX64WldFileF("test_out_64.wld", world, 64);

    printLine();
    cout << "\n\nPGE-X World Read Header test:" << endl;
    FileFormats::ReadExtendedWldFileHeader("test.wldx", world);
    cout << world.meta.filename << "\n";
    cout << world.meta.path << "\n";
    if(!world.meta.ReadFileValid)
        cout << "Invalid file\n" << FileFormats::errorString;
    else
        printWorldInfo(world);

    printLine();
    cout << "\n\nPGE-X World Read test:" << endl;
    FileFormats::OpenWorldFile("test.wldx", world);
    cout << world.meta.filename << "\n";
    cout << world.meta.path << "\n";
    if(!world.meta.ReadFileValid)
        cout << "Invalid file\n" << FileFormats::errorString;
    else
        printWorldInfo(world);

    FileFormats::WriteExtendedWldFileF("test_out.wldx", world);

    PGE_FileFormats_misc::FileInfo x("shit.txt");
    cout << "\n\n\n";
    cout << "Name: " << x.filename() << endl;
    cout << "FPat: " << x.fullPath() << endl;
    cout << "BNam: " << x.basename() << endl;
    cout << "Sufx: " << x.suffix() << endl;
    cout << "Dirt: " << x.dirpath() << endl;



    //Deep tests of the level file formats
#define ENABLE_SMBX64_DEEPTEST
#define ENABLE_SMBX38A_DEEPTEST
#define ENABLE_PGEX_DEEPTEST //required SMBX64 deeptest to pre-generate LVLX files!

    #ifdef ENABLE_SMBX64_DEEPTEST
    /**********************DEEP TEST OF SMBX64 files*********************/
    {
        //#define GENERATE_LVLX_FILES
        cout << "==================DEEP TEST OF SMBX64==================\n";
        std::string path = "../PGEFileLib_test_files/smbx64/";
        std::string wpath = "../PGEFileLib_test_files/smbx64_out/";
        #ifdef GENERATE_LVLX_FILES
        QString xpath = "../PGEFileLib_test_files/pgex/";
        #endif
        TheDir testDir(path);
        testDir.mkdir("../smbx64_out/");
        #ifdef GENERATE_LVLX_FILES
        testDir.mkdir("../pgex/");
        #endif
        vector<string> files = testDir.entryList();

        std::ofstream niout;
        niout.open("invalid_s64.log", std::ios::out);

        std::ofstream timesout;
        timesout.open("times_s64.log", std::ios::out);

        ElapsedTimer meter;
        meter.start();

        for(const string &file : files)
        {
            PGE_FileFormats_misc::TextFileInput fileI(path + file, false);
            LevelData FileDataNew;

            FileDataNew = FileFormats::CreateLevelData();
            fileI.seek(0, PGE_FileFormats_misc::TextInput::begin);

            meter.restart();
            if(FileFormats::ReadSMBX64LvlFile(fileI, FileDataNew))
            {
                int64_t got = meter.elapsed();
                timesout << flString(file, 30) << " READ -> " << flString(std::to_string(got), 20);

                meter.restart();
                FileFormats::smbx64LevelPrepare(FileDataNew);
                FileFormats::WriteSMBX64LvlFileF(wpath + file, FileDataNew, FileDataNew.meta.RecentFormatVersion);
                got = meter.elapsed();
                timesout << " WRITE -> " << got << "\n";
                timesout.flush();
                #ifdef GENERATE_LVLX_FILES
                FileFormats::WriteExtendedLvlFileF(xpath + file + "x", FileDataNew);
                #endif
            }
            else
            {
                cout << "NEW PARSER FAILED: Invalid file\n" << FileFormats::errorString;
                niout << path + file << "\r\nInfo: "
                      << FileDataNew.meta.ERROR_info << "\r\nlinedata" << FileDataNew.meta.ERROR_linedata
                      << "\r\nline:" << FileDataNew.meta.ERROR_linenum << "\r\n\r\n";
                niout.flush();
            }
            cout.flush();
        }
        niout.close();
        cout << "==================DEEP TEST OF SMBX64=END==============\n";
        cout.flush();
    }
    /*********************************************************************/
    #endif //ENABLE_SMBX64_DEEPTEST

    #ifdef ENABLE_SMBX38A_DEEPTEST
    /**********************DEEP TEST OF SMBX38A files*********************/
    {
        cout << "==================DEEP TEST OF SMBX38A==================\n";
        std::string path = "../PGEFileLib_test_files/smbx38a/";
        string opath = "../PGEFileLib_test_files/smbx38a_lvlx_diffs/";
        string wpath = "../PGEFileLib_test_files/smbx38a_out/";
        TheDir testDir(path);
        testDir.mkdir("../smbx38a_lvlx_diffs");
        testDir.mkdir("../smbx38a_out");
        vector<string> files = testDir.entryList();

        std::ofstream niout;
        niout.open("invalid_new.log", std::ios::out);

        std::ofstream  oiout;
        oiout.open("invalid_old.log", std::ios::out);

        std::ofstream diout;
        diout.open("differents.log", std::ios::out);

        std::ofstream timesout;
        timesout.open("times.log", std::ios::out);

        ElapsedTimer meter;
        meter.start();

        for(const string &file : files)
        {
            PGE_FileFormats_misc::TextFileInput fileI(path + file, false);
            string raw_old;
            string raw_new;
            string raw_original;
            LevelData FileDataNew;
            LevelData FileDataOld;

            clock_t time_old = 0;
            clock_t time_new = 0;

            raw_original = fileI.readAll();

            FileDataNew = FileFormats::CreateLevelData();
            fileI.seek(0, PGE_FileFormats_misc::TextInput::begin);

            meter.restart();
            if(FileFormats::ReadSMBX38ALvlFile(fileI, FileDataNew))
            {
                int64_t got = meter.elapsed();
                time_new = got;
                timesout << flString(file, 30) << " NEW -> " <<  flString(std::to_string(got), 20);
                FileFormats::smbx64CountStars(FileDataNew);
                meter.restart();
                FileFormats::WriteSMBX38ALvlFileRaw(FileDataNew, raw_new);
                got = meter.elapsed();
                timesout << " WRITE -> " << flString(std::to_string(got), 20);

                FileFormats::WriteSMBX38ALvlFileF(wpath + file, FileDataNew);
            }
            else
            {
                cout << "NEW PARSER FAILED: Invalid file\n" << FileFormats::errorString;
                niout << path + file << "\r\nInfo: "
                      << FileDataNew.meta.ERROR_info << "\r\nlinedata" << FileDataNew.meta.ERROR_linedata
                      << "\r\nline:" << FileDataNew.meta.ERROR_linenum << "\r\n\r\n";
                niout.flush();
            }

            FileDataOld = FileFormats::CreateLevelData();
            fileI.close();

            fileI.open(path + file, false);
            meter.restart();
            if(FileFormats::ReadSMBX38ALvlFile_OLD(fileI, FileDataOld))
            {
                int64_t got = meter.elapsed();
                time_old = got;
                timesout << " OLD -> " << flString(std::to_string(got), 20);
                if(time_old > time_new)
                    timesout << " NEW READS FASTER";
                else if(time_old < time_new)
                    timesout << " OLD READS FASTER";
                else
                    timesout << " BOTH ARE SAME";
                timesout << "\n";
                FileFormats::smbx64CountStars(FileDataOld);
                FileFormats::WriteExtendedLvlFileRaw(FileDataOld, raw_old);
            }
            else
            {
                cout << "OLD PARSER FAILED: Invalid file\n" << FileFormats::errorString;
                oiout << path + file << "\r\nInfo: "
                      << FileDataOld.meta.ERROR_info << "\r\nlinedata" << FileDataOld.meta.ERROR_linedata
                      << "\r\nline:" << FileDataOld.meta.ERROR_linenum << "\r\n\r\n";
                oiout.flush();
            }

            if(raw_old.empty())
                continue;
            if(raw_new.empty())
                continue;
            if(raw_original != raw_new)
            {
                cout << "FILES ARE DIFFERENT\n";
                diout << path + file << "\r\n";
                diout.flush();
                FileFormats::WriteExtendedLvlFileF(opath + file + ".old.lvlx", FileDataNew);
                FileFormats::WriteExtendedLvlFileF(opath + file + ".new.lvlx", FileDataOld);
            }
            else
            {
                //Remove similar files!
                //QFile(opath+file+".old.lvlx").remove();
                //QFile(opath+file+".new.lvlx").remove();
            }
            cout.flush();
        }
        oiout.close();
        niout.close();
        diout.close();
        cout << "==================DEEP TEST OF SMBX38A=END==============\n";
        cout.flush();
    }
    /*********************************************************************/
    #endif//ENABLE_SMBX38A_DEEPTEST


    #if defined(ENABLE_SMBX64_DEEPTEST) && defined(ENABLE_PGEX_DEEPTEST)
    /**********************DEEP TEST OF PGE-X files*********************/
    {
        cout << "==================DEEP TEST OF PGE-X==================\n";
        string path = "../PGEFileLib_test_files/pgex/";
        string wpath = "../PGEFileLib_test_files/pgex_out/";
        TheDir testDir(path);
        testDir.mkdir("../pgex_out");
        vector<string> files = testDir.entryList();

        std::ofstream niout;
        niout.open("invalid_pgex.log", std::ios::out);

        std::ofstream timesout;
        timesout.open("times_pgex.log", std::ios::out);

        ElapsedTimer meter;
        meter.start();

        for(const string &file : files)
        {
            PGE_FileFormats_misc::TextFileInput fileI(path + file, false);
            LevelData FileDataNew;

            FileDataNew = FileFormats::CreateLevelData();
            fileI.seek(0, PGE_FileFormats_misc::TextInput::begin);

            meter.restart();
            if(FileFormats::ReadExtendedLvlFile(fileI, FileDataNew))
            {
                int64_t got = meter.elapsed();
                timesout << flString(file, 30) << " READ -> " << flString(std::to_string(got), 20);

                meter.restart();
                FileFormats::smbx64CountStars(FileDataNew);
                FileFormats::WriteExtendedLvlFileF(wpath + file, FileDataNew);
                got = meter.elapsed();
                timesout << " WRITE -> " << got << "\n";
                timesout.flush();

            }
            else
            {
                cout << "NEW PARSER FAILED: Invalid file\n" << FileFormats::errorString;
                niout << path + file << "\r\nInfo: "
                      << FileDataNew.meta.ERROR_info << "\r\nlinedata" << FileDataNew.meta.ERROR_linedata
                      << "\r\nline:" << FileDataNew.meta.ERROR_linenum << "\r\n\r\n";
                niout.flush();
            }
            cout.flush();
        }
        niout.close();
        cout << "==================DEEP TEST OF PGE-X=END==============\n";
        cout.flush();
    }
    /*********************************************************************/
    #endif

    cout << "!!!!!!!!!!!!!!!!!!!!!EVERYTHING HAS BEEN DONE!!!!!!!!!!!!!!!!!!!!!\n";


    return 0;
}
