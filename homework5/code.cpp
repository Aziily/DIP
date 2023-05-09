#include <bits/stdc++.h>
#include <direct.h>
#include <Windows.h>

using namespace std;

class BMP 
{
private:
    const BYTE defaultY = 128;

// variables
private:
    typedef struct { // to store origin RGB picture data
        BYTE blue, green, red;
    } BGR;
    typedef struct { // to store changed YUV picture data
        BYTE Y, U, V;
    } YUV;

    string path;                        // path to the origin picture

    // about not changing the size
    long width, height;                 // picture size
    BITMAPFILEHEADER fileHead;          // file's information
    BITMAPINFOHEADER infoHead;          // bmp's information
    RGBQUAD Pla[256];                   // store the pla
    BGR *originRGBBuf;                  // store the origin RGB picture
    BGR *greyYUVPicBuf;                 // store the grey YUV picture to output
    BGR *newRGBPicBuf;                  // store the rgb picture to output
    YUV *newYUVBuf;                     // store the YUV from the origin and after being changed

    BYTE *originBiBuf;                  // store the origin Bi picture
    RGBQUAD biPla[256];                 // store the pla of binary bmp
    BITMAPFILEHEADER biFileHead;        // binary file's information
    BITMAPINFOHEADER biInfoHead;        // binary bmp's information
    BYTE *biBuf;

    // about create new photo
    long create_width, create_height;
    BITMAPFILEHEADER create_fileHead;   // file's information
    BITMAPINFOHEADER create_infoHead;   // bmp's information
    RGBQUAD create_pla[256];            // store the pla
    BGR *createBuf;                     // store the data

// private functions
private:
    /**
     * > The function is used to binarize a block of pixels
     * 
     * @param startPoint the starting point of the block
     * @param blockHeight the height of the block
     * @param blockWidth the width of the block
     */
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
    /**
     * For each block of pixels in the image, binarize the block
     * 
     * @param startPoint the starting point of the block
     * @param areaHeight the height of the area to be binarized
     * @param areaWidth the width of the area to be binarized
     */
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
    /**
     * It compares two BGR objects and returns true if they are the same.
     * 
     * @param a The first color to compare.
     * @param b The second color to compare.
     * 
     * @return a boolean value.
     */
    bool sameRGB(BGR a, BGR b) {
        return a.blue == b.blue && a.green == b.green && a.red == b.red;
    }

    /**
     * It takes a shear factor, and shears the image by that factor
     * 
     * @param dH the amount of shear in the vertical direction
     * @param dW the amount of shear in the x direction
     */
    void shearOne(double dH, double dW) {
        const BGR pad = {defaultY, defaultY, defaultY};

        int midwidth = (create_width + 1) / 2, midheight = (create_height + 1) / 2;
        int new_midwidth = midwidth + midheight * abs(dW), new_midheight = midheight + midwidth * abs(dH);
        
        BGR *tempBuf = new BGR[4 * new_midheight * new_midwidth];
        for (int i = 0; i < 4 * new_midheight * new_midwidth; i ++) tempBuf[i] = pad;

        for (int i = 0; i < create_height; i ++) {
            for (int j = 0; j < create_width; j ++) {
                int x = j - midwidth, y = i - midheight;
                tempBuf[(int)(y + x * dH + new_midheight) * 2 * new_midwidth + (int)(x + y * dW + new_midwidth)] = createBuf[i * create_width + j]; 
            }
        }

        free(createBuf);
        create_width = 2 * new_midwidth; create_height = 2 * new_midheight;
        createBuf = new BGR[2 * new_midwidth * 2 * new_midheight];
        for (int i = 0; i < 2 * new_midwidth * 2 * new_midheight; i ++) createBuf[i] = tempBuf[i];
        free(tempBuf);
    }

// public functions
public:
   /**
    * A constructor.
    * 
    * @param bmp_path The path to the BMP file.
    */
    BMP(string bmp_path) // initialize
    {
        path = bmp_path;
    }   
    


