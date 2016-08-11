#pragma once

enum class DeliveryType {
	RELIABLE,
	UNRELIABLE
};

enum ChannelIDs {
	RELIABLE_CHANNEL = 0,
	UNRELIABLE_CHANNEL,
	NUM_CHANNELS
};
