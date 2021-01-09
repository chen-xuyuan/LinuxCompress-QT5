#include "aes128.h"

#define AES128_subKey subKey

/*全局变量定义*/
FILE * result;
FILE * result2;
long int nFlen;
unsigned char error = 1;
unsigned char p;

unsigned char state[4][4];
//加密过程中的中间状态
unsigned char key[4][4] ={
    0x57,0x69,0x6C,0x6C,0x69,0x61,0x6D,0x53,0x74,0x61,0x6C,0x6C,0x69,0x6E,0x67,0x73
};
//原始密钥,128位,原始密钥是横着排的
unsigned char subKey[11][4][4];
//AES的字密钥共11个，子密钥是竖着排列的，这么做以后方便
unsigned char mText[16] = {
    0x43,0x72,0x79,0x70,
    0x74,0x6F,0x67,0x72,
    0x61,0x70,0x68,0x79,
    0x20,0x61,0x6E,0x64
};
unsigned char mMatrix[4][4];
//AES的S盒
const unsigned char SBox[] = {
    0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5,
    0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
    0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0,
    0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
    0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC,
    0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
    0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A,
    0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
    0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0,
    0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
    0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B,
    0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
    0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85,
    0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
    0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5,
    0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
    0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17,
    0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
    0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88,
    0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
    0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C,
    0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
    0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9,
    0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
    0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6,
    0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
    0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E,
    0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
    0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94,
    0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
    0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68,
    0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
};
const unsigned char one_bit_in8[] = {
    0x01,0x02,0x04,0x08,
    0x10,0x20,0x40,0x80
};

const unsigned int one_bit_in16[] = {
    0x0001,0x0002,0x0004,0x0008,
    0x0010,0x0020,0x0040,0x0080,
    0x0100,0x0200,0x0400,0x0800,
    0x1000,0x2000,0x4000,0x8000
};

const unsigned int px[] = {
    0x011B,0x0236,0x046C,0x08D8,
    0x11B0,0x2360,0x46C0,0x8D80
};

unsigned char RC[16] = {
    0x00,0x01,0x02,0x04,
    0x08,0x10,0x20,0x40,
    0x80,0x1b,0x36,0x6c,
    0xd8,0xb0,0x60,0xc0
    
};/*轮系数RC,使用中直接查表得到*/
char rawString[256];

unsigned char invSBox[256] =

{/* 0    1     2    3     4    5    6   7    8    9    a    b    c    d    e    f  */
    
    0x52,0x09,0x6a,0xd5,0x30,0x36,0xa5,0x38,0xbf,0x40,0xa3,0x9e,0x81,0xf3,0xd7,0xfb,/*0*/
    0x7c,0xe3,0x39,0x82,0x9b,0x2f,0xff,0x87,0x34,0x8e,0x43,0x44,0xc4,0xde,0xe9,0xcb,/*1*/
    0x54,0x7b,0x94,0x32,0xa6,0xc2,0x23,0x3d,0xee,0x4c,0x95,0x0b,0x42,0xfa,0xc3,0x4e,/*2*/
    0x08,0x2e,0xa1,0x66,0x28,0xd9,0x24,0xb2,0x76,0x5b,0xa2,0x49,0x6d,0x8b,0xd1,0x25,/*3*/
    0x72,0xf8,0xf6,0x64,0x86,0x68,0x98,0x16,0xd4,0xa4,0x5c,0xcc,0x5d,0x65,0xb6,0x92,/*4*/
    0x6c,0x70,0x48,0x50,0xfd,0xed,0xb9,0xda,0x5e,0x15,0x46,0x57,0xa7,0x8d,0x9d,0x84,/*5*/
    0x90,0xd8,0xab,0x00,0x8c,0xbc,0xd3,0x0a,0xf7,0xe4,0x58,0x05,0xb8,0xb3,0x45,0x06,/*6*/
    0xd0,0x2c,0x1e,0x8f,0xca,0x3f,0x0f,0x02,0xc1,0xaf,0xbd,0x03,0x01,0x13,0x8a,0x6b,/*7*/
    0x3a,0x91,0x11,0x41,0x4f,0x67,0xdc,0xea,0x97,0xf2,0xcf,0xce,0xf0,0xb4,0xe6,0x73,/*8*/
    0x96,0xac,0x74,0x22,0xe7,0xad,0x35,0x85,0xe2,0xf9,0x37,0xe8,0x1c,0x75,0xdf,0x6e,/*9*/
    0x47,0xf1,0x1a,0x71,0x1d,0x29,0xc5,0x89,0x6f,0xb7,0x62,0x0e,0xaa,0x18,0xbe,0x1b,/*a*/
    0xfc,0x56,0x3e,0x4b,0xc6,0xd2,0x79,0x20,0x9a,0xdb,0xc0,0xfe,0x78,0xcd,0x5a,0xf4,/*b*/
    0x1f,0xdd,0xa8,0x33,0x88,0x07,0xc7,0x31,0xb1,0x12,0x10,0x59,0x27,0x80,0xec,0x5f,/*c*/
    0x60,0x51,0x7f,0xa9,0x19,0xb5,0x4a,0x0d,0x2d,0xe5,0x7a,0x9f,0x93,0xc9,0x9c,0xef,/*d*/
    0xa0,0xe0,0x3b,0x4d,0xae,0x2a,0xf5,0xb0,0xc8,0xeb,0xbb,0x3c,0x83,0x53,0x99,0x61,/*e*/
    0x17,0x2b,0x04,0x7e,0xba,0x77,0xd6,0x26,0xe1,0x69,0x14,0x63,0x55,0x21,0x0c,0x7d /*f*/
};
//AES的S盒的逆变换