    /* functions for 24bit Pic */
    /**
     * Read the bmp file and store the RGB value of each pixel in the originRGBBuf array
     */
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
    /**
     * Convert the RGB image to YUV image, and save the YUV image in greyYUVPicBuf.
     */
    void RGB2YUV() // change picture saved by RGB to YUV
    {
        greyYUVPicBuf = new BGR[width * height];
        newYUVBuf = new YUV[width * height];
        for (int i = 0; i < width * height; i ++) {
            newYUVBuf[i].Y = (BYTE)(0.299 * (double)originRGBBuf[i].red + 0.587 * (double)originRGBBuf[i].green + 0.114 * (double)originRGBBuf[i].blue);
            newYUVBuf[i].U = (BYTE)(-0.1687 * (double)originRGBBuf[i].red - 0.3313 * (double)originRGBBuf[i].green + 0.5 * (double)originRGBBuf[i].blue + 128);
            newYUVBuf[i].V = (BYTE)(0.5 * (double)originRGBBuf[i].red - 0.4187 * (double)originRGBBuf[i].green - 0.0813 * (double)originRGBBuf[i].blue + 128);

            greyYUVPicBuf[i].blue = newYUVBuf[i].Y;
            greyYUVPicBuf[i].green = newYUVBuf[i].Y;
            greyYUVPicBuf[i].red = newYUVBuf[i].Y;
        }
    }
    /**
     * It converts YUV to RGB.
     */
    void YUV2RGB() // rechange picture saved by YUV to RGB
    {
        newRGBPicBuf = new BGR[width * height];
        for (int i = 0; i < width * height; i ++) {
            double red = (double)newYUVBuf[i].Y + 1.402 * ((double)newYUVBuf[i].V - 128);
            double green = (double)newYUVBuf[i].Y - 0.34414 * ((double)newYUVBuf[i].U - 128) - 0.71414 * ((double)newYUVBuf[i].V - 128);
            double blue = (double)newYUVBuf[i].Y + 1.772 * ((double)newYUVBuf[i].U - 128);

            if (red < 0) newRGBPicBuf[i].red = 0;
            else if (red > 255) newRGBPicBuf[i].red = 255;
            else newRGBPicBuf[i].red = (BYTE)red;
            if (green < 0) newRGBPicBuf[i].green = 0;
            else if (green > 255) newRGBPicBuf[i].green = 255;
            else newRGBPicBuf[i].green = (BYTE)green;
            if (blue < 0) newRGBPicBuf[i].blue = 0;
            else if (blue > 255) newRGBPicBuf[i].blue = 255;
            else newRGBPicBuf[i].blue = (BYTE)blue;
        }
    }
    
