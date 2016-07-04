#pragma once

#include <enet/enet.h>

#include <chrono>
#include <thread>

std::time_t timestamp();

void sleepMS(uint32_t);

std::string addressToString(const ENetAddress*);