//以下是函数定义
void writeResult();

void transferText2Matrix()
{
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            mMatrix[j][i] = mText[i*4+j];
        }
    }
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            state[i][j] = mMatrix[i][j];
        }
    }
}

void invByteSub()
{
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            unsigned char index = state[i][j];
            state[i][j] = invSBox[index];
        }
    }
}

void ByteSub()
{
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            unsigned char index = state[i][j];
            state[i][j] = SBox[index];
        }
    }
}
unsigned int GF28_multiplication(unsigned char ax,unsigned char bx)
{
    unsigned int result = 0x0000;
    unsigned char bitE[8] = {};
    unsigned int bxR[8] = {};
    for (int i = 0; i < 8; ++i)/*建立表*/
    {
        bitE[i] = ((ax&one_bit_in8[i]) == (one_bit_in8[i]));
        /*判断ax的每一位，并存在数组中，便于下面做x乘法使用*/
        
        bxR[i] = bx<<i;
        /*构造表达式bx的移位结果，向右移位*/
    }
    for (int i = 0; i < 8; ++i)
    {
        if (bitE[i] == 1)
        {
            result = result ^ bxR[i];
        }
    }
    return result;
}

unsigned char GF28_modPx(unsigned int input)
{
    unsigned char result = 0x00;
    unsigned int ax = input;
    unsigned char bitE[16] = {};
    for (int i = 8; i < 16; ++i)/*这里建立表只需要从8开始*/
    {
        bitE[i] = ((input & one_bit_in16[i]) == one_bit_in16[i]);
    }
    for (int i = 15; i > 7; --i)
    {
        if (ax < 256)
        {
            //printf("__\n");
            break;
        }
        if (bitE[i])
        {
            ax = ax ^ px[i-8];
        }
    }
    result = ax;
    return result;
}

void ShiftRow()
{
    unsigned char swap[4][4];
    for(int i = 0;i < 4 ;++i)
    {
        swap[0][i] = state[0][i];
    }
    swap[1][0] = state[1][1];
    swap[1][1] = state[1][2];
    swap[1][2] = state[1][3];
    swap[1][3] = state[1][0];
    
    swap[2][0] = state[2][2];
    swap[2][1] = state[2][3];
    swap[2][2] = state[2][0];
    swap[2][3] = state[2][1];
    
    swap[3][0] = state[3][3];
    swap[3][1] = state[3][0];
    swap[3][2] = state[3][1];
    swap[3][3] = state[3][2];
    for (int i = 0; i < 4; ++i)
    {
        for (int j  = 0; j < 4; ++j)
        {
            state[i][j] = swap[i][j];
        }
    }
}

void invShiftRow()
{
    unsigned char swap[4][4];
    for(int i = 0;i < 4 ;++i)
    {
        swap[0][i] = state[0][i];
    }
    swap[1][0] = state[1][3];
    swap[1][1] = state[1][0];
    swap[1][2] = state[1][1];
    swap[1][3] = state[1][2];
    
    swap[2][0] = state[2][2];
    swap[2][1] = state[2][3];
    swap[2][2] = state[2][0];
    swap[2][3] = state[2][1];
    
    swap[3][0] = state[3][1];
    swap[3][1] = state[3][2];
    swap[3][2] = state[3][3];
    swap[3][3] = state[3][0];
    for (int i = 0; i < 4; ++i)
    {
        for (int j  = 0; j < 4; ++j)
        {
            state[i][j] = swap[i][j];
        }
    }
}


