#include "io.h"

#define FB_black 0
#define FB_bLUE 1
#define FB_green 3
#define FB_cyan 4
#define FB_red 5
#define FB_magenta 6
#define FB_brown 7
#define FB_lightgrey 8
#define FB_lightblue 9
#define FB_lightgreen 10
#define FB_lightcyan 11
#define FB_lightred 12
#define FB_lightmagenta 13
#define FB_lightbrown 14
#define FB_white 15

char *fb = (char *) 0x000B8000;
char *dataMarkerStart = (char*) 0x00200000;
char *dataMarkerEnd = (char*) 0x010FF7FE;
char *dataRealStart = (char*) 0x010FF7FF;
char *dataRealEnd = (char*) 0x01FFEFFE;


/**   
     *  @param i  The location in the framebuffer
     *  @param c  The character
     *  @param fg The foreground color
     *  @param bg The background color
     */
void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg)
    {
        fb[i] = c;
        fb[i + 1] = ((fg & 0x0F) << 4) | (bg & 0x0F);
    }
#define FB_COMMAND_PORT         0x3D4
#define FB_DATA_PORT            0x3D5    
#define FB_HIGH_BYTE_COMMAND    14
#define FB_LOW_BYTE_COMMAND     15

/**
 * @param short
 */

void fb_move_cursor(unsigned short pos)
    {
        outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
        outb(FB_DATA_PORT,    ((pos >> 8) & 0x00FF));
        outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
        outb(FB_DATA_PORT,    pos & 0x00FF);
    }

void fb_write_string(int offset, char* s, int length)
{
    int i;
    for (i = 0; i<length; i = i+1)
    {
        fb_write_cell(offset + i*2,s[i],FB_red,FB_lightgrey);
    }
}

void fb_clear()
{
    for(int i = 0; i<80*25;i++)
    {
        fb_write_cell(i*2,' ',FB_black,FB_black);
    }
}

int isMemoryOpen(char* baseAddress, int size)
{
    for(char* c = baseAddress; c <(baseAddress+size); c++)
    {
        if(*c == 1)
        {
            return 0;
        }
    }
    return 1;
}

void markMemory(char* baseAdress, int size)
{
    for(char* c = baseAdress; c< baseAdress+size; c++)
    {
        *c = 1;
    }
}

char* getMem(int numBytes)
{
    int dataOffset = (int)dataRealStart - (int)dataMarkerStart;
    for(char* c = dataMarkerStart; c<= dataMarkerEnd; c++)
    {
        if(*c == 0)
        {
            if(isMemoryOpen(c, numBytes))
            {
                markMemory(c, numBytes);
                return c+ dataOffset;
            }

        }

    }
    return 0x0;
}

void unmarkUnused()
{
    for(char* c = dataMarkerStart; c<= dataMarkerEnd; c++)
    {
        *c = 0;
    }
}

void freeMemory(char* baseAddres, int numBytes)
{
    for(int i = 0; i < numBytes; i++)
    {
        *(baseAddres + i)= 0;
    }
}

void strcpy(char* dest,char* src, int length)
{
    for(int i = 0; i<length; i++)
    {
        dest[i] = src[i];
    }
}

int main()
{
    
    unmarkUnused();

    //fb_write_cell(0,'F', FB_cyan,FB_lightgreen);
    //fb_move_cursor(0x0800);

    char* s;

    fb_clear();

    for(int i = 0; i<25; i++)
    {
        s = getMem(5);
        strcpy(s,"hello", 5);
        fb_write_string(80*2*i,s,5);
        //freeMemory(s,5);
    }
    
    
    

    return 0;
    
}