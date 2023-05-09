#include <bits/stdc++.h>
#include <direct.h>
#include <Windows.h>

using namespace std;

class BMP 
{

// variables
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
    BITMAPFILEHEADER biFileHead; // binary file's information
    BITMAPINFOHEADER biInfoHead; // binary bmp's information
    RGBQUAD Pla[256]; // store the pla
    RGBQUAD biPla[256]; // store the pla of binary bmp
    BGR *originRGBBuf; // store the origin RGB picture
    BYTE *originBiBuf; // store the origin Bi picture
    BGR *grayYUVPicBuf; // store the gray YUV picture to output
    BGR *newRGBPicBuf; // store the rgb picture to output
    YUV *newYUVBuf; // store the YUV from the origin and after being changed
    BYTE *biBuf;

// private functions
private:
    void binarizeBlock(BYTE *startPoint, int blockHeight, int blockWidth) { // a subfunction
        int N = blockHeight * blockWidth; // number of all
        int Nf = 0, Nb = 0; // number of front and back
        int Vf = 0, Vb = 0; // sum of front and back
        int minValue = 255, maxValue = 0;
        int count[256];
        memset(count, 0, sizeof(int) * 256);

        for (int i = 0; i < blockHeight; i ++) { // count all number and find min and max
            for (int j = 0; j < blockWidth; j ++) {
                minValue = min(minValue, (int)startPoint[i * width + j]);
                maxValue = max(maxValue, (int)startPoint[i * width + j]);
                count[(int)startPoint[i * width + j]] ++; // count 
            }
        }
        int threshold = minValue; // final threshold
        double rank = 0; // how to judge
        for (int value = minValue; value <= maxValue; value ++) { // initialize
            Nb += count[value];
            Vb += count[value] * value;
        }
        assert(Nb == N);

        for (int temphold = minValue; temphold <= maxValue; temphold ++) {
            Nf += count[temphold]; Nb -= count[temphold];
            Vf += count[temphold] * temphold; Vb -= count[temphold] * temphold; // change state

            double wf = (double)Nf / N, wb = (double)Nb / N; // weight of each
            double muf = (double)Vf / Nf, mub = (double)Vb / Nb; // average of each
            double mu = wf * muf + wb * mub; // average of all
            double tempRank = wf * pow(muf - mu, 2) + wb * pow(mub - mu, 2); // calculate the between value

            if (tempRank >= rank) { // update threshold
                rank = tempRank;
                threshold = temphold;
                // cout << rank << ' ' << threshold << endl;
            }
        }
        // cout << threshold << endl;

        for (int i = 0; i < blockHeight; i ++) {
            for (int j = 0; j < blockWidth; j ++) {
                startPoint[i * width + j] = startPoint[i * width + j] > threshold ? 1 : 0;
            }
        }
    }
    void binarizeAll(BYTE *startPoint, int areaHeight, int areaWidth) { // change grey in biBuf into binary
        for (int i = 0; i < (height + areaHeight - 1) / areaHeight; i ++) {
            int blockHeight = min(areaHeight, (int)height - areaHeight * i);
            for (int j = 0; j < (width + areaWidth - 1) / areaWidth; j ++) {
                int blockWidth = min(areaWidth, (int)width - areaWidth * j);
                binarizeBlock(&startPoint[i * areaHeight * width + j * areaWidth], blockHeight, blockWidth);
            }
        }
        // binarizeBlock(startPoint, areaHeight, areaWidth);
    }

// public functions
public:
    BMP(string bmp_path) // initialize
    {
        path = bmp_path;
    }   
    void ReadRGB()  // read the bmp
    {
        FILE *fp = fopen(path.c_str(), "rb");
        if (!fp) {
            cout << "fail when open the origin bmp\n";
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

        int offset = (width * 3 + 3) / 4 * 4 - width * 3;
        for (int i = 0; i < height; i ++) {
            for (int j = 0; j < width; j ++) {
                fread(&originRGBBuf[i * width + j], sizeof(BGR), 1, fp);
            }
            if (offset) {
                for (int j = 0; j < offset; j ++) {
                    BYTE temp = 0;
                    fwrite(&temp, sizeof(BYTE), 1, fp);
                }
            }
        }

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
    void ChangeY(int yNum) // change the number of Y in newYUVBuf and according grayYUVPicBuf
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
    void WriteGray(string save_path) // output the results stored in grayYUVPicBuf
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

        int offset = (width * 3 + 3) / 4 * 4 - width * 3;
        for (int i = 0; i < height; i ++) {
            for (int j = 0; j < width; j ++) {
                fwrite(&grayYUVPicBuf[i * width + j], sizeof(BGR), 1, fp);
            }
            if (offset) {
                for (int j = 0; j < offset; j ++) {
                    BYTE temp = 0;
                    fwrite(&temp, sizeof(BYTE), 1, fp);
                }
            }
        }
        // fwrite(grayYUVPicBuf, sizeof(BGR), width * height, fp);

        fclose(fp);
    }
    void WriteRGB(string save_path) // output the results stored in newRGBPicBuf
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
        
        int offset = (width * 3 + 3) / 4 * 4 - width * 3; 
        for (int i = 0; i < height; i ++) {
            for (int j = 0; j < width; j ++) {
                fwrite(&newRGBPicBuf[i * width + j], sizeof(BGR), 1, fp);
            }
            if (offset) {
                for (int j = 0; j < offset; j ++) {
                    BYTE temp = 0;
                    fwrite(&temp, sizeof(BYTE), 1, fp);
                }
            }
        }
        // fwrite(newRGBPicBuf, sizeof(BGR), width * height, fp);

        fclose(fp);
    }
    
