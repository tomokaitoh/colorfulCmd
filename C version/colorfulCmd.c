#include <stdio.h>
#include <windows.h>
#include <stdlib.h>

#define BITMAP_FILE_PATH "furina.bmp"  // Bitmap文件的路径
#define CHARACTER "#"  // 在控制台用来显示图片的字符

struct Pixel {
    unsigned char B,G,R;
};

struct Pixel *pixels = NULL;    // 图片的所有像素
int width, height;              // 图片的 宽度 和 高度


// 开启Windows的虚拟终端序列支持
int enableVTMode() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
        return -1;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
        return -2;

    dwMode |= 0x0004;
    if (!SetConsoleMode(hOut, dwMode))
        return -3;
    return 1;
}


// 读取Bitmap图片
int readBitmap(const char* filePath) {
    FILE *file = fopen(filePath, "rb");
    if(file == NULL)
        return -1;

    // 读取Bitmap的文件头部分
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;
    fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, file);
    fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, file);
    width = infoHeader.biWidth;
    height = infoHeader.biHeight;

    // 读取Bitmap图片的数据
    pixels = malloc(sizeof(struct Pixel) * width * height);
    fread(pixels, sizeof(struct Pixel), width*height, file);

    // Bitmap图片的行顺序是上下颠倒的，这里把它颠倒回来
    struct Pixel* temp = malloc(sizeof(struct Pixel) * width * height);
    for (int y = 0; y < height; y++)
        memcpy(temp+y*width, pixels+(height-y-1)*width, width*sizeof(struct Pixel));
    free(pixels);
    pixels = temp;

    fclose(file);
    return 1;
}

int main(int argc, char *argv[]){

    // 开启Windows的虚拟终端序列支持
    if(enableVTMode() < 0){
        printf("终端不支持ANSI转义序列.");
        return -1;
    }

    // 读取Bitmap图片数据
    if(readBitmap(BITMAP_FILE_PATH) < 0){
        printf("文件 '" BITMAP_FILE_PATH "' 打开失败！");
        return -1;
    }

    system("cls");  // 控制台清屏

    for(int y = 0; y < height; ++y) {
        for(int x = 0; x < width; ++x) {
            struct Pixel pixel = pixels[x + y*width];  // 获取图片中 (x, y) 点的像素值

            // printf("\033[5m");  // 闪烁效果
            // printf("\033[1m");  // 高亮度
            // printf("\033[48;2;%d;%d;%dm", pixel.R, pixel.G, pixel.B);  // 设置文本背景色

            printf("\033[38;2;%d;%d;%dm", pixel.R, pixel.G, pixel.B);  // 设置前景色(文本颜色)

            printf(CHARACTER); // 打印字符
        }
        putchar('\n');
    }

    // 清除文字样式
    printf("\033[0m");

    free(pixels);

    system("pause");
    return 0;
}
