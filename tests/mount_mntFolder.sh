export containerId=`docker create busybox`
docker export ${containerId} -o busybox.tar
tar -xf busybox.tar -C rootdir/containers/testContainer/mntFolder
docker rm ${containerId}
rm busybox.tar