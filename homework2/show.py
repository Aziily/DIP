import os
from PIL import Image, ImageFile
import matplotlib.pyplot as plt

if __name__ == "__main__":
    ImageFile.LOAD_TRUNCATED_IMAGES = True
    if not os.path.exists("results"):
        print("there is no results to show\n")
        exit(1)
    origin = Image.open(os.path.join("origin.bmp"))
    originBi = Image.open(os.path.join("results", "originBi.bmp"))
    
    erosionBi = Image.open(os.path.join("results", "erosionBi.bmp"))
    delationBi = Image.open(os.path.join("results", "delationBi.bmp"))
    openingBi = Image.open(os.path.join("results", "openingBi.bmp"))
    closingBi = Image.open(os.path.join("results", "closingBi.bmp"))

    # plt.ioff()
    plt.figure("Image show")
    plt.suptitle("all Images")
    plt.subplot(3, 2, 1), plt.title("origin"), plt.axis("off"), plt.imshow(origin)
    plt.subplot(3, 2, 2), plt.title("originBi"), plt.axis("off"), plt.imshow(originBi)
    plt.subplot(3, 2, 3), plt.title("erosionBi"), plt.axis("off"), plt.imshow(erosionBi)
    plt.subplot(3, 2, 4), plt.title("delationBi"), plt.axis("off"), plt.imshow(delationBi)
    plt.subplot(3, 2, 5), plt.title("openingBi"), plt.axis("off"), plt.imshow(openingBi)
    plt.subplot(3, 2, 6), plt.title("closingBi"), plt.axis("off"), plt.imshow(closingBi)
    plt.show()
    