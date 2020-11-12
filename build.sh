if [ ! -d "build" ]; then
    mkdir build
fi

dir=`pwd`

docker run --rm -v `pwd`:`pwd` xiangyuxin/grid-env:2.0 /bin/bash -c "cd $dir/build && cmake .. && make"