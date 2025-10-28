cd fastjet-3.5.1
./configure --prefix=$PWD/fastjet-install
make
make check
make install
cd ..
