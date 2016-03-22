
#include <QCoreApplication>
#include <QTextStream>
#include <PGE_File_Formats/file_formats.h>

//for DEEP test of SMBX38A
#include <QDir>

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
    cout << "\n\nSMBX65-38A Level Read Header test:" << endl;
    level=FileFormats::ReadSMBX65by38ALvlFileHeader("test_65-38a.lvl");
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
    level=FileFormats::OpenLevelFile("test_65-38a.lvl");
    cout << level.filename << "\n";
    cout << level.path << "\n";
    if(!level.ReadFileValid)
    {
        cout << "Invalid file\n" << FileFormats::errorString;
    } else {
        printLevelInfo(level,cout);
        QFile testOutput("test_65-38a-out.lvlx");
        testOutput.open(QIODevice::WriteOnly);
        QTextStream(&testOutput) << FileFormats::WriteExtendedLvlFile(level);
        testOutput.close();
    }

    /**********************DEEP TEST OF SMBX38A files*********************/
    {
        cout << "==================DEEP TEST OF SMBX38A==================\n";
        QString path = "38a_deep_test/";
        QString opath = "38_deep_test_out/";
        QDir testDir(path);
        QStringList files = testDir.entryList(QDir::NoDotAndDotDot|QDir::Files);

        QFile("invalid_new.log").remove();
        QFile("invalid_old.log").remove();
        QFile("differents.log").remove();

        QFile newInvalid("invalid_new.log");
        newInvalid.open(QIODevice::Append);
        QTextStream niout(&newInvalid);

        QFile oldInvalid("invalid_old.log");
        oldInvalid.open(QIODevice::Append);
        QTextStream oiout(&oldInvalid);

        QFile diffs("differents.log");
        diffs.open(QIODevice::Append);
        QTextStream diout(&diffs);

        foreach(QString file, files)
        {
            PGE_FileFormats_misc::TextFileInput fileI(path+file, false);
            QString raw_old;
            QString raw_new;

            LevelData FileData = FileFormats::CreateLevelData();
            fileI.seek(0, PGE_FileFormats_misc::TextInput::begin);
            if(FileFormats::ReadSMBX65by38ALvlFile(fileI, FileData))
            {
                QFile testOutput(opath+file+".new.lvlx");
                testOutput.open(QIODevice::WriteOnly);
                raw_new=FileFormats::WriteExtendedLvlFile(FileData);
                QTextStream(&testOutput) << raw_new;
                testOutput.flush();
                testOutput.close();
            } else {
                cout << "NEW PARSER FAILED: Invalid file\n" << FileFormats::errorString;
                niout << path+file << "\r\nInfo: "
                      << FileData.ERROR_info << "\r\nlinedata" << FileData.ERROR_linedata
                      << "\r\nline:" << FileData.ERROR_linenum << "\r\n\r\n";
                newInvalid.flush();
            }


            FileData = FileFormats::CreateLevelData();
            fileI.seek(0, PGE_FileFormats_misc::TextInput::begin);
            if(FileFormats::ReadSMBX65by38ALvlFile_OLD(fileI, FileData))
            {
                QFile testOutput(opath+file+".old.lvlx");
                testOutput.open(QIODevice::WriteOnly);
                raw_old=FileFormats::WriteExtendedLvlFile(FileData);
                QTextStream(&testOutput) << raw_old;
                testOutput.flush();
                testOutput.close();
            } else {
                cout << "OLD PARSER FAILED: Invalid file\n" << FileFormats::errorString;
                oiout << path+file << "\r\nInfo: "
                      << FileData.ERROR_info << "\r\nlinedata" << FileData.ERROR_linedata
                      << "\r\nline:" << FileData.ERROR_linenum << "\r\n\r\n";
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
            } else {
                //Remove similar files!
                QFile(opath+file+".old.lvlx").remove();
                QFile(opath+file+".old.lvlx").remove();
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

