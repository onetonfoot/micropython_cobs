# Micropython Cobs

A [native c module](https://docs.micropython.org/en/latest/develop/natmod.html) for [COBS](https://en.wikipedia.org/wiki/Consistent_Overhead_Byte_Stuffing) encoding in Micropython. 

Before building the module install the dependencies.

```sh
git clone https://github.com/micropython/micropython.git
sudo mv micropython /opt/
cd /opt/micropython
git tag -l 
git checkout tags/v1.19.1
pip install pyelftools
```

Next to generate the `.mpy` file set your ARCH and run

```sh
# Architecture to build for (x86, x64, armv6m, armv7m, xtensa, xtensawin)
make ARCH=armv6m
# Copy the byte code onto your board
rshell cp cobs.mpy /pyboard/cobs.mpy
```

Example usage

```py
import cobs
# supports bytes bytearray and strings
data = bytearray([0,1,2])
encoded_data = cobs.encode(data)
data == cobs.decode(encoded_data)
```