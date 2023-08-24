#include "dev/codec_ak4619.h"
#include "sys/system.h"

#define I2C_ADDR 0x20 // I2C slave address (0x10<<1) and RW bit (W=0)

// Datasheet: 9.14. Register Map

#define NUM_REG 21
const uint8_t config[NUM_REG] = {
    0x37, // 0b.0011.0111 - 0x00 Power Management (all Normal Operation) // TODO: only release reset State (D0 = 1) at end of setup?!
 // 0xAC, // 0b.1010.1100 - 0x01 Audio I/F Format (TDM256 mode I2S compatible, Figure 19, DCF=010, DSL=11=32bit, BCKP=0=Falling with LRCK, SDOPH=0=Slow)
    0xAE, // 0b.1111.1110 - 0x01 Audio I/F Format (TDM128 mode MSB justified, Figure 18, DCF=111, DSL=11=32bit, BCKP=1=Rising  with LRCK, SDOPH=0=Slow)
    0x1C, // 0b.0001.1100 - 0x02 Reset Control =~ Audio I/F Format 2 (DIDL=11=32-Bit, DODL=00=24-Bit
 // 0x04, // 0b.0000.0100 - 0x03 System Clock Setting (FS=100 = 192kHz)
    0x00, // 0b.0000.0000 - 0x03 System Clock Setting (FS=000 = MCLK=256*fs, BICK=128*fs, fs=48khz )
    0x22, // 0b.0010.0010 - 0x04 MIC AMP Gain (0010 = 0dB, 0010 = 0dB) 
    0x22, // 0b.0010.0010 - 0x05 MIC AMP Gain (0010 = 0dB, 0010 = 0dB)
    0x30, // 0b.0011.0000 - 0x06 ADC1 Lch Digital Volume (0011 = 0dB) 
    0x30, // 0b.0011.0000 - 0x07 ADC1 Rch Digital Volume (0011 = 0dB) 
    0x30, // 0b.0011.0000 - 0x08 ADC2 Lch Digital Volume (0011 = 0dB) 
    0x30, // 0b.0011.0000 - 0x09 ADC2 Rch Digital Volume (0011 = 0dB) 
    0x22, // 0b.0010.0010 - 0x0A ADC Digital Filter Setting (both Short Delay Sharp Roll-Off Filter)
    0x55, // 0b.0101.0101 - 0x0B ADC Analog Input Setting (all Single-Ended1 = AIN1L, AIN1R, AIN4L, AIN4R)
    0x00, // 0b.0000.0000 - 0x0C Reserved
    0x06, // 0b.0000.0110 - 0x0D ADC Mute & HPF Control (ADC1/2 Soft Mute Disable, ADC1/2 HPF = 1 = Disable) --> AD1HPFN: ADC1 DC Offset Cancel HPF 
    0x18, // 0b.0001.1000 - 0x0E DAC1 Lch Digital Volume (0x18 = 0dB)
    0x18, // 0b.0001.1000 - 0x0F DAC1 Rch Digital Volume (0x18 = 0dB)
    0x18, // 0b.0001.1000 - 0x10 DAC2 Lch Digital Volume (0x18 = 0dB)
    0x18, // 0b.0001.1000 - 0x11 DAC2 Rch Digital Volume (0x18 = 0dB)
    0x04, // 0b.0000.0100 - 0x12 DAC Input Select Setting (DAC1 = SDIN1, DAC2=SDIN2) (can be used to route audio from ADC directly to DAC)
    0x05, // 0b.0000.0101 - 0x13 DAC De-Emphasis Setting (off)
    0x0A  // 0b.0000.1010 - 0x14 DAC Mute & Filter Setting
};

namespace daisy
{
AK4619::Result AK4619::Init(I2CHandle i2c)
{
    i2c_ = i2c;

    // TODO: Reset the codec (though by default we may not need to do this)
    for(int i = 0; i < NUM_REG; i++)
    {
        if(WriteRegister(i, config[i]) != Result::OK)
            return Result::ERR;

        System::Delay(4);
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
