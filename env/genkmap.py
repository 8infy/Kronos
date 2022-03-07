import fileinput

with open("kmap.bin", "wb") as f:
	for line in fileinput.input():
		line = line.rstrip()
		addr = int(line[:16], base=16)
		name = line[17:]

		f.write(addr.to_bytes(8, byteorder='little', signed=False))
		f.write(len(name).to_bytes(8, byteorder='little', signed=False))
		f.write(name.encode())
		f.write(int(0).to_bytes(1, byteorder='little'))
