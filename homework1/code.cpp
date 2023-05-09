#include <bits/stdc++.h>
#include <direct.h>
#include <Windows.h>

using namespace std;

class BMP 
{
private:
    typedef struct { // to store origin RGB picture data
        BYTE blue, green, red;
    } BGR;
    typedef struct { // to store changed YUV picture data
        BYTE Y, U, V;
    } YUV;

    string path; // path to the origin picture
    long width, height; // picture size
    BITMAPFILEHEADER fileHead; // file's information
    BITMAPINFOHEADER infoHead; // bmp's information
    RGBQUAD Pla[256]; // store the pla
    BGR *originRGBBuf; // store the origin picture
    BGR *grayYUVPicBuf; // store the gray YUV picture to output
    BGR *newRGBPicBuf; // store the rgb picture to output
    YUV *newYUVBuf; // store the YUV from the origin and after being changed

public:
    BMP(string bmp_path) // initialize
    {
        path = bmp_path;
    }
    
    void read()  // read the bmp
    {
        FILE *fp = fopen(path.c_str(), "rb");
        if (!fp) {
            cout << "fail when open the original bmp\n";
            exit(1);
        }

        fread(&fileHead, sizeof(BITMAPFILEHEADER), 1, fp);
        if (0x4d42 != fileHead.bfType) {
            cout << "origin is not a bmp\n";
            exit(1);
        }

        fread(&infoHead, sizeof(BITMAPINFOHEADER), 1, fp);
        for (int i = 0; i < 256; i ++) {
            fread(&Pla[i].rgbBlue, sizeof(BYTE), 1, fp);
            fread(&Pla[i].rgbGreen, sizeof(BYTE), 1, fp);
            fread(&Pla[i].rgbRed, sizeof(BYTE), 1, fp);
            fread(&Pla[i].rgbReserved, sizeof(BYTE), 1, fp);
        }

        width = infoHead.biWidth;
        height = infoHead.biHeight;
        // cout << width << ' ' << height << '\n';

        fseek(fp, fileHead.bfOffBits, 0);
        originRGBBuf = new BGR[width * height];
        fread(originRGBBuf, sizeof(BGR), width * height, fp);
        
        fclose(fp);
    }

    void RGB2YUV() // change picture saved by RGB to YUV
    {
        grayYUVPicBuf = new BGR[width * height];
        newYUVBuf = new YUV[width * height];
        for (int i = 0; i < width * height; i ++) {
            newYUVBuf[i].Y = ((77 * originRGBBuf[i].red + 150 * originRGBBuf[i].green + 29 * originRGBBuf[i].blue)>>8);
            newYUVBuf[i].U = ((-44 * originRGBBuf[i].red - 87 * originRGBBuf[i].green + 131 * originRGBBuf[i].blue)>>8) + 128;
            newYUVBuf[i].V = ((131 * originRGBBuf[i].red - 110 * originRGBBuf[i].green - 21 * originRGBBuf[i].blue)>>8) + 128;

            grayYUVPicBuf[i].blue = newYUVBuf[i].Y;
            grayYUVPicBuf[i].green = newYUVBuf[i].Y;
            grayYUVPicBuf[i].red = newYUVBuf[i].Y;
        }
    }

    void YUV2RGB() // rechange picture saved by YUV to RGB
    {
        newRGBPicBuf = new BGR[width * height];
        for (int i = 0; i < width * height; i ++) {
            newRGBPicBuf[i].red = newYUVBuf[i].Y + ((360 * (newYUVBuf[i].V - 128))>>8);
            newRGBPicBuf[i].green = newYUVBuf[i].Y - ((( 88 * (newYUVBuf[i].U - 128) + 184 * (newYUVBuf[i].V - 128)))>>8);
            newRGBPicBuf[i].blue = newYUVBuf[i].Y +((455 * (newYUVBuf[i].U - 128))>>8);
        }
    }

