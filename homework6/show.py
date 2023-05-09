import os
from PIL import Image, ImageFile
import matplotlib.pyplot as plt

if __name__ == "__main__":
    ImageFile.LOAD_TRUNCATED_IMAGES = True
    if not os.path.exists("results"):
        print("there is no results to show\n")
        exit(1)
    origin = Image.open(os.path.join("origin.bmp"))
    origin_grey = Image.open(os.path.join("results", "origin_grey.bmp"))
    
    bilateral_fliter = Image.open(os.path.join("results", "bilateral_fliter.bmp"))
    bilateral_fliter_grey = Image.open(os.path.join("results", "bilateral_fliter_grey.bmp"))

    # plt.ioff()
    plt.figure("Image show", figsize=(16, 8))
    plt.suptitle("all Images")
    plt.subplot(2, 2, 1), plt.title("origin"), plt.axis("off"), plt.imshow(origin)
    plt.subplot(2, 2, 2), plt.title("origin_grey"), plt.axis("off"), plt.imshow(origin_grey)
    plt.subplot(2, 2, 3), plt.title("bilateral_fliter"), plt.axis("off"), plt.imshow(bilateral_fliter)
    plt.subplot(2, 2, 4), plt.title("bilateral_fliter_grey"), plt.axis("off"), plt.imshow(bilateral_fliter_grey)
    plt.show()
    