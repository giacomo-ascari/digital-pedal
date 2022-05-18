from PIL import Image

def compacter(in_filename, name, fwidth, fheight, out_filename):
    res = []
    temp = 0
    with Image.open(in_filename) as im:
        width, height = im.size
        for i in range(0, width * height):
            x = i % width
            y = (int(i / width) * fheight % height) + int((int(i / width) * fheight) / height)
            print(x, y)
            if im.getpixel((x, y)) == 1:
                temp += 2 ** (7 - (i % 8))
            if i % 8 == 7:
                res.append(temp)
                temp = 0
    s = ""
    for r in res:
        s += str(hex(r)) + ", "
    with open(out_filename, "w") as f:
        f.write("uint8_t {}[{}] = {} {}{};".format(name, len(res), "{", s, "}"))

compacter("font8x12.png", "font_8_12", 8, 12, "font8x12.txt")
#compacter("font12x18.png", "font_12_18", 12, 18, "font12x18.txt")