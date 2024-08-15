echo Starting all configurations
sleep 2

echo Starting MEMORY
cd ../memory
make clean
make
cd ..
sleep 2

echo Starting CPU
cd cpu
make clean
make
cd ..
sleep 2

echo Starting KERNEL
cd kernel
make clean
make
cd ..
sleep 2

echo Starting CONSOLE
cd console
make clean
make
cd ..
sleep 2

echo All modules were compiled
