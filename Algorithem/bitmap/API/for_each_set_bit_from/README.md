for_each_set_bit_from [中文教程](https://biscuitos.github.io/blog/BITMAP_for_each_set_bit_from/)
----------------------------------

Iterate over set bit from pos.

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
