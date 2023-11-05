#include "pcf-8574.h"

template<size_t S, typename T>
static bool compareArrays(std::array<T, S> prev, std::array<T, S> curr)
{
	bool equal = true;
	for (int i{0}; i < S; i++) {
		if (prev[i] != curr[i]) {
			equal = false;
			break;
		}
	}
	return equal;
}

//------------------------------// 

const std::array<int, 4>& EncoderGroup::read() {
	// generate 2 arrays, before read and after read
	auto prevState = readEncoderData(raw_state);
	read_raw_state();
	auto currState = readEncoderData(raw_state);

	for (int i{0}; i < 4; i++) { 
		state[i] = getRotationDirection(prevState[i], currState[i]);
	}
	return state;
}

std::array<int, 4> EncoderGroup::readEncoderData(const std::bitset<8>& state) {
	auto intValue = state.to_ulong();
	int v1 = (intValue & 0x03); // Mask with 0b00000011 to get the lowest 2 bits.
	int v2 = ((intValue >> 2) & 0x03); // Shift and mask for 2.
	int v3 = ((intValue >> 4) & 0x03); // Shift and mask for 3.
	int v4 = ((intValue >> 6) & 0x03);
	return {v1, v2, v3, v4};
}

int EncoderGroup::getRotationDirection(int previousState, int currentState) {
	std::bitset<4> stack((previousState << 2) | currentState);
	if (stack == cw[0] || stack == cw[1] || stack == cw[2] || stack == cw[3]) {
		return 1; // Clockwise
	} else if (stack == ccw[0] || stack == ccw[1] || stack == ccw[2] || stack == ccw[3]) {
		return -1; // Counterclockwise
	} else {
		return 0; // No rotation
	}
}
