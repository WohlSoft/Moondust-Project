
#include <QCoreApplication>
#include <QTextStream>
#include <PGE_File_Formats/file_formats.h>

//for DEEP test of SMBX38A
#include <QDir>
#include <QElapsedTimer>

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
    cout<<"Variables: "<< lvl.variables.size() << "\n";
    if(lvl.scripts.size()>0)
    {
        cout<< "Test of script printing" << "\n";
        for(int i=0; i<lvl.scripts.size(); i++)
        {
            cout<< "===========" << lvl.scripts[i].name << "============" << "\n";
            cout<<lvl.scripts[i].script << "\n";
        }
        cout<< "==================================" << "\n";
    }
    cout.flush();
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
    cout.flush();
}

void printLine(QTextStream &cout)
{
    cout <<"\n\n=============================\n";
    cout.flush();
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTextStream cout(stdout);
    char endl='\n';

    LevelData level;
    printLine(cout);
    cout << "\n\nSMBX64 Level Read Header test:" << endl;
    FileFormats::ReadSMBX64LvlFileHeader("test.lvl", level);
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
    FileFormats::OpenLevelFile("test.lvl", level);
    cout << level.filename << "\n";
    cout << level.path << "\n";
    if(!level.ReadFileValid)
    {
        cout << "Invalid file\n" << FileFormats::errorString;
    } else {
        printLevelInfo(level,cout);
        FileFormats::SaveLevelFile(level, "test_out_64.lvl", FileFormats::LVL_SMBX64, 64);
        FileFormats::SaveLevelFile(level, "test_out_50.lvl", FileFormats::LVL_SMBX64, 50);
        FileFormats::SaveLevelFile(level, "test_out_01.lvl", FileFormats::LVL_SMBX64, 1);
    }


    printLine(cout);
    cout << "\n\nSMBX65-38A Level Read Header test:" << endl;
    FileFormats::ReadSMBX38ALvlFileHeader("test_65-38a.lvl", level);
    cout << level.filename << "\n";
    cout << level.path << "\n";
    if(!level.ReadFileValid)
    {
        cout << "Invalid file\n" << FileFormats::errorString;
    } else {
        printLevelInfo(level,cout);
    }

    printLine(cout);
    cout << "\n\nSMBX65-38A Level Read test:" << endl;
    FileFormats::OpenLevelFile("test_65-38a.lvl", level);
    cout << level.filename << "\n";
    cout << level.path << "\n";
    if(!level.ReadFileValid)
    {
        cout << "Invalid file\n" << FileFormats::errorString;
    } else {
        printLevelInfo(level,cout);
        FileFormats::WriteExtendedLvlFileF("test_65-38a-out.lvlx", level);
    }

    /**********************DEEP TEST OF SMBX38A files*********************/
    {
        cout << "==================DEEP TEST OF SMBX38A==================\n";
        QString path = "38a_deep_test/";
        QString opath = "38_deep_test_out/";
        QDir testDir(path);
        testDir.mkdir("../38_deep_test_out");
        QStringList files = testDir.entryList(QDir::NoDotAndDotDot|QDir::Files);

        QFile newInvalid("invalid_new.log");
        newInvalid.open(QIODevice::WriteOnly|QIODevice::Truncate);
        QTextStream niout(&newInvalid);

        QFile oldInvalid("invalid_old.log");
        oldInvalid.open(QIODevice::WriteOnly|QIODevice::Truncate);
        QTextStream oiout(&oldInvalid);

        QFile diffs("differents.log");
        diffs.open(QIODevice::WriteOnly|QIODevice::Truncate);
        QTextStream diout(&diffs);

        QFile times("times.log");
        times.open(QIODevice::WriteOnly|QIODevice::Truncate);
        QTextStream timesout(&times);

        QElapsedTimer meter;
        meter.start();

        foreach(QString file, files)
        {
            PGE_FileFormats_misc::TextFileInput fileI(path+file, false);
            QString raw_old;
            QString raw_new;
            LevelData FileDataNew;
            LevelData FileDataOld;

            FileDataNew = FileFormats::CreateLevelData();
            fileI.seek(0, PGE_FileFormats_misc::TextInput::begin);

            meter.restart();
            if(FileFormats::ReadSMBX38ALvlFile(fileI, FileDataNew))
            {
                qint64 got = meter.elapsed();
                timesout << file << " NEW ->\t" << got << "\t\t";
                FileFormats::WriteExtendedLvlFileRaw(FileDataNew, raw_new);
            } else {
                cout << "NEW PARSER FAILED: Invalid file\n" << FileFormats::errorString;
                niout << path+file << "\r\nInfo: "
                      << FileDataNew.ERROR_info << "\r\nlinedata" << FileDataNew.ERROR_linedata
                      << "\r\nline:" << FileDataNew.ERROR_linenum << "\r\n\r\n";
                newInvalid.flush();
            }

            FileDataOld = FileFormats::CreateLevelData();
            fileI.seek(0, PGE_FileFormats_misc::TextInput::begin);

            meter.restart();
            if(FileFormats::ReadSMBX38ALvlFile_OLD(fileI, FileDataOld))
            {
                qint64 got = meter.elapsed();
                timesout << file << " OLD ->\t" << got << "\n";
                FileFormats::WriteExtendedLvlFileRaw(FileDataOld, raw_old);
            } else {
                cout << "OLD PARSER FAILED: Invalid file\n" << FileFormats::errorString;
                oiout << path+file << "\r\nInfo: "
                      << FileDataOld.ERROR_info << "\r\nlinedata" << FileDataOld.ERROR_linedata
                      << "\r\nline:" << FileDataOld.ERROR_linenum << "\r\n\r\n";
                oldInvalid.flush();
            }

            if(raw_old.isEmpty())
                continue;
            if(raw_new.isEmpty())
                continue;
            if(raw_old!=raw_new)
            {
                cout << "FILES ARE DIFFERENT\n";
                diout << path+file << "\r\n";
                diffs.flush();
                FileFormats::WriteExtendedLvlFileF(opath+file+".old.lvlx", FileDataNew);
                FileFormats::WriteExtendedLvlFileF(opath+file+".new.lvlx", FileDataOld);
            } else {
                //Remove similar files!
                QFile(opath+file+".old.lvlx").remove();
                QFile(opath+file+".new.lvlx").remove();
            }
            cout.flush();
        }
        oldInvalid.close();
        newInvalid.close();
        diffs.close();
        cout << "==================DEEP TEST OF SMBX38A=END==============\n";
        cout.flush();
    }
    /*********************************************************************/

    printLine(cout);
    cout << "\n\nPGE-X Level Read Header test:" << endl;
    FileFormats::ReadExtendedLvlFileHeader("test.lvlx", level);
    cout << level.filename << "\n";
    cout << level.path << "\n";
    if(!level.ReadFileValid)
    {
        cout << "Invalid file\n" << FileFormats::errorString;
    } else {
        printLevelInfo(level,cout);
    }

    printLine(cout);
    cout << "\n\nPGE-X Level Read test №2:" << endl;
    FileFormats::OpenLevelFile("test2.lvlx", level);
    cout << level.filename << "\n";
    cout << level.path << "\n";
    if(!level.ReadFileValid)
    {
        cout << "Invalid file\n" << FileFormats::errorString;
    } else {
        printLevelInfo(level,cout);
    }

    printLine(cout);
    cout << "\n\nPGE-X Level Read test №1:" << endl;
    FileFormats::OpenLevelFile("test.lvlx", level);
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
    FileFormats::ReadSMBX64WldFileHeader("test.wld", world);
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
    FileFormats::OpenWorldFile("test.wld", world);
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
    FileFormats::ReadExtendedWldFileHeader("test.wldx", world);
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
    FileFormats::OpenWorldFile("test.wldx", world);
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

