#include <stdio.h>
#include <signal.h>
#include <stdint.h>
#include <windows.h>
#include <process.h>

#define BUFFER_MAX 8192

static char g_nbstdin_buffer[2][BUFFER_MAX];

static HANDLE g_input[2];
static HANDLE g_process[2];

static int g_is_working = TRUE;

typedef struct ShareBlock_t
{
    uint8_t BufferA[BUFFER_MAX];
    uint8_t BufferB[BUFFER_MAX];
} ShareBlock;

static DWORD WINAPI console_input(LPVOID lpParameter)
{
    while(1)
    {
        int i;
        for(i = 0; i < 2; i++)
        {
            fgets(g_nbstdin_buffer[i], BUFFER_MAX, stdin);
            SetEvent(g_input[i]);
            WaitForSingleObject(g_process[i], INFINITE);
        }
    }
    return 0;
}

static void create_nbstdin()
{
    int i;
    DWORD tid;
    CreateThread(NULL, 1024, &console_input, 0, 0, &tid);
    for(i = 0; i < 2; i++)
    {
        g_input[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        g_process[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        g_nbstdin_buffer[i][0] = '\0';
    }
}

static const char *nbstdin()
{
    DWORD n = WaitForMultipleObjects(2, g_input, FALSE, 0);
    if(n == WAIT_OBJECT_0 || n == WAIT_OBJECT_0 + 1)
    {
        n = n - WAIT_OBJECT_0;
        SetEvent(g_process[n]);
        return g_nbstdin_buffer[n];
    }
    else
        return 0;
}

static void caughtSignal(int sig)
{
    if(sig == SIGINT)
    {
        g_is_working = FALSE;
    }
}


int main()
{
    char OutputBuffer[8192];
    char Buffer[2048];
    short MsgLen, MsgLenI;
    HANDLE hMap = NULL;
    HANDLE hAddress = NULL;
    ShareBlock *pBlock = NULL;
    int flag = 0;

    g_is_working = TRUE;
    signal(SIGINT,  &caughtSignal);
    signal(SIGTERM,  &caughtSignal);

    printf("WAITING\n");
    fflush(stdout);

    while(flag == 0)
    {
        if(hMap == NULL)
            hMap = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, "smbx_memory_block");

        if(hMap != NULL)
        {
            if(hAddress == NULL)
                hAddress = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);

            if(hAddress != NULL)
            {
                pBlock = (ShareBlock *)hAddress;
                flag = 1;
            }
        }

        if(flag == 0)
            Sleep(300);

        if(!g_is_working)
        {
            printf("TERMINATED\n");
            fflush(stdout);
            return 0; // Terminated
        }
    }

    printf("READY\n");
    fflush(stdout);

    create_nbstdin();

    while(g_is_working)
    {
        const char *line = nbstdin();

        if(line)
        {
            memcpy(&MsgLenI, &(pBlock->BufferB[0]), 2);
            MsgLen = (short)strlen(line);

            if(MsgLen > 0)
            {
                if(MsgLenI == 0)
                {
                    memcpy(&(pBlock->BufferB[0]), &MsgLen, 2);
                    memcpy(&(pBlock->BufferB[2]), &line[0], MsgLen);
                }
                else
                {
                    pBlock->BufferB[2 + MsgLenI] = '\n';
                    memcpy(&(pBlock->BufferB[3 + MsgLenI]), &line[0], MsgLen);
                    MsgLen += (1 + MsgLenI);
                    memcpy(&(pBlock->BufferB[0]), &MsgLen, 2);
                }
            }
        }
        else
        {
            memcpy(&MsgLen, &(pBlock->BufferA[0]), 2);
            if(MsgLen > 0 && MsgLen < 8000)
            {
                memcpy(&OutputBuffer[0], &(pBlock->BufferA[2]), MsgLen);
                OutputBuffer[MsgLen] = 0;

                printf("INPUT:%s\n", OutputBuffer);
                fflush(stdout);

                memset(&(pBlock->BufferA[0]), 0, 8192);
            }
            Sleep(1);
        }
    }

    printf("TERMINATED\n");
    fflush(stdout);

    UnmapViewOfFile(hAddress);
    CloseHandle(hMap);
}
