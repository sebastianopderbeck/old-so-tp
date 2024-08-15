echo Cleanning logs
sleep 1

echo Cleanning MEMORY logs
cd ../memory/logs
rm memory.log
cd ../..
sleep 1

echo Cleanning CPU logs
cd cpu/logs
rm cpu.log
cd ../..
sleep 1

echo Cleanning KERNEL logs
cd kernel/logs
rm kernel.log
cd ../..
sleep 1

echo Cleanning CONSOLE logs
cd console/logs
rm console.log
cd ../..
sleep 1

echo Logs cleaned
