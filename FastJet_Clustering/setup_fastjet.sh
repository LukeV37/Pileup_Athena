curl -O https://fastjet.fr/repo/fastjet-3.5.1.tar.gz
tar zxvf fastjet-3.5.1.tar.gz
rm fastjet-3.5.1.tar.gz

cd fastjet-3.5.1
./configure --prefix=$PWD/fastjet-install
make
make check
make install
cd ..
