# gdb server
This project contains a simple gdb-server implementation. Supports [GDB Remote Serial Protocol](https://sourceware.org/gdb/current/onlinedocs/gdb.html/Remote-Protocol.html).
## Usage
Follow this to compile project

    mkdir build
    cd build
    cmake ..
    make
The output will be a file `server`\
Now run `server` and pass the port number and full path to the executable file as a command line argument. The server will wait for a connection to gdb

Run gdb on the host and connect to the remote server

    gdb
    target remote server_ip_adress

Write your server adress instead of server_ip_adress\
You can now use gdb on the host as usual