    void changeY(int yNum) // change the number of Y in newYUVBuf and according grayYUVPicBuf
    {
        for (int i = 0; i < width * height; i ++) {
            newYUVBuf[i].Y += yNum;
            if (newYUVBuf[i].Y < 0) newYUVBuf[i].Y = 0;
            if (newYUVBuf[i].Y > 255) newYUVBuf[i].Y = 255;

            grayYUVPicBuf[i].blue = newYUVBuf[i].Y;
            grayYUVPicBuf[i].green = newYUVBuf[i].Y;
            grayYUVPicBuf[i].red = newYUVBuf[i].Y;
        }
    }

    void writeGray(string save_path) // output the results stored in grayYUVPicBuf
    {
        FILE *fp;
        fp = fopen(save_path.c_str(), "wb");
        if (!fp) {
            cout << "fail to open path to save\n";
            exit(1);
        }
        fwrite(&fileHead, sizeof(BITMAPFILEHEADER), 1, fp);
        fwrite(&infoHead, sizeof(BITMAPINFOHEADER), 1, fp);

        for (int i = 0; i < 256; i ++) {
            fwrite(&Pla[i].rgbBlue, sizeof(BYTE), 1, fp);
            fwrite(&Pla[i].rgbGreen, sizeof(BYTE), 1, fp);
            fwrite(&Pla[i].rgbRed, sizeof(BYTE), 1, fp);
            fwrite(&Pla[i].rgbReserved, sizeof(BYTE), 1, fp);
        }
        fseek(fp, fileHead.bfOffBits, 0);
        fwrite(grayYUVPicBuf, sizeof(BGR), width * height, fp);

        fclose(fp);
    }

    void writeRGB(string save_path) // output the results stored in newRGBPicBuf
    {
        FILE *fp;
        fp = fopen(save_path.c_str(), "wb");
        if (!fp) {
            cout << "fail to open path to save\n";
            exit(1);
        }
        fwrite(&fileHead, sizeof(BITMAPFILEHEADER), 1, fp);
        fwrite(&infoHead, sizeof(BITMAPINFOHEADER), 1, fp);

        for (int i = 0; i < 256; i ++) {
            fwrite(&Pla[i].rgbBlue, sizeof(BYTE), 1, fp);
            fwrite(&Pla[i].rgbGreen, sizeof(BYTE), 1, fp);
            fwrite(&Pla[i].rgbRed, sizeof(BYTE), 1, fp);
            fwrite(&Pla[i].rgbReserved, sizeof(BYTE), 1, fp);
        }
        fseek(fp, fileHead.bfOffBits, 0);
        fwrite(newRGBPicBuf, sizeof(BGR), width * height, fp);

        fclose(fp);
    }
    
    void test() { // test something
        cout << "old: " << (int)originRGBBuf[300].blue << ' ' << (int)originRGBBuf[300].green << ' ' << (int)originRGBBuf[300].red << '\n'\
             << "new: " << (int)newRGBPicBuf[300].blue << ' ' << (int)newRGBPicBuf[300].green << ' ' << (int)newRGBPicBuf[300].red << '\n';
    }
};

void solve(int num = 100) {
    mkdir("./results"); // where to save results

    BMP bmp("origin.bmp"); // initialize a class for actions
    bmp.read(); // read the origin picture

    bmp.RGB2YUV(); // change the RGB picture to YUV

    bmp.YUV2RGB(); // test if the read and change is correct
    bmp.writeRGB("results/rgbOrigin.bmp"); // output the origin picture
    
    bmp.writeGray("results/grayOrigin.bmp"); // output the gray picture from the origin picture

    bmp.changeY(num); // change value of Y
    bmp.writeGray("results/grayAfterChange.bmp"); // output the gray picture after change

    bmp.YUV2RGB(); // rechange the after-changed YUV to RGB
    bmp.writeRGB("results/rgbAfterChange.bmp"); // output the RGB picture after changing Y
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        string number = argv[1];
        solve(stoi(number));
    } else {
        solve();
    }
}