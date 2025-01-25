Using Direct Memory Access with stm32 and libopencm3.


## Run
To build the project, navigate to the firmware folder and run
```console

make

```

to compile the project and produce the image.

then run
```console

make burn

```
to burn the image to the Microcontroller flash memory

use `screen.sh` to monitor uart output
```terminal
./screen.sh

DMA Started ...
Source -> 124
Distination -> 123
Data transferred to distination
Source -> 124
Distination -> 124

DMA Started ...
Source -> 125
Distination -> 124
Data transferred to distination
Source -> 125
Distination -> 125

```