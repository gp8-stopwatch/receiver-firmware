See [link link](http://) to see more.
Buy [on Tinidie](http://) TODO.

# Firmware upgrade
Manjaro : install dfu-util

```
dfu-util -D receiver.bin -d 0483:df11 -a0 --dfuse-address 0x08000000
```

git clone git@github.com:majbthrd/elf2dfuse.git
elf2dfuse receiver.elf receiver.dfu

dfu-util -D receiver.dfu -a0
