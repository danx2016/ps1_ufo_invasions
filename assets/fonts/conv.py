print("hello")

inFile = "prst2p8.fnt"
outFile = "prst2p8.bin"

newFileBytes = []
char_index = 0

with open(inFile) as f:
    for line in f:
        line = line.strip()

        # common
        if (line.startswith("common")):
            print(line)
            props = line.split(" ")
            for prop in props:
                print(prop)
                #	uint16_t line_height;
                if (prop.startswith("lineHeight")):
                    props = prop.split("=")
                    print(props[0], "===", props[1])
                    newFileBytes.append(int(props[1]))

                #	uint16_t base;
                elif (prop.startswith("base")):
                    props = prop.split("=")
                    print(props[0], "===", props[1])
                    newFileBytes.append(int(props[1]))

                #	uint16_t width;
                elif (prop.startswith("scaleW")):
                    props = prop.split("=")
                    print(props[0], "===", props[1])
                    newFileBytes.append(int(props[1]))

                #   uint16_t height;
                elif (prop.startswith("scaleH")):
                    props = prop.split("=")
                    print(props[0], "===", props[1])
                    newFileBytes.append(int(props[1]))

                #	uint16_t pages;
                elif (prop.startswith("pages")):
                    props = prop.split("=")
                    print(props[0], "===", props[1])
                    newFileBytes.append(int(props[1]))

        # char
        if (line.startswith("char")):
            print(line)
            props = line.split(" ")
            for prop in props:
                if prop.strip() == "":
                    continue
                
                #print(prop)

                # id
                if (prop.startswith("id")):
                    props = prop.split("=")
                    print(props[0], "===", props[1])
                    id = int(props[1])
                    while (id > char_index):
                        newFileBytes.append(0) #1
                        newFileBytes.append(0) #2
                        newFileBytes.append(0) #3
                        newFileBytes.append(0) #4
                        newFileBytes.append(0) #5
                        newFileBytes.append(0) #6
                        newFileBytes.append(0) #7
                        newFileBytes.append(0) #8
                        print("filling char ", char_index, "with 0 ..")
                        char_index += 1

                    char_index += 1
                
                # uint16_t x;
                if (prop.startswith("x")):
                    props = prop.split("=")
                    #print(props[0], "===", props[1])
                    newFileBytes.append(int(props[1]))

                # uint16_t y;
                elif (prop.startswith("y")):
                    props = prop.split("=")
                    #print(props[0], "===", props[1])
                    newFileBytes.append(int(props[1]))

                # uint16_t width;
                elif (prop.startswith("width")):
                    props = prop.split("=")
                    #print(props[0], "===", props[1])
                    newFileBytes.append(int(props[1]))

                # uint16_t height;
                elif (prop.startswith("height")):
                    props = prop.split("=")
                    #print(props[0], "===", props[1])
                    newFileBytes.append(int(props[1]))

                # int16_t x_offset;
                elif (prop.startswith("xoffset")):
                    props = prop.split("=")
                    #print(props[0], "===", props[1])
                    newFileBytes.append(int(props[1]))

                # int16_t y_offset;
                elif (prop.startswith("yoffset")):
                    props = prop.split("=")
                    #print(props[0], "===", props[1])
                    newFileBytes.append(int(props[1]))

                # uint16_t x_advance;
                elif (prop.startswith("xadvance")):
                    props = prop.split("=")
                    #print(props[0], "===", props[1])
                    newFileBytes.append(int(props[1]))

                # uint16_t page;
                elif (prop.startswith("page")):
                    props = prop.split("=")
                    #print(props[0], "===", props[1])
                    newFileBytes.append(int(props[1]))

print("char_index=", char_index)
id = 255
while (id >= char_index):
    newFileBytes.append(0) #1
    newFileBytes.append(0) #2
    newFileBytes.append(0) #3
    newFileBytes.append(0) #4
    newFileBytes.append(0) #5
    newFileBytes.append(0) #6
    newFileBytes.append(0) #7
    newFileBytes.append(0) #8
    print("filling char ", char_index, "with 0 ..")
    char_index += 1

# reserve space for TIM_IMAGE and size_t (24 bytes)
for i in range(0, 24):
    newFileBytes.append(0)

print("newFileBytes=", newFileBytes)

# make binary file
newFile = open(outFile, "wb")
# write to file
for byte in newFileBytes:
    newFile.write(byte.to_bytes(2, byteorder='little', signed=True))