from PIL import Image

def compacter(in_filename, name, fwidth, fheight, out_filename):
    res = []
    temp = 0
    with Image.open(in_filename) as im:
        i = 0
        for c in range(0, 64):
            basex = c % 8 * fwidth
            basey = int(c / 8) * fheight
            for y in range(0, fheight):
                for x in range(0, fwidth):

                    pixel = im.getpixel((basex + x, basey + y))
                    if isinstance(pixel, int):
                        if pixel == 1:
                            temp += 2 ** (7 - (i % 8))
                    elif isinstance(pixel, tuple):
                        if pixel[0] == 0:
                            temp += 2 ** (7 - (i % 8))

                    if i % 8 == 7:
                        print(hex(temp))
                        res.append(temp)
                        temp = 0
                    i += 1
            #input()

            
    s = ""
    for r in res:
        s += str(hex(r)) + ", "
    with open(out_filename, "w") as f:
        f.write("uint8_t {}[{}] = {} {}{};".format(name, len(res), "{", s, "}"))

compacter("font8x12.png", "font_8_12", 8, 12, "font8x12.txt")
compacter("font10x14.png", "font_10_14", 10, 14, "font10x14.txt")
compacter("font12x18.png", "font_12_18", 12, 18, "font12x18.txt")