    void RGB2Grey() { // change 24bit-rgb into 8bit-grey and create head
        biInfoHead = infoHead;
        biInfoHead.biBitCount = 8; // bitcount
        biInfoHead.biSizeImage = (width + 3) / 4 * 4 * height; // size
        biInfoHead.biClrUsed = 256; // use plane
        biInfoHead.biClrImportant = 0;

        biFileHead = fileHead;
        biFileHead.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD); // new offset
        biFileHead.bfSize = biFileHead.bfOffBits + biInfoHead.biSizeImage;

        memset(biPla, 0, sizeof(RGBQUAD) * 256);
        biPla[0].rgbBlue = biPla[0].rgbGreen = biPla[0].rgbRed = (BYTE)255; biPla[0].rgbReserved = (BYTE)0; // white
        biPla[1].rgbBlue = biPla[1].rgbGreen = biPla[1].rgbRed = biPla[1].rgbReserved = (BYTE)0; // black

        biBuf = new BYTE[width * height];

        for (int i = 0; i < height; i ++) {
            for (int j = 0; j < width; j ++) {
                int index = i * width + j;
                biBuf[index] = (BYTE)((77 * (int)originRGBBuf[index].red + 150 * (int)originRGBBuf[index].green + 29 * (int)originRGBBuf[index].blue)>>8); // count grey
            }
        }
    }
    void ErosionGreyorBi() { // erosion to a grey or binary bmp with crossing
        BYTE *tempBiBuf = new BYTE[width * height];

        for (int i = 0; i < height; i ++) {
            for (int j = 0; j < width; j ++) {
                int index = i * width + j;
                tempBiBuf[index] = biBuf[index];
                if (i > 0           &&  biBuf[index - width] < biBuf[index])    tempBiBuf[index] = biBuf[index - width];// up
                if (i < height - 1  &&  biBuf[index + width] < biBuf[index])    tempBiBuf[index] = biBuf[index + width];// down
                if (j > 0           &&  biBuf[index - 1] < biBuf[index])        tempBiBuf[index] = biBuf[index - 1];    // left
                if (j < width - 1   &&  biBuf[index + 1] < biBuf[index])        tempBiBuf[index] = biBuf[index + 1];    // right
                if (i == 0 || i == height - 1 || j == 0 || j == width - 1)      tempBiBuf[index] = 0;                   // padding
            }
        }

        for (int i = 0; i < height * width; i ++) {
            biBuf[i] = tempBiBuf[i]; // change
        }

        free(tempBiBuf);
    }
    void DilationGreyorBi() { // delation to a grey or binary bmp with crossing
        BYTE *tempBiBuf = new BYTE[width * height];

        for (int i = 0; i < height; i ++) {
            for (int j = 0; j < width; j ++) {
                int index = i * width + j;
                tempBiBuf[index] = biBuf[index];
                if (i > 0           &&  biBuf[index - width] > biBuf[index])    tempBiBuf[index] = biBuf[index - width];// up
                if (i < height - 1  &&  biBuf[index + width] > biBuf[index])    tempBiBuf[index] = biBuf[index + width];// down
                if (j > 0           &&  biBuf[index - 1] > biBuf[index])        tempBiBuf[index] = biBuf[index - 1];    // left
                if (j < width - 1   &&  biBuf[index + 1] > biBuf[index])        tempBiBuf[index] = biBuf[index + 1];    // right
            }
        }

        for (int i = 0; i < height * width; i ++) {
            biBuf[i] = tempBiBuf[i];    // change
        }

        free(tempBiBuf);
    }
    void OpeningGreyorBi() { // mixture of erosion and dilation
        ErosionGreyorBi();
        DilationGreyorBi();
    }
    void ClosingGreyorBi() { // mixture of dilation and erosion
        DilationGreyorBi();
        ErosionGreyorBi();
    }
    void Grey2Bi(int area_height_number, int area_width_number) { // change grey into binary
        int h, w;
        if (height % area_height_number == 0) h = area_height_number;
        else h = area_height_number - 1;
        if (width % area_width_number == 0) w = area_width_number;
        else w = area_width_number - 1;
        binarizeAll(biBuf, height / h, width / w); // find threshold of each area
    }
    void WriteBi(string save_path) { // output the results stored in biBuf
        FILE *fp;
        fp = fopen(save_path.c_str(), "wb");
        if (!fp) {
            cout << "fail to open path to save\n";
            exit(1);
        }

        fwrite(&biFileHead, sizeof(BITMAPFILEHEADER), 1, fp);
        fwrite(&biInfoHead, sizeof(BITMAPINFOHEADER), 1, fp);
        fwrite(biPla, sizeof(RGBQUAD), 256, fp); // write plane

        fseek(fp, biFileHead.bfOffBits, 0);

        int offset = (width + 3) / 4 * 4 - width;
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width ; j += 1 ) {
                fwrite(&biBuf[i * width + j], sizeof(BYTE), 1, fp);
            }
            if (offset) {
                for (int j = 0; j < offset; j ++) {
                    BYTE temp = 0;
                    fwrite(&temp, sizeof(BYTE), 1, fp);
                }
            }
        }
        // fwrite(biBuf, sizeof(BYTE), width * height, fp);

        fclose(fp);
    }

    void ReadBi() { // read a binary bmp
        FILE *fp = fopen(path.c_str(), "rb");
        if (!fp) {
            cout << "fail when open the original bmp\n";
            exit(1);
        }

        fread(&biFileHead, sizeof(BITMAPFILEHEADER), 1, fp);
        if (0x4d42 != biFileHead.bfType) {
            cout << "origin is not a bmp\n";
            exit(1);
        }

        fread(&biInfoHead, sizeof(BITMAPINFOHEADER), 1, fp);
        for (int i = 0; i < 256; i ++) {
            fread(&biPla[i].rgbBlue, sizeof(BYTE), 1, fp);
            fread(&biPla[i].rgbGreen, sizeof(BYTE), 1, fp);
            fread(&biPla[i].rgbRed, sizeof(BYTE), 1, fp);
            fread(&biPla[i].rgbReserved, sizeof(BYTE), 1, fp);
        }

        width = biInfoHead.biWidth;
        height = biInfoHead.biHeight;
        // cout << width << ' ' << height << '\n';

        fseek(fp, biFileHead.bfOffBits, 0);

        originBiBuf = new BYTE[width * height];

        int offset = (width + 3) / 4 * 4 - width;
        for (int i = 0; i < height; i ++) {
            for (int j = 0; j < width; j ++) {
                fread(&originBiBuf[i * width + j], sizeof(BYTE), 1, fp);
            }
            if (offset) {
                for (int j = 0; j < offset; j ++) {
                    BYTE temp = 0;
                    fwrite(&temp, sizeof(BYTE), 1, fp);
                }
            }
        }

        fclose(fp);
    }
    void BiReadytoWrite() {
        biBuf = new BYTE[width * height];
        for (int i = 0; i < height * width; i ++) {
            biBuf[i] = originBiBuf[i];
            // cout << i << ":" << originBiBuf << endl;
        }
    }

    void test() { // test something
        for (int i = 0; i < width * height; i ++) {
            biBuf[i] = 1;
        }
    }
};

