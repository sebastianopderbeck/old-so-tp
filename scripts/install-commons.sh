echo Starting initial configuration
cd ~
echo 1- Cloning commons
sleep 2
git clone https://github.com/sisoputnfrba/so-commons-library/

echo 2- Installing commons...
sleep 2
cd so-commons-library
make install

echo Commons has been installed