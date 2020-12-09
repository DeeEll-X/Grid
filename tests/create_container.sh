mkdir -p rootdir/containers/testContainer/mntFolder
echo \
"{ 
    \"OCIVersion\" : \"1.0.0\",
    \"ID\" : \"testContainer\",
    \"Status\" : \"created\", 
    \"Pid\" : 0,
    \"Bundle\" : \"rootdir/containers/testContainer\"
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