mkdir -p rootdir/containers/testContainer/mntFolder
echo \
"{ 
    \"ociVersion\" : \"1.0.0\",
    \"id\" : \"testContainer\",
    \"status\" : \"created\", 
    \"pid\" : 0,
    \"bundle\" : \"rootdir/containers/testContainer\"
} " > rootdir/containers/testContainer/status.json
echo  \
"{
    "process": {
        "terminal": true,
        "env": [
            "PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin",
            "TERM=xterm"
        ],
        "cwd": "/root",
        "args": [
            "echo",
            "1"
        ]
    }
}" > rootdir/containers/testContainer/config.json