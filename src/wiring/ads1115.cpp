#include "ads1115.hpp"
#include <stdexcept>
#include <chrono>
#include <thread>
#include <wiringPiI2C.h>
#include <netinet/in.h>

namespace grower::wiring {
    enum conversion_delay : uint32_t {
        ADS1115_CONVERSION_DELAY_8 = 128000,
        ADS1115_CONVERSION_DELAY_16 = 64000,
        ADS1115_CONVERSION_DELAY_32 = 32000,
        ADS1115_CONVERSION_DELAY_64 = 16000,
        ADS1115_CONVERSION_DELAY_128 = 8000,
        ADS1115_CONVERSION_DELAY_250 = 4000,
        ADS1115_CONVERSION_DELAY_475 = 2200,
        ADS1115_CONVERSION_DELAY_860 = 1200
    };

    enum pointer_register : uint8_t {
        ADS1115_REG_POINTER_MASK = 0x03,
        ADS1115_REG_POINTER_CONVERT = 0x00,
        ADS1115_REG_POINTER_CONFIG = 0x01,
        ADS1115_REG_POINTER_LOWTHRESH = 0x02,
        ADS1115_REG_POINTER_HITHRESH = 0x03
    };

    enum config_register : uint16_t {
        ADS1115_REG_CONFIG_OS_MASK = 0x8000,
        ADS1115_REG_CONFIG_OS_SINGLE = 0x8000,
        ADS1115_REG_CONFIG_OS_BUSY = 0x0000,
        ADS1115_REG_CONFIG_OS_NOTBUSY = 0x8000,

        ADS1115_REG_CONFIG_MUX_MASK = 0x7000,
        ADS1115_REG_CONFIG_MUX_DIFF_0_1 = 0x0000,
        ADS1115_REG_CONFIG_MUX_DIFF_0_3 = 0x1000,
        ADS1115_REG_CONFIG_MUX_DIFF_1_3 = 0x2000,
        ADS1115_REG_CONFIG_MUX_DIFF_2_3 = 0x3000,
        ADS1115_REG_CONFIG_MUX_SINGLE_0 = 0x4000,
        ADS1115_REG_CONFIG_MUX_SINGLE_1 = 0x5000,
        ADS1115_REG_CONFIG_MUX_SINGLE_2 = 0x6000,
        ADS1115_REG_CONFIG_MUX_SINGLE_3 = 0x7000,

        ADS1115_REG_CONFIG_PGA_MASK = 0x0E00,
        ADS1115_REG_CONFIG_PGA_6_144V = 0x0000,
        ADS1115_REG_CONFIG_PGA_4_096V = 0x0200,
        ADS1115_REG_CONFIG_PGA_2_048V = 0x0400,
        ADS1115_REG_CONFIG_PGA_1_024V = 0x0600,
        ADS1115_REG_CONFIG_PGA_0_512V = 0x0800,
        ADS1115_REG_CONFIG_PGA_0_256V = 0x0A00,

        ADS1115_REG_CONFIG_MODE_MASK = 0x0100,
        ADS1115_REG_CONFIG_MODE_CONTIN = 0x0000,
        ADS1115_REG_CONFIG_MODE_SINGLE = 0x0100,

        ADS1115_REG_CONFIG_DR_MASK = 0x00E0,
        ADS1115_REG_CONFIG_DR_8SPS = 0x0000,
        ADS1115_REG_CONFIG_DR_16SPS = 0x0020,
        ADS1115_REG_CONFIG_DR_32SPS = 0x0040,
        ADS1115_REG_CONFIG_DR_64SPS = 0x0060,
        ADS1115_REG_CONFIG_DR_128SPS = 0x0080,
        ADS1115_REG_CONFIG_DR_250SPS = 0x00A0,
        ADS1115_REG_CONFIG_DR_475SPS = 0x00C0,
        ADS1115_REG_CONFIG_DR_860SPS = 0x00E0,


        ADS1115_REG_CONFIG_CMODE_MASK = 0x0010,
        ADS1115_REG_CONFIG_CMODE_TRAD = 0x0000,
        ADS1115_REG_CONFIG_CMODE_WINDOW = 0x0010,

