from PIL import Image

def compacter(in_filename, name, out_filename):
    res = []
    temp = 0
    with Image.open(in_filename) as im:
        width, height = im.size
        for h in range(0, height):
            for w in range(0, width):
                index = width * h + w
                if im.getpixel((w, h)) == 1:
                    temp += 2 ** (7 - (index % 8))
                if index % 8 == 7:
                    res.append(temp)
                    temp = 0
    s = ""
    for r in res:
        s += str(hex(r)) + ", "
    with open(out_filename, "w") as f:
        f.write("uint8_t {}[{}] = {} {}{};".format(name, len(res), "{", s, "}"))

compacter("font8x12.png", "font_8_12", "font8x12.txt")
compacter("font12x18.png", "font_12_18", "font12x18.txt")