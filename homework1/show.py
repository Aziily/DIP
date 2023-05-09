import os
from PIL import Image, ImageFile
import matplotlib.pyplot as plt

if __name__ == "__main__":
    ImageFile.LOAD_TRUNCATED_IMAGES = True
    if not os.path.exists("results"):
        print("there is no results to show\n")
        exit(1)
    rgbOrigin = Image.open(os.path.join("results", "rgbOrigin.bmp"))
    grayOrigin = Image.open(os.path.join("results", "grayOrigin.bmp"))
    rgbAfterChange = Image.open(os.path.join("results", "rgbAfterChange.bmp"))
    grayAfterChange = Image.open(os.path.join("results", "grayAfterChange.bmp"))

    # plt.ioff()
    plt.figure("Image show")
    plt.suptitle("all Images")
    plt.subplot(2, 2, 1), plt.title("rgbOrigin"), plt.axis("off"), plt.imshow(rgbOrigin)
    plt.subplot(2, 2, 2), plt.title("grayOrigin"), plt.axis("off"), plt.imshow(grayOrigin)
    plt.subplot(2, 2, 3), plt.title("rgbAfterChange"), plt.axis("off"), plt.imshow(rgbAfterChange)
    plt.subplot(2, 2, 4), plt.title("grayAfterChange"), plt.axis("off"), plt.imshow(grayAfterChange)
    plt.show()
    
    