unsigned char GF28_multipyWithModPx(unsigned char ax,unsigned char bx)
{
    unsigned int a = GF28_multiplication(ax, bx);
    unsigned char result = GF28_modPx(a);
    return result;
}

void MixColumn()//这个版本貌似没问题
{
    unsigned char swap[4][4];
    for (int i = 0; i <4; ++i) {
        swap[0][i] =
        GF28_multipyWithModPx(0x02, state[0][i])^
        GF28_multipyWithModPx(0x03, state[1][i])^
        GF28_multipyWithModPx(0x01, state[2][i])^
        GF28_multipyWithModPx(0x01, state[3][i]);
        swap[1][i] =
        GF28_multipyWithModPx(0x01, state[0][i])^
        GF28_multipyWithModPx(0x02, state[1][i])^
        GF28_multipyWithModPx(0x03, state[2][i])^
        GF28_multipyWithModPx(0x01, state[3][i]);
        swap[2][i] =
        GF28_multipyWithModPx(0x01, state[0][i])^
        GF28_multipyWithModPx(0x01, state[1][i])^
        GF28_multipyWithModPx(0x02, state[2][i])^
        GF28_multipyWithModPx(0x03, state[3][i]);
        swap[3][i] =
        GF28_multipyWithModPx(0x03, state[0][i])^
        GF28_multipyWithModPx(0x01, state[1][i])^
        GF28_multipyWithModPx(0x01, state[2][i])^
        GF28_multipyWithModPx(0x02, state[3][i]);
    }
    for (int i = 0; i < 4; ++i) {
        for (int j  = 0; j < 4; ++j) {
            state[i][j] = swap[i][j];
        }
    }
}

void invMixColumn()//这个版本貌似没问题
{
    unsigned char swap[4][4];
    for (int i = 0; i <4; ++i) {
        swap[0][i] =
        GF28_multipyWithModPx(0x0e, state[0][i])^
        GF28_multipyWithModPx(0x0b, state[1][i])^
        GF28_multipyWithModPx(0x0d, state[2][i])^
        GF28_multipyWithModPx(0x09, state[3][i]);
        swap[1][i] =
        GF28_multipyWithModPx(0x09, state[0][i])^
        GF28_multipyWithModPx(0x0e, state[1][i])^
        GF28_multipyWithModPx(0x0b, state[2][i])^
        GF28_multipyWithModPx(0x0d, state[3][i]);
        swap[2][i] =
        GF28_multipyWithModPx(0x0d, state[0][i])^
        GF28_multipyWithModPx(0x09, state[1][i])^
        GF28_multipyWithModPx(0x0e, state[2][i])^
        GF28_multipyWithModPx(0x0b, state[3][i]);
        swap[3][i] =
        GF28_multipyWithModPx(0x0b, state[0][i])^
        GF28_multipyWithModPx(0x0d, state[1][i])^
        GF28_multipyWithModPx(0x09, state[2][i])^
        GF28_multipyWithModPx(0x0e, state[3][i]);
    }
    for (int i = 0; i < 4; ++i) {
        for (int j  = 0; j < 4; ++j) {
            state[i][j] = swap[i][j];
        }
    }
}

void generateSubkey()
{
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            subKey[0][i][j] = key[j][i];
        }
    }
    //初始密钥赋值完成，这一步的操作是把密钥竖过来
    //int m = 1;//假定的轮数
    //for起始
    for (int m = 1; m < 11; ++m) {
        unsigned char swap[4];//临时变量
        swap[0] = subKey[m-1][1][3];
        swap[1] = subKey[m-1][2][3];
        swap[2] = subKey[m-1][3][3];
        swap[3] = subKey[m-1][0][3];
        //swap 做s盒替换
        for (int i = 0; i <4 ; ++i) {
            swap[i] = SBox[swap[i]];
        }
        swap[0] = swap[0] ^ RC[m];
        //和上一轮密钥异或
        for (int i = 0; i < 4; ++i) {
            swap[i] = swap[i] ^ subKey[m-1][i][0];
        }
        for (int i = 0; i < 4; ++i) {
            subKey[m][i][0] = swap[i];
        }
        for (int x = 1; x < 4; ++x) {
            for (int i = 0; i < 4; ++i) {
                subKey[m][i][x] = subKey[m][i][x-1] ^ subKey[m-1][i][x];
            }
        }
    }
}

