#include "pcf-8574.h"

void PCF8574::read() {
  auto response = i2c_smbus_read_byte_data(file, 0xff);
		if (response < 0) {
			//std::cerr << "Error reading device" << '\n';
			return;
		}
		auto new_state = std::bitset<8>(response);
		state = new_state;
}

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

void EncoderGroup::read() {
	auto prevState = readEncoderData(state);
	PCF8574::read();
	auto currState = readEncoderData(state);
	for (int i{0}; i < 4; i++) {
		int direction = getRotationDirection(prevState[i], currState[i]);
		if (direction != 0)
			std::cout << "Encoder " << i << " turned " << direction << '\n';
	}
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
    if (stack == 0b1110 || stack == 0b1000 || stack == 0b0001 || stack == 0b0111) {
        return 1; // Clockwise
    } else if (stack == 0b0100 || stack == 0b0010 || stack == 0b1011 || stack == 0b1101) {
        return -1; // Counterclockwise
    } else {
        return 0; // No rotation
    }
}