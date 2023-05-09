import os
from PIL import Image, ImageFile
import matplotlib.pyplot as plt

if __name__ == "__main__":
    ImageFile.LOAD_TRUNCATED_IMAGES = True
    if not os.path.exists("results"):
        print("there is no results to show\n")
        exit(1)
    origin = Image.open(os.path.join("origin.bmp"))
    originGrey = Image.open(os.path.join("results", "originGrey.bmp"))
    
    enhanceYRGB = Image.open(os.path.join("results", "enhanceYRGB.bmp"))
    enhanceYGrey = Image.open(os.path.join("results", "enhanceYGrey.bmp"))
    enhanceRGBRGB = Image.open(os.path.join("results", "enhanceRGBRGB.bmp"))
    
    equalizeYRGB = Image.open(os.path.join("results", "equalizeYRGB.bmp"))
    equalizeYGrey = Image.open(os.path.join("results", "equalizeYGrey.bmp"))
    equalizeRGBRGB = Image.open(os.path.join("results", "equalizeRGBRGB.bmp"))

    # plt.ioff()
    plt.figure("Image show", figsize=(16, 8))
    plt.suptitle("all Images")
    plt.subplot(3, 3, 1), plt.title("origin"), plt.axis("off"), plt.imshow(origin)
    plt.subplot(3, 3, 2), plt.title("originGrey"), plt.axis("off"), plt.imshow(originGrey)
    plt.subplot(3, 3, 4), plt.title("enhanceYRGB"), plt.axis("off"), plt.imshow(enhanceYRGB)
    plt.subplot(3, 3, 5), plt.title("enhanceYGrey"), plt.axis("off"), plt.imshow(enhanceYGrey)
    plt.subplot(3, 3, 6), plt.title("enhanceRGBRGB"), plt.axis("off"), plt.imshow(enhanceRGBRGB)
    plt.subplot(3, 3, 7), plt.title("equalizeYRGB"), plt.axis("off"), plt.imshow(equalizeYRGB)
    plt.subplot(3, 3, 8), plt.title("equalizeYGrey"), plt.axis("off"), plt.imshow(equalizeYGrey)
    plt.subplot(3, 3, 9), plt.title("equalizeRGBRGB"), plt.axis("off"), plt.imshow(equalizeRGBRGB)
    plt.show()
    