void roundKeyAddition(int m)
{
    //m为轮数
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; j++) {
            state[i][j] = state[i][j] ^ subKey[m][i][j];
        }
    }
}
void getSize(const char filePointer[])//获得文件大小，单位：字节
{
    FILE * ff;
    if ((ff = fopen(filePointer,"r")) == NULL)
    {
        printf("KeyFile NOT Exist!\n");
        exit(0);
    }
    fseek(ff,0,SEEK_END);
    nFlen = ftell(ff);
    printf("File Size: %ld Byte\n", nFlen);
    //int x = nFlen%16;
    //fputc(result,x);
    fclose(ff);
}
void AES128_E()
{
    //初始化过程
    generateSubkey();//预计算子密钥，最先执行
    transferText2Matrix();
    roundKeyAddition(0);//初始化
    //初始化完成
    //下面是第一轮加密
    
    
    
    ByteSub();
    ShiftRow();
    MixColumn();
    roundKeyAddition(1);
    //第一轮加密，结果正确
    //下面进行2-9轮加密
    for (int i = 2; i < 10; ++i) {
        ByteSub();
        ShiftRow();
        MixColumn();
        roundKeyAddition(i);
    }
    //2-9轮加密完成，结果正确
    //下面进行最后一轮加密，第10轮加密没有列混合
    ByteSub();
    ShiftRow();
    roundKeyAddition(10);
}

void writeResult()
{
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            int a = (int)state[j][i];
            //矩阵是竖着排列的
            fputc(a, result);
        }
    }
}

void AES128_D()
{
    generateSubkey();
    /*首先对第10轮解密*/
    roundKeyAddition(10);
    invShiftRow();
    invByteSub();
    
    /*第十轮解密完成*/
    
    for (int i = 9; i >= 1; --i) {
        //        ByteSub();
        //        ShiftRow();
        //        MixColumn();
        //        roundKeyAddition(i);
        //以上是加密过程，解密反着来即可
        roundKeyAddition(i);
        invMixColumn();
        invShiftRow();
        invByteSub();
    }
    //以上完成了9-1轮的解密
    //最后一轮的密钥加法层
    roundKeyAddition(0);
}

void processFile_E(const char filePointer[])
{
    // printf(">");
    // int xx = 1;
    // 拥有变量：结果文件指针：result，文件长度nFlen，文件路径filePointer[]
    FILE * fileE = fopen(filePointer,"r");
    if(fileE == NULL){
        printf("Target File NOT EXIST! ");
        exit(0);
    }
  
    long int lastRound = nFlen - (nFlen%16);
    for(long int i = 0;i < lastRound ;++i)
    {
        int index = i % 16;
        if((index == 0&&(i != 0))){
            AES128_E();
            writeResult();
        }
        mText[index] = fgetc(fileE);
    }
    AES128_E();
    writeResult();//最后一组规则的加密
    unsigned char lastIndex = nFlen%16;
    for(int i = lastIndex;i < 16;++i)
    {
        mText[i] = 0;
    }
    for(int i = 0;i < lastIndex;++i)
    {
        mText[i] = fgetc(fileE);
    }
    AES128_E();
    if(lastIndex){
        writeResult();
    }
    fputc(lastIndex,result);
    fclose(fileE);

}

void processFile_D(const char filePointer[])
{

    FILE * fileE = fopen(filePointer,"r");
    if(fileE == NULL){
        printf("Target File NOT EXIST! ");
        exit(0);
    }
    if((nFlen%16) != 1)
    {
        printf("Invalid Target File!\n");
        exit(0);
    }
    long int round = nFlen / 16;
    for(int i = 0;i < round; ++i)
    {
        for(int x = 0;x < 4 ;++x){
            for(int y = 0; y < 4;++y)
            {
                state[y][x] = fgetc(fileE);
            }
        }
        AES128_D();
        if(i != round - 1)
            writeResult();
    }
    int ltt = fgetc(fileE);
    if(ltt == 0)
    {
        writeResult();
        fclose(fileE);
        return;
    }
    unsigned char last[16];
    unsigned char xx = 0;
    for(int i = 0;i < 4;++i)
    {
        for(int j = 0; j < 4 ;++j)
        {
            last[xx++] = state[j][i];
        }
    }
    for(int i = 0;i < ltt;++i)
    {
        fputc(last[i],result);
    }
    
     fclose(fileE);
}

void encryptFile(char* password, char* srcPath, char* destPath)
{
    error = 0;
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            key[i][j] = password[i*4+j];
        }
    }
    result = fopen(destPath, "w");
    getSize(srcPath);
    processFile_E(srcPath);
    fclose(result);
}

void decryptFile(char* password, char* srcPath, char* destPath)
{
    error = 0;
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            key[i][j] = password[i * 4 + j];
        }
    }
    result = fopen(destPath, "w");
    getSize(srcPath);
    processFile_D(srcPath);
    fclose(result);
}
