if [ ! -d "build" ]; then
    mkdir build
fi

dir=`pwd`

docker run --rm -v `pwd`:`pwd` veiasai/grid-env:1.0 /bin/bash -c "cd $dir/build && cmake .. && make"