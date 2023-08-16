#include "dev/codec_ak4619.h"
#include "sys/system.h"

// TODO:
// *

// Datasheet: 9.14. Register Map

#define I2C_ADDR 0x20 // I2C slave address (0x10<<1) and RW bit (W=0)

const uint8_t config[] = {
    0x37, // 0x00 Power Management
    0xAC, // 0x01 Audio I/F Format
    0x1C, // 0x02 Audio I/F Format
    0x04, // 0x03 System Clock Setting
    0x22, // 0x04 MIC AMP Gain
    0x22, // 0x05 MIC AMP Gain
    0x30, // 0x06 ADC1 Lch Digital Volume
    0x30, // 0x07 ADC1 Rch Digital Volume
    0x30, // 0x08 ADC2 Lch Digital Volume
    0x30, // 0x09 ADC2 Rch Digital Volume
    0x22, // 0x0A ADC Digital Filter Setting
    0x55, // 0x0B ADC Analog Input Setting
    0x00, // 0x0C Reserved
    0x06, // 0x0D ADC Mute & HPF Control
    0x18, // 0x0E DAC1 Lch Digital Volume
    0x18, // 0x0F DAC1 Rch Digital Volume
    0x18, // 0x10 DAC2 Lch Digital Volume
    0x18, // 0x11 DAC2 Rch Digital Volume
    0x04, // 0x12 DAC Input Select Setting
    0x05, // 0x13 DAC De-Emphasis Setting
    0x0A  // 0x14 DAC Mute & Filter Setting
};

#define LED_DRIVER_PC9635_ADDR \
    0x0A // I2C slave address (0x5<<1) and RW bit (W=0)

const uint8_t led_reg_vals[] = {
    0x81, // MODE1
    0x01, // MODE2
    0x10, // PWM0
    0x10, // PWM1
    0x08, // PWM2
    0x04, // PWM3
    0x02, // PWM4
    0x01, // PWM5
    0x00, // PWM6
    0x10, // PWM7
    0x10, // PWM8
    0x10, // PWM9
    0x10, // PWM10
    0x10, // PWM11
    0x10, // PWM12
    0x10, // PWM13
    0x10, // PWM14
    0x10, // PWM15
    0xFF, // GRPPWM
    0x00, // GRPFREQ
    0xAA, // LEDOUT0
    0xAA, // LEDOUT1
    0xAA, // LEDOUT2
    0xAA  // LEDOUT3
};


#define REG_POWER_MANAGEMENT 0x00           // 0x37
#define REG_AUDIO_IF_FORMAT_1 0x01          // 0xAC
#define REG_AUDIO_IF_FORMAT_2 0x02          // 0x1C
#define REG_SYSTEM_CLOCK_SETTING 0x03       // 0x04
#define REG_MIC_AMP_GAIN_1 0x04             // 0x22
#define REG_MIC_AMP_GAIN_2 0x05             // 0x22
#define REG_ADC1_LCH_DIGITAL_VOLUME 0x06    // 0x30
#define REG_ADC1_RCH_DIGITAL_VOLUME 0x07    // 0x30
#define REG_ADC2_LCH_DIGITAL_VOLUME 0x08    // 0x30
#define REG_ADC2_RCH_DIGITAL_VOLUME 0x09    // 0x30
#define REG_ADC_DIGITAL_FILTER_SETTING 0x0A // 0x22
#define REG_ADC_ANALOG_INPUT_SETTING 0x0B   // 0x55
#define REG_RESERVED 0x0C                   // 0x00
#define REG_ADC_MUTE_HPF_CONTROL 0x0D       // 0x06
#define REG_DAC1_LCH_DIGITAL_VOLUME 0x0E    // 0x18
#define REG_DAC1_RCH_DIGITAL_VOLUME 0x0F    // 0x18
#define REG_DAC2_LCH_DIGITAL_VOLUME 0x10    // 0x18
#define REG_DAC2_RCH_DIGITAL_VOLUME 0x11    // 0x18
#define REG_DAC_INPUT_SELECT_SETTING 0x12   // 0x04
#define REG_DAC_DE_EMPHASIS_SETTING 0x13    // 0x05
#define REG_DAC_MUTE_FILTER_SETTING 0x14    // 0x0A

namespace daisy
{
AK4619::Result AK4619::Init(I2CHandle i2c)
{
    i2c_ = i2c;

    // Reset the codec (though by default we may not need to do this)
    uint8_t sysreg = 0x00;

    for(int i = 0; i <= REG_DAC_MUTE_FILTER_SETTING; i++)
    {
        if(WriteRegister(i, config[i]) != Result::OK)
            return Result::ERR;

        System::Delay(4);
    }

    for(int i = 0; i < 24; i++)
    {
        uint8_t val = led_reg_vals[i];
        if(i2c_.WriteDataAtAddress(LED_DRIVER_PC9635_ADDR, i, 1, &val, 1, 250)
           != I2CHandle::Result::OK)
        {
            return Result::ERR;
        }
    }


    // Success
    return Result::OK;
}

AK4619::Result AK4619::ReadRegister(uint8_t addr, uint8_t* data)
{
    if(i2c_.ReadDataAtAddress(I2C_ADDR, addr, 1, data, 1, 250)
       != I2CHandle::Result::OK)
    {
        return Result::ERR;
    }
    return Result::OK;
}

AK4619::Result AK4619::WriteRegister(uint8_t addr, uint8_t val)
{
    if(i2c_.WriteDataAtAddress(I2C_ADDR, addr, 1, &val, 1, 250)
       != I2CHandle::Result::OK)
    {
        return Result::ERR;
    }
    return Result::OK;
}

} // namespace daisy
