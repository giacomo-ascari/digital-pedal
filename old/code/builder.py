import os

def list_files(startpath):
    c_files = []
    h_files = []
    for root, dirs, files in os.walk(startpath):
        level = root.replace(startpath, '').count(os.sep)
        for f in files:
            #print('{} {} {}'.format(level, f, f[-1:]))
            if f[-6:] != '.min.c' and f[-6:] != '.min.h':
                if f[-1:] == 'c':
                    c_files.append((os.path.join(root, f),level))
                elif f[-1:] == 'h':
                    h_files.append((os.path.join(root, f),level))
    return c_files, h_files

def append_command_set(command_set, file_list, command):
    for c in file_list:
        with open(c[0], "r") as f:
            for l in f.read().splitlines():
                if l[:len(command)] == command:
                    command_set.add(l)

def append_content(line_list, file_list):
    for c in file_list:
        with open(c[0], "r") as f:
            for l in f.read().splitlines():
                if l[:1] != "#":
                    line_list.append(l)

def main():

    c_files, h_files = list_files("./lib")
    c_files = sorted(c_files, key=lambda tup: tup[1], reverse=True)
    h_files = sorted(h_files, key=lambda tup: tup[1], reverse=False)

    define_set = set()
    append_command_set(define_set, c_files, "#define")
    append_command_set(define_set, h_files, "#define")
    include_set = set()
    append_command_set(include_set, c_files, "#include <")
    append_command_set(include_set, h_files, "#include <")

    source_list = []
    append_content(source_list, c_files)

    header_list = []
    append_content(header_list, h_files)
    
    with open("lib/pedalboard.min.h", "w") as f:
        for line in include_set:
            f.write(line + "\n")
        for line in define_set:
            f.write(line + "\n")
        for line in header_list:
            f.write(line + "\n")
    
    with open("lib/pedalboard.min.c", "w") as f:
        for line in include_set:
            f.write(line + "\n")
        f.write("#include \"pedalboard.min.h\"\n")
        for line in source_list:
            f.write(line + "\n")

if __name__ == "__main__":
    main()