    /* should use before rgb2yuv */
    /**
     * ChangeY(int yNum) // change the number of Y in newYUVBuf and according greyYUVPicBuf
     * 
     * @param yNum the number of Y to be changed
     */
    void ChangeY(int yNum) // change the number of Y in newYUVBuf and according greyYUVPicBuf
    {
        for (int i = 0; i < width * height; i ++) {
            int temp = (int)newYUVBuf[i].Y + yNum;
            newYUVBuf[i].Y = (BYTE)temp;
            if (temp < 0) newYUVBuf[i].Y = 0;
            if (temp > 255) newYUVBuf[i].Y = 255;

            greyYUVPicBuf[i].blue = newYUVBuf[i].Y;
            greyYUVPicBuf[i].green = newYUVBuf[i].Y;
            greyYUVPicBuf[i].red = newYUVBuf[i].Y;
        }
    }
    /**
     * The function is used to enhance the visibility of the image by using Y
     */
    void EnhanceVisibilityY() { // use image logarithmic operation for visibility enhancement by using Y
        int maxY = 0;
        for (int i = 0; i < height; i ++) {
            for (int j = 0; j < width; j ++) {
                maxY = max(maxY, (int)newYUVBuf[i * width + j].Y); // find max
            }
        }
        for (int i = 0; i < height * width; i ++) {
            int temp = (int)(255 * (double)log10((double)newYUVBuf[i].Y + 1) / (double)log10((double)maxY+ 1)); // update the value
            newYUVBuf[i].Y = (BYTE)temp;

            greyYUVPicBuf[i].blue = newYUVBuf[i].Y; // store it
            greyYUVPicBuf[i].green = newYUVBuf[i].Y;
            greyYUVPicBuf[i].red = newYUVBuf[i].Y;
        }
    }
    /**
     * > The function is to equalize the histogram of the Y component of the YUV image
     */
    void HistogramEqualizeY() { // histogram equalization by using Y
        double percent[256] = {0};
        int newY[256] = {0};

        for (int i = 0; i < height * width; i ++) {
            percent[newYUVBuf[i].Y] ++; // count all
        }

        percent[0] = percent[0] / (height * width); // calculate percent
        newY[0] = int(percent[0] * 255); // update Y
        for (int i = 1; i < 256; i ++) {
            percent[i] = percent[i] / (height * width);
            percent[i] = percent[i] + percent[i-1];
            newY[i] = int(percent[i] * 255);
        }

        for (int i = 0; i < height * width; i ++) {
            newYUVBuf[i].Y = (BYTE)newY[newYUVBuf[i].Y]; // store new

            greyYUVPicBuf[i].blue = newYUVBuf[i].Y;
            greyYUVPicBuf[i].green = newYUVBuf[i].Y;
            greyYUVPicBuf[i].red = newYUVBuf[i].Y;
        }
    }
    /**
     * > This function is used to do the mean filter
     * 
     * @param len the length of the filter
     */
    void MeanFilter(int len) {
        //  modify length
        if (len <= 0) return;
        else len = ((len + 1) / 2 * 2 - 1);

        int radius = len / 2;   //  radius
        YUV *tempYUV = new YUV[height * width];

        for (int i = 0; i < height; i ++) {
            for (int j = 0; j < width; j ++) {
                int temp = 0;   //  store the sum
                for (int a = i - radius; a <= i + radius; a ++) {
                    for (int b = j - radius; b <= j + radius; b ++) {
                        temp += (a < 0 || b < 0 || a >= height || b >= width) ? 0 : (int)newYUVBuf[a * width + b].Y;    //  add pad or grey
                    }
                }
                tempYUV[i * width + j].Y = BYTE((double)temp / len / len);  // calculate mean
            }
        }
        //  update
        for (int i = 0; i < height * width; i ++) {
            newYUVBuf[i].Y = tempYUV[i].Y;

            greyYUVPicBuf[i].blue = newYUVBuf[i].Y;
            greyYUVPicBuf[i].green = newYUVBuf[i].Y;
            greyYUVPicBuf[i].red = newYUVBuf[i].Y;
        }

        free(tempYUV);
    }
    /**
     * > The function is used to enhance the image by using the Laplacian operator
     * 
     * @param type 0 for first-oreder, other for second-order
     */
    void LaplacianEnhance(int type) {
        YUV *tempYUV = new YUV[height * width];
        //  judge which type of core to choose
        if (type) {
            for (int i = 0; i < height; i ++) {
                for (int j = 0; j < width; j ++) {
                    int temp = 0;
                    //  8 besides rect
                    for (int a = i - 1; a <= i + 1; a ++) {
                        for (int b = j - 1; b <= j + 1; b ++) {
                            temp += (a < 0 || b < 0 || a >= height || b >= width) ? 0 : (int)newYUVBuf[a * width + b].Y;
                        }
                    }
                    tempYUV[i * width + j].Y = BYTE(temp - 9 * (int)newYUVBuf[i * width + j].Y);
                }
            }
        } else {
            for (int i = 0; i < height; i ++) {
                for (int j = 0; j < width; j ++) {
                    int temp = 0;
                    //  4 besides rect
                    for (int a = i - 1; a <= i + 1; a ++) {
                        temp += (a < 0 || a >= height) ? 0 : (int)newYUVBuf[a * width + j].Y;
                    }
                    for (int b = j - 1; b <= j + 1; b ++) {
                        temp += (b < 0 || b >= width) ? 0 : (int)newYUVBuf[i * width + b].Y;
                    }
                    tempYUV[i * width + j].Y = BYTE(temp - 6 * (int)newYUVBuf[i * width + j].Y);
                }
            }
        }
        for (int i = 0; i < height * width; i ++) {
            newYUVBuf[i].Y = tempYUV[i].Y + newYUVBuf[i].Y;

            greyYUVPicBuf[i].blue = newYUVBuf[i].Y;
            greyYUVPicBuf[i].green = newYUVBuf[i].Y;
            greyYUVPicBuf[i].red = newYUVBuf[i].Y;
        }

        free(tempYUV);
    }
    /**
     * The function is used to filter the image with a bilateral filter
     * 
     * @param len the length of the filter
     * @param sigma_dis the standard deviation of the distance
     * @param sigma_Y the standard deviation of the Gaussian function in the Y channel
     * 
     * @return the filtered image.
     */
    void BilateralFilter(int len, double sigma_dis, double sigma_Y) {
        if (len <= 0) return;
        else len = ((len + 1) / 2 * 2 - 1);
        double divider_dis = -1.0/(2 * sigma_dis * sigma_dis);
        double divier_Y = -1.0/(2 * sigma_Y * sigma_Y);

        int radius = len / 2;
        YUV *tempYUV = new YUV[height * width];

        double Y_w[256];
        double *dis_w = new double[len * len];

        for (int i = 0; i < 256; i ++) {
            Y_w[i] = exp(i * i * divier_Y);
        }
        for (int i = -radius; i <= radius; i ++) {
            for (int j = -radius; j <= radius; j ++) {
                dis_w[(i + radius) * len + (j + radius)] = exp((i * i + j * j) * divider_dis); 
            }
        }

        for (int i = 0; i < height; i ++) {
            for (int j = 0; j < width; j ++) {
                double weightSum = 0;
                double ValueSum = 0;
                for (int ti = -radius; ti <= radius; ti ++) {
                    for (int tj = -radius; tj <= radius; tj ++) {
                        int r_i = i + ti; r_i = r_i < 0 ? 0 : r_i; r_i = r_i >= height ? height - 1 : r_i;
                        int r_j = j + tj; r_j = r_j < 0 ? 0 : r_j; r_j = r_j >= width ? width - 1 : r_j;
                        int Y_temp = (int)abs(newYUVBuf[r_i * width + r_j].Y - newYUVBuf[i * width + j].Y);
                        double weight = dis_w[(ti + radius) * len + (tj + radius)] + Y_w[Y_temp];
                        ValueSum += (int)newYUVBuf[r_i * width + r_j].Y * weight;
                        weightSum += weight;
                    }
                }
                tempYUV[i * width + j].Y = (BYTE)(ValueSum / weightSum);
            }
        }

        for (int i = 0; i < height * width; i ++) {
            newYUVBuf[i].Y = tempYUV[i].Y;

            greyYUVPicBuf[i].blue = newYUVBuf[i].Y;
            greyYUVPicBuf[i].green = newYUVBuf[i].Y;
            greyYUVPicBuf[i].red = newYUVBuf[i].Y;
        }

        free(tempYUV);
    }
    /* should use before rgb2yuv and yuv2rgb */
    /**
    * The function takes the RGB values of each pixel and finds the maximum value of each color. Then,
    * it takes the logarithm of each pixel's color value and divides it by the logarithm of the maximum
    * value of that color. This is done to enhance the visibility of the image
    */
    void EnhanceVisibilityRGB() { // use image logarithmic operation for visibility enhancement by using RGB
        int maxR = 0, maxG = 0, maxB = 0;
        for (int i = 0; i < height; i ++) {
            for (int j = 0; j < width; j ++) {
                maxR = max(maxR, (int)newRGBPicBuf[i * width + j].red); // find max
                maxG = max(maxG, (int)newRGBPicBuf[i * width + j].green); // find max
                maxB = max(maxB, (int)newRGBPicBuf[i * width + j].blue); // find max
            }
        }
        for (int i = 0; i < height * width; i ++) {
            int red = (int)(255 * (double)log10((double)newRGBPicBuf[i].red + 1) / (double)log10((double)maxR+ 1)); // update the value
            int blue = (int)(255 * (double)log10((double)newRGBPicBuf[i].green + 1) / (double)log10((double)maxG+ 1)); // update the value
            int green = (int)(255 * (double)log10((double)newRGBPicBuf[i].blue + 1) / (double)log10((double)maxB+ 1)); // update the value

            newRGBPicBuf[i].blue = blue; // store it
            newRGBPicBuf[i].green = green;
            newRGBPicBuf[i].red = red;
        }
    }
    /**
     * For each pixel, we find the percentage of pixels that have a lower value than the current pixel,
     * and then multiply that percentage by 255 to get the new pixel value
     */
    void HistogramEqualizeRGB() { // histogram equalization by using RGB
        double percentR[256] = {0}, percentG[256] = {0}, percentB[256] = {0};
        int newR[256] = {0}, newG[256] = {0}, newB[256] = {0};

        for (int i = 0; i < height * width; i ++) {
            percentR[newRGBPicBuf[i].red] ++;
            percentG[newRGBPicBuf[i].green] ++;
            percentB[newRGBPicBuf[i].blue] ++; // count all
        }

        percentR[0] = percentR[0] / (height * width); // calculate percent
        newR[0] = int(percentR[0] * 255); // update R
        percentG[0] = percentG[0] / (height * width);
        newR[0] = int(percentG[0] * 255);
        percentB[0] = percentB[0] / (height * width);
        newR[0] = int(percentB[0] * 255);

        for (int i = 0; i < 256; i ++) {
            percentR[i] = percentR[i] / (height * width);
            percentR[i] = percentR[i] + percentR[i-1];
            newR[i] = int(percentR[i] * 255);

            percentG[i] = percentG[i] / (height * width);
            percentG[i] = percentG[i] + percentG[i-1];
            newG[i] = int(percentG[i] * 255);

            percentB[i] = percentB[i] / (height * width);
            percentB[i] = percentB[i] + percentB[i-1];
            newB[i] = int(percentB[i] * 255);
        }

        for (int i = 0; i < height * width; i ++) {
            newRGBPicBuf[i].red = newR[newRGBPicBuf[i].red];
            newRGBPicBuf[i].green = newG[newRGBPicBuf[i].green];
            newRGBPicBuf[i].blue = newB[newRGBPicBuf[i].blue];
        }
    }
    /**
     * It writes the image to the file.
     * 
     * @param save_path the path to save the output image
     */
    void WriteNewgrey(string save_path) // output the results stored in greyYUVPicBuf
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
                fwrite(&greyYUVPicBuf[i * width + j], sizeof(BGR), 1, fp);
            }
            if (offset) {
                for (int j = 0; j < offset; j ++) {
                    BYTE temp = 0;
                    fwrite(&temp, sizeof(BYTE), 1, fp);
                }
            }
        }
        // fwrite(greyYUVPicBuf, sizeof(BGR), width * height, fp);

        fclose(fp);
    }
    
    /* should use before yuv2rgb */
    /**
     * It writes the newRGBPicBuf to a new file.
     * 
     * @param save_path the path to save the new image
     */
    void WriteNewRGB(string save_path) // output the results stored in newRGBPicBuf
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
    
    
    /**
     * > This function is used to convert the 24-bit RGB image into 8-bit grey image
     */
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
    
    /* should be used after rgb2grey or readbi */
   /**
    * > The function is to do erosion to a grey or binary bmp with crossing
    */
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
    /**
     * > The function is to do a dilation to a grey or binary bmp with crossing
     */
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
    /**
     * "OpeningGreyorBi()" is a function that erodes the image and then dilates the image
     */
    void OpeningGreyorBi() { // mixture of erosion and dilation
        ErosionGreyorBi();
        DilationGreyorBi();
    }
    /**
     * ClosingGreyorBi() is a mixture of dilation and erosion
     */
    void ClosingGreyorBi() { // mixture of dilation and erosion
        DilationGreyorBi();
        ErosionGreyorBi();
    }
    /**
     * > The function is used to change the grey image into binary image
     * 
     * @param area_height_number the number of areas in the height direction
     * @param area_width_number the number of areas in the width direction
     */
    void Grey2Bi(int area_height_number, int area_width_number) { // change grey into binary
        int h, w;
        if (height % area_height_number == 0) h = area_height_number;
        else h = area_height_number - 1;
        if (width % area_width_number == 0) w = area_width_number;
        else w = area_width_number - 1;
        binarizeAll(biBuf, height / h, width / w); // find threshold of each area
    }
    /**
     * It writes the image data to a file.
     * 
     * @param save_path the path to save the image
     */
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



    /* functions for 8bit Pic */

   /**
    * Read the binary bmp file and store the data in the originBiBuf
    */
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
    /**
     * Write the binary bmp picture
     */
    void BiReadytoWrite() {
        biBuf = new BYTE[width * height];
        for (int i = 0; i < height * width; i ++) {
            biBuf[i] = originBiBuf[i];
            // cout << i << ":" << originBiBuf << endl;
        }
    }



    /* functions for transformation of 24-bit Picture*/

    /**
     * > Store the data from origin Pic
     */
    void PrepareTransform() { // store the data from origin Pic
        create_fileHead = fileHead;
        create_infoHead = infoHead;
        create_width = width;
        create_height = height;
        createBuf = new BGR[create_height * create_width];
        for (int i = 0; i < 256; i ++) create_pla[i] = Pla[i];
        for (int i = 0; i < width * height; i ++) createBuf[i] = originRGBBuf[i];
    }
    /* should be used after prepare transform */
    /**
     * It creates a new image with the same size as the original image, and then moves the picture
     * 
     * @param height_bia the height of the translation
     * @param width_bia the width of the image to be moved
     */
    void Translation(int height_bia, int width_bia) { // change the position
        const BGR pad = {defaultY, defaultY, defaultY};

        long new_width = (create_width + abs(width_bia)), new_height = (create_height + abs(height_bia));
        BGR *tempBuf = new BGR[new_width * new_height];
        for (int i = 0; i < new_height; i ++)
            for (int j = 0; j < new_width; j ++)
                tempBuf[i * new_width + j] = pad;

        if (height_bia >= 0) {
            if (width_bia >= 0) {
                for (int i = height_bia; i < new_height; i ++)
                    for (int j = width_bia; j < new_width; j ++)
                        tempBuf[i * new_width + j] = createBuf[(i - height_bia) * create_width + j - width_bia];
            } else {
                for (int i = height_bia; i < new_height; i ++)
                    for (int j = 0; j < create_width; j ++)
                        tempBuf[i * new_width + j] = createBuf[(i - height_bia) * create_width + j];
            }
        } else {
            if (width_bia >= 0) {
                for (int i = 0; i < create_height; i ++)
                    for (int j = width_bia; j < new_width; j ++)
                        tempBuf[i * new_width + j] = createBuf[i * create_width + j - width_bia];
            } else {
                for (int i = 0; i < create_height; i ++)
                    for (int j = 0; j < create_width; j ++)
                        tempBuf[i * new_width + j] = createBuf[i * create_width + j];
            }
        }

        free(createBuf);
        create_width = new_width; create_height = new_height;
        createBuf = new BGR[new_width * new_height];
        for (int i = 0; i < new_width * new_height; i ++) createBuf[i] = tempBuf[i];
        free(tempBuf);
    }
    /**
     * It rotates the image.
     * 
     * @param deg the degree of rotation
     */
    void Rotation(double deg) { // rotate the create photo
        const BGR pad = {defaultY, defaultY, defaultY};

        double deg_pi = - deg * 3.1415926 / 180;
        int midwidth = (create_width + 1) / 2, midheight = (create_height + 1) / 2;
        int maxlength = (int)ceil(sqrt(pow(midwidth, 2) + pow(midheight, 2)));
        
        BGR *tempBuf = new BGR[4 * maxlength * maxlength];
        for (int i = 0; i < 4 * maxlength * maxlength; i ++) tempBuf[i] = pad;

        for (int i = 0; i < create_height; i ++) {
            for (int j = 0; j < create_width; j ++) {
                int x = j - midwidth, y = i - midheight;
                tempBuf[(int)(x * sin(deg_pi) + y * cos(deg_pi) + maxlength) * 2 * maxlength + (int)(x * cos(deg_pi) - y * sin(deg_pi) + maxlength)] = createBuf[i * create_width + j]; 
            }
        }
        
        for (int i = 0; i < 2 * maxlength; i ++)  {
            for (int j = 1; j < 2 * maxlength - 1; j ++) {
                if (sameRGB(tempBuf[i * 2 * maxlength + j], pad) \
                && !sameRGB(tempBuf[i * 2 * maxlength + j - 1], pad)\
                && !sameRGB(tempBuf[i * 2 * maxlength + j + 1], pad)) tempBuf[i * 2 * maxlength + j] = tempBuf[i * 2 * maxlength + j - 1];
            }
        }

        free(createBuf);
        create_width = 2 * maxlength; create_height = 2 * maxlength;
        createBuf = new BGR[2 * maxlength * 2 * maxlength];
        for (int i = 0; i < 2 * maxlength * 2 * maxlength; i ++) createBuf[i] = tempBuf[i];
        free(tempBuf);
    }
    /**
     * It scales the image by the given scaleH and scaleW.
     * 
     * @param scaleH the scale of height
     * @param scaleW the scale of width
     */
    void Scale(double scaleH, double scaleW) { // enlarge or smallen the Pic by height and width
        const BGR pad = {defaultY, defaultY, defaultY};
        if (scaleH == 0 || scaleW == 0) {
            printf("error when using scale, meeting 0 as scale parameter\n");
            return;
        }
        if (scaleH == 1 && scaleW == 1) return;

        int new_width = create_width * scaleW, new_height = create_height * scaleH;
        BGR *tempBuf = new BGR[new_width * new_height];
        for (int i = 0; i < new_height; i ++)
            for (int j = 0; j < new_width; j ++)
                tempBuf[i * new_width + j] = pad;

        for (int i = 0; i < new_height; i ++) {
            int origini = i / scaleH;
            for (int j = 0; j < new_width; j ++) {
                int originj = j / scaleW;
                int red = 0, green = 0, blue = 0;
                int count = 0;
                for (int ti = 0; ti < 1 / scaleH; ti ++) {
                    for (int tj = 0; tj < 1 / scaleW; tj ++) {
                        count ++;
                        red += createBuf[(origini + ti) * create_width + (originj + tj)].red;
                        green += createBuf[(origini + ti) * create_width + (originj + tj)].green;
                        blue += createBuf[(origini + ti) * create_width + (originj + tj)].blue;
                    }
                }
                red /= count; green /= count; blue /= count;
                tempBuf[i * new_width + j].red = red;
                tempBuf[i * new_width + j].green = green;
                tempBuf[i * new_width + j].blue = blue;
            }
        }
        
        free(createBuf);
        create_width = new_width; create_height = new_height;
        createBuf = new BGR[new_width * new_height];
        for (int i = 0; i < new_width * new_height; i ++) createBuf[i] = tempBuf[i];
        free(tempBuf);
    }
   /**
    * It shears the picture by dH and dW.
    * 
    * @param dH the change in height
    * @param dW the change in width
    */
    void Shear(double dH, double dW) { // shear the pic, dH means the change in height and dW means the change in width
        shearOne(dH, 0);
        shearOne(0, dW);
    }
   /**
    * Mirror the picture, 0 means horizontal, 1 means vertical
    * 
    * @param axis 0 means horizontal, 1 means vertical
    * 
    * @return the value of the pixel at the given coordinates.
    */
    void Mirror(int axis) { // mirror the pic, 0 means horizontal, 1 means vertical
        const BGR pad = {defaultY, defaultY, defaultY};
        
        BGR *tempBuf = new BGR[create_width * create_height];
        for (int i = 0; i < create_height * create_width; i ++) tempBuf[i] = pad;

        if (axis == 0) {
            for (int i = 0; i < create_height; i ++) {
                for (int j = 0; j < create_width; j ++) {
                    tempBuf[i * create_width + create_width - 1 - j] = createBuf[i * create_width + j];
                }
            }
        } else if (axis == 1) {
            for (int i = 0; i < create_height; i ++) {
                for (int j = 0; j < create_width; j ++) {
                    tempBuf[(create_height - 1 - i) * create_width + j] = createBuf[i * create_width + j];
                }
            }
        } else {
            return;
        }

        free(createBuf);
        createBuf = new BGR[create_height * create_width];
        for (int i = 0; i < create_height * create_width; i ++) createBuf[i] = tempBuf[i];
        free(tempBuf);
    }
    /**
     * It writes the image to the file.
     * 
     * @param save_path the path to save the transformed image
     */
    void WriteTransform(string save_path) { // output the results stored in createBuf
        FILE *fp;
        fp = fopen(save_path.c_str(), "wb");
        if (!fp) {
            cout << "fail to open path to save\n";
            exit(1);
        }
        
        create_infoHead.biHeight = create_height;
        create_infoHead.biWidth = create_width;
        create_infoHead.biSizeImage = (create_width + 3) / 4 * 4 * create_height; // size
        create_infoHead.biClrUsed = 0; // use plane
        create_infoHead.biClrImportant = 0;

        create_fileHead.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER); // new offset
        create_fileHead.bfSize = create_fileHead.bfOffBits + biInfoHead.biSizeImage;

        fwrite(&create_fileHead, sizeof(BITMAPFILEHEADER), 1, fp);
        fwrite(&create_infoHead, sizeof(BITMAPINFOHEADER), 1, fp);

        fseek(fp, create_fileHead.bfOffBits, 0);

        int offset = (create_width * 3 + 3) / 4 * 4 - create_width * 3;
        for (int i = 0; i < create_height; i++) {
            for (int j = 0; j < create_width ; j += 1 ) {
                fwrite(&createBuf[i * create_width + j], sizeof(BGR), 1, fp);
            }
            if (offset) {
                for (int j = 0; j < offset; j ++) {
                    BYTE temp = 0;
                    fwrite(&temp, sizeof(BYTE), 1, fp);
                }
            }
        }
        // fwrite(createBuf, sizeof(BYTE), create_width * create_height, fp);

        fclose(fp);
    }



    void test() { // test something
        for (int i = 10 * width; i < 20 * width; i ++) {
            cout << (int)newYUVBuf[i].Y << ' ' << (int)newRGBPicBuf[i].red << ' ' << (int)newRGBPicBuf[i].green << ' ' << (int)newRGBPicBuf[i].blue << endl;
        }
    }
};

