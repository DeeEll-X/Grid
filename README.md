# Grid

Grid is an Open Contaner Initiative Runtime. The Open Container Initiative Runtime Specification can be found [here](https://github.com/opencontainers/runtime-spec).

## How to Run Grid

### prepare your rootdir

rootdir is the directory to store containers' information. Its structure is like
```
<rootdir>
├── containers
│   └── testContainer2
│       ├── mntFolder
│       │   ├── bin
│       │   ├── config.json
│       │   ├── dev
│       │   ├── etc
│       │   ├── home
│       │   ├── proc
│       │   ├── root
│       │   ├── sys
│       │   ├── tmp
│       │   ├── usr
│       │   └── var
│       ├── ns
│       │   ├── ipc
│       │   ├── mnt
│       │   ├── net
│       │   └── uts
│       ├── status.json
│       └── writeLayer
└── images
    └── busybox
        ├── bin
        ├── config.json
        ├── dev
        ├── etc
        ├── home
        ├── proc
        ├── root
        ├── sys
        ├── tmp
        ├── usr
        └── var
```
The `<rootdir>/containers/` folder stores created container informations. 
Directories in this folder named by the containers' ID. They are created by Grid.

Before running Grid,  you should define a environment variable named "GRID_CONFIG" to store rootdir value. Better store it in .bashrc .

### prepare bundle

[bundle.md](https://github.com/opencontainers/runtime-spec/blob/master/bundle.md) introduce what is filesystem bundle.

For example, busybox image partly can be created by docker:
```
export containerId=`docker create busybox`
docker export ${containerId} -o busybox.tar
tar -xf busybox.tar -C rootdir/containers/testContainer/mntFolder
docker rm ${containerId}
rm busybox.tar
```
`config.md` is included in bundle, but it need to be self-defined. You can refer to [config.md](https://github.com/opencontainers/runtime-spec/blob/master/config.md)

When coding, I tested by placing the bundle in `<rootdir>/images/`. A busybox image with config.json looks like:
```
└── images
    └── busybox
        ├── bin
        ├── config.json
        ├── dev
        ├── etc
        ├── home
        ├── proc
        ├── root
        ├── sys
        ├── tmp
        ├── usr
        └── var
```

###  run Grid

First, enter root and export `GRID_CONFIG`. Root privilege is needed here since Grid need to mount and umount namespace.
```
sudo su root
export GRID_CONFIG = <rootdir>
cd build; cmake ..; make
cd binary
```
basic operation
```
./Grid create \<ContainerID\> \<path-to-bundle\>
./Grid start \<ContainerID\>
./Grid kill \<ContainerID\>
./Grid delete \<ContainerID\>
./Grid state \<ContainerID\>
```
format code
```
.<Grid path>/format.sh
```
run unit test
```
cd build/tests
sudo ./unit_tests
```
