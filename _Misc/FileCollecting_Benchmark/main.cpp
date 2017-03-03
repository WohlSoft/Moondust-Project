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
    fflush(stdout);\
}


int main(int argc, char **argv)
{
    DirMan dm("dummy");
    if(dm.exists())
        dm.rmpath(dm.absolutePath());
    dm.mkAbsDir(dm.absolutePath());

    printf("== Preparing... ==\n");
    fflush(stdout);

    int existingFiles = 10000;
    if(argc > 1)
        existingFiles = atoi(argv[1]);
    {
        for(int i = 0; i < existingFiles; i++)
        {
            std::string myfile = dm.absolutePath() + "/xxx-" + std::to_string(i) + ".gif";
            FILE *junk = fopen(myfile.c_str(), "w");
            fprintf(junk, "Just a test!");
            fclose(junk);
        }
    }

    printf("== Testing %d existing files... ==\n", existingFiles);
    fflush(stdout);

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

    printf("== Clean-up... ==\n");
    fflush(stdout);

    dm.rmpath(dm.absolutePath());

    printf("== Benchmark completed! ==\n");
    fflush(stdout);

    return 0;
}