void solve() {
    cout << "start solve" << endl;

    mkdir("./results"); // where to save results

    BMP bmp = BMP("./origin.bmp"); // define a bmp which is rgb
    bmp.ReadRGB();
    bmp.RGB2YUV();
    bmp.WriteNewgrey("./results/origin_grey.bmp");

    int n1, len;
    cout << "Please enter the number of times to use mean fliter: " << endl;
    cin >> n1;
    cout << "Please enter the length of mean fliter core (should be an odd number larger than 0, or will be changed automatically): " << endl;
    cin >> len;
    bmp.RGB2YUV();
    for (int i = 0; i < n1; i ++) bmp.MeanFilter(len);
    bmp.YUV2RGB();
    bmp.WriteNewRGB("./results/mean_fliter.bmp");
    bmp.WriteNewgrey("./results/mean_fliter_grey.bmp");

    int n2, op;
    cout << "Please enter the number of times to use Laplacian fliter: " << endl;
    cin >> n2;
    cout << "Please enter the type you want to use (0 for first-order, others for second-order): " << endl;
    cin >> op;
    bmp.RGB2YUV();
    for (int i = 0; i < n2; i ++) bmp.LaplacianEnhance(op);
    bmp.YUV2RGB();
    bmp.WriteNewRGB("./results/laplacian_fliter.bmp");
    bmp.WriteNewgrey("./results/laplacian_fliter_grey.bmp");

    cout << "end solve" << endl;
}

int main(int argc, char* argv[]) {
    solve();
}