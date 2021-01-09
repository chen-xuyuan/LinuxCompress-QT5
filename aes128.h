#ifndef AES128_H
#define AES128_H

#include <stdio.h>
#include <stdlib.h>

void encryptFile(char* password, char* srcPath, char* destPath);
void decryptFile(char* password, char* srcPath, char* destPath);

#endif // AES128_H
