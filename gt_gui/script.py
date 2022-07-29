import os

def user_input(prefix, values):
    res = ""
    while res not in values:
        res = input(prefix)
    return res

def user_input_int(prefix, min, max):
    res = ""
    values = []
    for val in range(min, max):
        values.append(str(val))
    while res not in values:
        res = input(prefix)
    return res

def clear():
    os.system('cls' if os.name == 'nt' else 'clear')

def print_pedal(pedal):
    print("0\t|\t+0\t+1\t+2\t+3\t+4\t+5\t+6\t+7")
    print("----------------------------------------------------------------------------")
    print("0\t|\t" + str(pedal[0]))
    for i in range(1, 145, 8):
        line = "{}\t|\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}".format(
            str(i),
            str(pedal[i]),
            str(pedal[i+1]),
            str(pedal[i+2]),
            str(pedal[i+3]),
            str(pedal[i+4]),
            str(pedal[i+5]),
            str(pedal[i+6]),
            str(pedal[i+7]),
        )
        print(line)

def main():

    pedals = []

    action = user_input("Open or create new file? [o/c] ", ["o", "c"])

    path = ""

    if action == "o":
        while path == "":
            try:
                path = input("Write filepath to open: ")
                file = open(path, "rb")
                for i in range(6):
                    pedal = bytearray(file.read(145))
                    pedals.append(pedal)
                file.close()
            except:
                print("I/O ERROR")
                path = ""

    else:
        path = input("Write filepath for the new file: ")
        for i in range(6):
            temp = b""
            for j in range(145):
                temp += b"\x00"
            pedal = bytearray(temp)
            pedals.append(pedal)

    while True:
        clear()
        print("0) Close \n1) View raw pedals \n2) View single pedal\n3) Edit pedal byte\n4) Save and close")
        choice = user_input("> ", ["0", "1", "2", "3", "4"])
        
        if choice == "0":
            return
        else:
            if choice == "1":
                print(pedals)
            elif choice == "2":
                index = int(user_input_int("Pedal index: [0-5] ", 0, 6))
                print_pedal(pedals[index])
            elif choice == "3":
                index = int(user_input_int("Pedal index: [0-5] ", 0, 6))
                print_pedal(pedals[index])
                byte = int(user_input_int("Byte index: [0-145] ", 0, 145))
                pedals[index][byte] = int(user_input_int("New value: [0-255] ", 0, 256))
            elif choice == "4":
                file = open(path, "wb")
                for i in range(6):
                    file.write(pedals[i])
                file.close()
                return
        
        input("\nPress enter to continue ")

if __name__ == '__main__':
    main()