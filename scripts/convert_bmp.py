from PIL import Image


img = Image.open("../assets/sample2.bmp")
img.save("../assets/sample2.bmp", format="BMP", compression=0)
