make
for i in 0 1 2 3 4;
do
    ./Reconstruction.out ../data/calo_jet_95k.root ../data/trk_jet_95k.root $i
done
make clean