void solve(int area_height_number, int area_width_number , int erosion_times, int delation_times, int opening_times, int closing_times) {
    mkdir("./results"); // where to save results

    BMP bmp = BMP("./origin.bmp"); // define a bmp which is rgb

    bmp.ReadRGB();
    // bmp.test();
    bmp.RGB2Grey(); // first change it into grey
    bmp.Grey2Bi(area_height_number, area_width_number); // change grey into binary
    bmp.WriteBi("./results/originBi.bmp"); // use blocks to get threshold and change it into binary

    bmp.RGB2Grey(); // recreate the grey bmp
    bmp.Grey2Bi(area_height_number, area_width_number);
    for (int i = 0; i < erosion_times; i ++) bmp.ErosionGreyorBi(); // erosion to the grey bmp
    bmp.WriteBi("./results/erosionBi.bmp");

    bmp.RGB2Grey(); // recreate the grey bmp
    bmp.Grey2Bi(area_height_number, area_width_number);
    for (int i = 0; i < delation_times; i ++) bmp.DilationGreyorBi(); // delation to the grey bmp
    bmp.WriteBi("./results/delationBi.bmp");

    bmp.RGB2Grey(); // recreate the grey bmp
    bmp.Grey2Bi(area_height_number, area_width_number);
    for (int i = 0; i < opening_times; i ++) bmp.OpeningGreyorBi(); // opening
    bmp.WriteBi("./results/openingBi.bmp");

    bmp.RGB2Grey(); // recreate the grey bmp
    bmp.Grey2Bi(area_height_number, area_width_number);
    for (int i = 0; i < closing_times; i ++) bmp.ClosingGreyorBi(); // closing
    bmp.WriteBi("./results/closingBi.bmp");
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        solve(stoi((string)argv[1]), stoi((string)argv[2]), stoi((string)argv[3]), stoi((string)argv[4]), stoi((string)argv[5]), stoi((string)argv[6]));
    } else {
        solve(3, 3, 3, 3, 3, 3);
    }
}