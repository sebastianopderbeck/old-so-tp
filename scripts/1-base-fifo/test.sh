echo Executing FILESYSTEM module through a terminal
cd ../../filesystem
gnome-terminal -- ./filesystem

echo Executing MEMORY module through a terminal
cd ../memory
gnome-terminal -- ./memory

echo Executing CPU module through a terminal
cd ../cpu
gnome-terminal -- ./cpu

echo Executing KERNEL module through a terminal
cd ../kernel
gnome-terminal -- ./kernel

echo Executing CONSOLE module
cd ../console
gnome-terminal -x bash -c './console ../scripts/1-base-fifo/TEST_1; exec bash'