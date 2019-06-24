BITS_PER_LONG_LONG [中文教程](https://biscuitos.github.io/blog/BITMAP_BITS_PER_LONG_LONG/)
----------------------------------

The bit number for double-word (unsigned long long).

Context:

* Driver Files: bitmap.c

## Usage

Copy Driver Files into `/drivers/xxx/`, and modify Makefile on current 
directory, as follow:

```
obj-$(CONFIG_BITMAP_XX) += bitmap.o
```

Then, compile driver or dts. Details :

```
make
```

## Running

Packing image and runing on the target board.