# Chat system
---

## Overview

This is a project aimed to learn the basics of socket programming. The language that it was written in is C (POSIX).

The program uses pthread and UNIX sockets, so it must be compiled either on linux, or windows using WSL.

## Compilation

You can compile it using cmake as follows:
```
cd chat-system
mkdir build
cd build
cmake .. -G "Unix Makefiles"
cmake . --build
```

## Usage 

To use the program, you have to first run the server, and then the client, they both have hardcoded credentials for 
localhost:8083.

The client has to have a parameter passed for the username.

Launching the server:
```
./chat_server
```

Launching the client:
```
./chat_client <username>
```

You can launch as many clients as you want, up to the limit of you system.

Once you are connected, you can send a message and the server will broadcast it to all of the other clients.