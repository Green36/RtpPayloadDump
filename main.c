// RtpPayloadDump.cpp
//

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>

// 入力ファイル関係のdefine
#define INFILE_ONELINE_MAX 1600
#define RTP_PAYLOAD_OFFSET 50   // 先頭からRTP Payload までのオフセット
#define PACKET_START_CODE  "|0  "

// 出力ファイル関係のdefine
#define OUTPUT_FILE_NAME_LENGTH 30
#define OUTFILE_ONELINE_MAX 41


void create_output_name(char name[25])
{
    time_t now;
    struct tm *ltm;

    time(&now);
    ltm = localtime(&now);

    sprintf(name, "RtpPayloadDump_%02d%02d%02d%02d%02d.txt",
            ltm->tm_mon+1,
            ltm->tm_mday,
            ltm->tm_hour,
            ltm->tm_min,
            ltm->tm_sec);
}

bool within_range(char s)
{
    if( ( s >= 'a' && s <= 'f' ) ||
        ( s >= '0' && s <= '9' ) )
    {
        return true;
    }
    return false;
}

int parse_and_output(char* line_str, FILE* fp_output)
{
    static int output_count = 0;
    static char output_buff[OUTFILE_ONELINE_MAX];

    int i = 0;
    for (;; i++)
    {
        if (line_str + i == NULL) break;
        if (line_str[i] == '\n' ) break;
        if (line_str[i] == '\r') break;
        if (line_str[i] == '|') continue;
        if (isascii(line_str[i]) == 0 ) continue;
        if (!within_range(line_str[i])) continue;

        output_buff[output_count] = line_str[i];
        output_count++;

        if (output_count % 8 == 0)
        {
            output_buff[output_count] = ' ';
            output_count++;
        }
        if (output_count == OUTFILE_ONELINE_MAX - 2)
        {
            output_buff[output_count] = '\n';
            fputs(output_buff, fp_output );
            output_count = 0;
        }

    }
    return 0;
}

int output_rtp_dump(FILE* fp_input, FILE* fp_output)
{
    char read_buff[INFILE_ONELINE_MAX] = { 0x00 };

    while (fgets(read_buff, INFILE_ONELINE_MAX, fp_input))
    {
        if (read_buff[0] == '|')
        {
            parse_and_output(read_buff + RTP_PAYLOAD_OFFSET , fp_output);
        }
        memset(read_buff, 0x00, INFILE_ONELINE_MAX);
    }
    return 0;
}

int main(int arg, char** args)
{
    FILE* fp_output = NULL;
    FILE* fp_input = NULL;

    fp_input = fopen(args[1], "r");
    if (fp_input == NULL)
    {
        printf("open error");
        goto Exit;
    }

    char output_name[OUTPUT_FILE_NAME_LENGTH];
    create_output_name(output_name);
    fp_output = fopen(output_name, "a");
    if (fp_output == NULL)
    {
        printf("open error");
        goto Exit;
    }

    output_rtp_dump(fp_input, fp_output );

Exit:
    if (fp_input)
    {
        fclose(fp_input);
    }
    if (fp_output)
    {
        fclose(fp_output);
    }

    return 0;
}