        ADS1115_REG_CONFIG_CPOL_MASK = 0x0008,
        ADS1115_REG_CONFIG_CPOL_ACTVLOW = 0x0000,
        ADS1115_REG_CONFIG_CPOL_ACTVHI = 0x0008,

        ADS1115_REG_CONFIG_CLAT_MASK = 0x0004,
        ADS1115_REG_CONFIG_CLAT_NONLAT = 0x0000,
        ADS1115_REG_CONFIG_CLAT_LATCH = 0x0004,

        ADS1115_REG_CONFIG_CQUE_MASK = 0x0003,
        ADS1115_REG_CONFIG_CQUE_1CONV = 0x0000,
        ADS1115_REG_CONFIG_CQUE_2CONV = 0x0001,
        ADS1115_REG_CONFIG_CQUE_4CONV = 0x0002,
        ADS1115_REG_CONFIG_CQUE_NONE = 0x0003,
    };

    enum ads_gain : uint16_t {
        GAIN_TWOTHIRDS = ADS1115_REG_CONFIG_PGA_6_144V,
        GAIN_ONE = ADS1115_REG_CONFIG_PGA_4_096V,
        GAIN_TWO = ADS1115_REG_CONFIG_PGA_2_048V,
        GAIN_FOUR = ADS1115_REG_CONFIG_PGA_1_024V,
        GAIN_EIGHT = ADS1115_REG_CONFIG_PGA_0_512V,
        GAIN_SIXTEEN = ADS1115_REG_CONFIG_PGA_0_256V
    };

    enum ads_sps : uint16_t {
        SPS_8 = ADS1115_REG_CONFIG_DR_8SPS,
        SPS_16 = ADS1115_REG_CONFIG_DR_16SPS,
        SPS_32 = ADS1115_REG_CONFIG_DR_32SPS,
        SPS_64 = ADS1115_REG_CONFIG_DR_64SPS,
        SPS_128 = ADS1115_REG_CONFIG_DR_128SPS,
        SPS_250 = ADS1115_REG_CONFIG_DR_250SPS,
        SPS_475 = ADS1115_REG_CONFIG_DR_475SPS,
        SPS_860 = ADS1115_REG_CONFIG_DR_860SPS
    };

    ads1115::ads1115(int device_id) {
        _device = wiringPiI2CSetup(device_id);
        if (_device < 0) {
            throw std::runtime_error{"Error opening I2C device"};
        }

        _gain = GAIN_ONE;
        _sps = SPS_860;
        _conversion_delay = ADS1115_CONVERSION_DELAY_860;
    }

    uint16_t ads1115::read_single_channel(uint8_t channel) {
        if (channel > 3) {
            throw std::runtime_error{"Invalid channel, must be < 4"};
        }

        auto config = static_cast<uint16_t>(ADS1115_REG_CONFIG_CQUE_NONE |
                                            ADS1115_REG_CONFIG_CLAT_NONLAT |
                                            ADS1115_REG_CONFIG_CPOL_ACTVLOW |
                                            ADS1115_REG_CONFIG_CMODE_TRAD |
                                            ADS1115_REG_CONFIG_MODE_SINGLE);

        config |= _gain;
        config |= _sps;

        switch(channel) {
            case 0:
                config |= ADS1115_REG_CONFIG_MUX_SINGLE_0;
                break;

            case 1:
                config |= ADS1115_REG_CONFIG_MUX_SINGLE_1;
                break;

            case 2:
                config |= ADS1115_REG_CONFIG_MUX_SINGLE_2;
                break;

            case 3:
                config |= ADS1115_REG_CONFIG_MUX_SINGLE_3;
                break;

            default:
                throw std::runtime_error{"Channel must be <= 3"};
        }

        config |= ADS1115_REG_CONFIG_OS_SINGLE;
        wiringPiI2CWriteReg16(_device, ADS1115_REG_POINTER_CONFIG, htons(config));

        std::this_thread::sleep_for(std::chrono::microseconds{_conversion_delay});

        const auto value = (uint16_t) wiringPiI2CReadReg16(_device, ADS1115_REG_POINTER_CONVERT);
        return htons(value);
    }
}