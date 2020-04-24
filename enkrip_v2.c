#include <stdio.h>
#include <string.h>
#define SEGMENT 1024 //approximate target size of small file

long file_size(char *name);//function definition below

int main(void)
{
    int segments=0, i, len, accum;
    char filename[260] = {"/home/afif/sisop/shift4/src/small_"};//base name for small files.
    char largeFileName[] = {"/home/afif/sisop/shift4/src/saving.txt"};//change to your path
    char smallFileName[260];
    char line[1080];
    FILE *fp1, *fp2;
    long sizeFile = file_size(largeFileName);
    segments = sizeFile/SEGMENT;//ensure end of file

    fp1 = fopen(largeFileName, "r");
    if(fp1)
    {
        for(i=0;i<segments;i++)
        {
            accum = 0;
            sprintf(smallFileName, "%s%d.txt", filename, i);
            fp2 = fopen(smallFileName, "w");
            if(fp2)
            {
                while(fgets(line, 1080, fp1) && accum <= SEGMENT)
                {
                    accum += strlen(line);//track size of growing file
                    fputs(line, fp2);
                }
                fclose(fp2);
            }
        }
        fclose(fp1);
    }
    return 0;
}

long file_size(char *name)
{
    FILE *fp = fopen(name, "rb"); //must be binary read to get bytes

    long size=-1;
    if(fp)
    {
        fseek (fp, 0, SEEK_END);
        size = ftell(fp)+1;
        fclose(fp);
    }
    return size;
}
