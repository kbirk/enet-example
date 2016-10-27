# ENet Example

> A simple ENet client/server implementation using ENet.

## Dependencies

#### Linux

Install CMake:

```bash
sudo aptitude install cmake
```

Install Dependencies:

```bash
sudo aptitude install libglm-dev
sudo aptitude install libsdl2-dev
sudo aptitude install libenet-dev
sudo aptitude install libepoxy-dev
```

#### OSX

Install CMake:

```bash
brew install cmake
```

Install Dependencies:

```bash
brew install glm
brew install sdl2
brew install enet
brew install libepoxy
```

## Build

Clone the repo:

```bash
git clone git@github.com:kbirk/enet-example.git
```

Build the Makefile:

```bash
cd enet-example
mkdir build
cd build
cmake ../
```

Build the executables:

```bash
make
```

## Usage

Run the server executable:

```bash
./server
```

Run a client executable:

```bash
./client
```
