# 4InARow

Project for the Foundations of Cybersecurity course of the Master of Science
in Computer Engineering, University of Pisa.

The project consists in the design and implementation of a secure online version of Four in a Row,
written in C++. The communications are confidential, authenticated and protected against replay attacks.
For more details, please refer to the documentation.

## Overview
The repository is organised in the following way:
- _src/client_ contains the client application.
- _src/crypto_ contains the cryptographic library.
- _src/exception_ contains the custom exceptions used in the code.
- _src/game_ contains the manager of the game board and the player class.
- _src/message_ contains the messages exchanged between parties.
- _src/server_ contains the server application.
- _src/socket_ contains the networking library.
- _src/utils_ contains utility functions and constants.

## Requirements
- A Unix-like OS. The code has been tested on Ubuntu 20.04.3 LTS. 
- A C++ compiler supporting C++14.
- CMake 3.16+. Older versions should work too (not tested).
- OpenSSL 1.1.1+. The most recent version of OpenSSL can be installed running:
```bash
  sudo apt-get install libssl-dev
```

## Installation and execution
A user is registered if its RSA-2048 private-public key pair is stored in ```src/client/keys``` and
the same public key is stored in ```src/server/players```. The file names must follow the pattern  ```username``` + 
```_pubkey.pem/_privkey.pem```. All the provided private keys have password ```unipi```, which is asked at runtime.

To execute the application:
- Inside the root folder, create a compilation directory, for example called ```build-dir```:
  ```bash
  mkdir build-dir
  ```
- Go inside the folder and compile the project:
  ```bash
  cd build-dir
  cmake ..
  make -C src/server/ all
  make -C src/client/ all
  ```
- Start the server, choosing a private IPv4 address:
  ```bash
  cd src/server
  ./server --address 127.0.0.1
  ```
- Run another shell and start the first client, choosing a private IPv4 address:
  ```bash
  cd src/client
  ./client --username Alice --client 127.0.0.2 --server 127.0.0.1
  ```
- Run another shell and start the second client, choosing a private IPv4 address:
  ```bash
  cd src/client
  ./client --username Bob --client 127.0.0.3 --server 127.0.0.1
  ```
- Play!

The application uses the ports 5000 and 5001. If necessary, they can be changed by modifying the
variables ```SERVER_PORT``` and ```PLAYER_PORT``` in ```src/utils/Constants.cpp```.