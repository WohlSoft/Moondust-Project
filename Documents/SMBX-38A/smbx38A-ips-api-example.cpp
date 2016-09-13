#include <stdio.h>
#include <tchar.h>
#include <stdlib.h>
#include <windows.h>
#include <process.h>
#define BUFFER_MAX 8192
char g_nbstdin_buffer[2][BUFFER_MAX];
HANDLE g_input[2];
HANDLE g_process[2];
struct ShareBlock
{
	unsigned char BufferA[BUFFER_MAX];
	unsigned char BufferB[BUFFER_MAX];
};
DWORD WINAPI console_input(LPVOID lpParameter)
{
    for (;;) 
	{
        int i;
        for (i=0;i<2;i++) 
		{
            fgets(g_nbstdin_buffer[i],BUFFER_MAX,stdin);
            SetEvent(g_input[i]);
            WaitForSingleObject(g_process[i],INFINITE);
        }
    }
    return 0;
}

void create_nbstdin()
{
    int i;
    DWORD tid;
    CreateThread(NULL,1024,&console_input,0,0,&tid);
    for (i=0;i<2;i++) 
	{
        g_input[i]=CreateEvent(NULL,FALSE,FALSE,NULL);
        g_process[i]=CreateEvent(NULL,FALSE,FALSE,NULL);
        g_nbstdin_buffer[i][0]='\0';
    }
}
const char* nbstdin()
{
    DWORD n=WaitForMultipleObjects(2,g_input,FALSE,0);
    if (n==WAIT_OBJECT_0 || n==WAIT_OBJECT_0+1) 
	{
        n=n-WAIT_OBJECT_0;
        SetEvent(g_process[n]);
        return g_nbstdin_buffer[n];
    }
    else 
	{
        return 0;
    }
}
int _tmain(int argc, _TCHAR* argv[])
{
	char OutputBuffer[8192];
	char Buffer[2048];
	short MsgLen, MsgLenI;
	HANDLE hMap = NULL;
    HANDLE hAddress = NULL;
	struct ShareBlock *pBlock;
	int Flag = 0;
	printf("Please open smbx.exe with command line.\n");
	printf("Waiting...\n");
	while (Flag == 0)
	{
		if (hMap == NULL) hMap = OpenFileMappingA(FILE_MAP_ALL_ACCESS, false, "smbx_memory_block");
		if (hMap != NULL)
		{
			if (hAddress == NULL) hAddress = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
			if (hAddress != NULL)
			{
				pBlock = (ShareBlock*)hAddress;
				Flag = 1;
			}
		}
		if (Flag == 0)
		{
			Sleep(1000);
		}
	}
	printf("Done!\n");
	create_nbstdin();
    for (;;) {
        const char *line=nbstdin();
        if (line) 
		{
			memcpy(&MsgLenI ,&(pBlock->BufferB[0]), 2);
			MsgLen = (short)strlen(line);
			if (MsgLen > 0)
			{
				if (MsgLenI == 0)
				{
					memcpy(&(pBlock->BufferB[0]), &MsgLen, 2);
					memcpy(&(pBlock->BufferB[2]), &line[0], MsgLen);
				}
				else
				{
					pBlock->BufferB[2+MsgLenI] = '\n';
					memcpy(&(pBlock->BufferB[3+MsgLenI]), &line[0], MsgLen);
					MsgLen += (1 + MsgLenI);
					memcpy(&(pBlock->BufferB[0]), &MsgLen, 2);
				}
			}
        }
        else 
		{
            memcpy(&MsgLen ,&(pBlock->BufferA[0]), 2);
			if (MsgLen > 0 && MsgLen < 8000)
			{
				memcpy(&OutputBuffer[0], &(pBlock->BufferA[2]), MsgLen);
				OutputBuffer[MsgLen] = 0;
				printf(">smbx.exe:\n%s\n", OutputBuffer);
				memset(&(pBlock->BufferA[0]), 0, 8192);
			}
			Sleep(0);
        }
    }
}