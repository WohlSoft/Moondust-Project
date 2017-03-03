#include <stdint.h>
#include <chrono>
#include <stdio.h>
#include <set>

#include <DirManager/dirman.h>

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

#define BENCHMARK(taskName, expression)\
{\
    ElapsedTimer clock;\
    clock.start();\
    {expression}\
    int64_t done = clock.elapsed();\
    printf("=== [%lli] nanoseconds == %s \n", static_cast<long long>(done), (taskName));\
    \
}


int main(int , char **)
{
    DirMan dm("dummy");
    if(!dm.exists())
        dm.mkAbsDir(dm.absolutePath());

    {
        for(int i = 0; i < 3000; i++)
        {
            std::string myfile = dm.absolutePath() + "/xxx-" + std::to_string(i) + ".gif";
            FILE *junk = fopen(myfile.c_str(), "w");
            fprintf(junk, "Just a test!");
            fclose(junk);
        }
    }

    BENCHMARK("Test existing of every",
        for(int i = 0; i < 40000; i++)
        {
            std::string myfile = dm.absolutePath() + "/xxx-" + std::to_string(i) + ".gif";
            FILE *junk = fopen(myfile.c_str(), "r");
            if(junk)
                fclose(junk);
        }
    );

    BENCHMARK("Collecting list and testing by hash-table",
        std::set<std::string> files;
        std::vector<std::string> fileList;
        dm.getListOfFiles(fileList, {".gif", ".png"});
        for(std::string &s : fileList)
            files.insert(s);

        for(int i = 0; i < 40000; i++)
        {
            std::string fname = "xxx-" + std::to_string(i) + ".gif";
            std::string myfile = dm.absolutePath() + "/" + fname;
            if(files.find(fname) != files.end())
            {
                FILE *junk = fopen(myfile.c_str(), "r");
                if(junk)
                    fclose(junk);
            }
        }
    );

    return 0;
}
