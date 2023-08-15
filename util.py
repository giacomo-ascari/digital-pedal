import pyautogui
import time
def print_no_newline(string):
    import sys
    sys.stdout.write("\r")
    sys.stdout.write(string)
    sys.stdout.flush()
try:
    while True:
        x, y = pyautogui.position()
        pixelColor = pyautogui.screenshot().getpixel((x, y))
        ss = 'X: ' + str(x).rjust(4) + ' Y: ' + str(y).rjust(4)
        ss += ' RGB: (' + str(pixelColor[0]).rjust(3)
        ss += ', ' + str(pixelColor[1]).rjust(3)
        ss += ', ' + str(pixelColor[2]).rjust(3) + ')'
        print_no_newline(ss)
        time.sleep(1.0)
except KeyboardInterrupt:
    print("\nDone...")