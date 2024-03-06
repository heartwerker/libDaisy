#pragma once
#ifndef DSY_SWITCH_H
#define DSY_SWITCH_H
#include "daisy_core.h"
#include "per/gpio.h"
#include "sys/system.h"

namespace daisy
{
/** 
    Generic Class for handling momentary/latching switches \n 
    Inspired/influenced by Mutable Instruments (pichenettes) Switch classes
    @author Stephen Hensley
    @date December 2019
    @ingroup controls
*/
class Switch
{
  public:
    /** Specifies the expected behavior of the switch */
    enum Type
    {
        TYPE_TOGGLE,    /**< & */
        TYPE_MOMENTARY, /**< & */
    };
    /** Specifies whether the pressed is HIGH or LOW. */
    enum Polarity
    {
        POLARITY_NORMAL,   /**< & */
        POLARITY_INVERTED, /**< & */
    };

    /** Specifies whether to use built-in Pull Up/Down resistors to hold button at a given state when not engaged. */
    enum Pull
    {
        PULL_UP,   /**< & */
        PULL_DOWN, /**< & */
        PULL_NONE, /**< & */
    };

    Switch() {}
    ~Switch() {}

    /** 
    Initializes the switch object with a given port/pin combo.
    \param pin port/pin object to tell the switch which hardware pin to use.
    \param update_rate Does nothing. Backwards compatibility until next breaking update.
    \param t switch type -- Default: TYPE_MOMENTARY
    \param pol switch polarity -- Default: POLARITY_INVERTED
    \param pu switch pull up/down -- Default: PULL_UP
    */
    void
    Init(dsy_gpio_pin pin, float update_rate, Type t, Polarity pol, Pull pu);

    /**
       Simplified Init.
       \param pin port/pin object to tell the switch which hardware pin to use.
       \param update_rate Left for backwards compatibility until next breaking change.
    */
    void Init(dsy_gpio_pin pin, float update_rate = 0.f);

    void Init()
    {

    last_update_ = System::GetNow();
    updated_     = false;
    state_       = 0x00;
    // // Flip may seem opposite to logical direction,
    // // but here 1 is pressed, 0 is not.
    // flip_         = pol == POLARITY_INVERTED ? true : false;
    // hw_gpio_.pin  = pin;
    // hw_gpio_.mode = DSY_GPIO_MODE_INPUT;
    // switch(pu)
    // {
    //     case PULL_UP: hw_gpio_.pull = DSY_GPIO_PULLUP; break;
    //     case PULL_DOWN: hw_gpio_.pull = DSY_GPIO_PULLDOWN; break;
    //     case PULL_NONE: hw_gpio_.pull = DSY_GPIO_NOPULL; break;
    //     default: hw_gpio_.pull = DSY_GPIO_PULLUP; break;
    // }
    // dsy_gpio_init(&hw_gpio_);
    }

    /** 
    Called at update_rate to debounce and handle timing for the switch.
    In order for events not to be missed, its important that the Edge/Pressed checks
    be made at the same rate as the debounce function is being called.
    */
    void Debounce();

    void processDebounce(bool value);

    float        activity_time_;
    inline float sinceActivity() { return System::GetNow() - activity_time_; }


    /** \return true if a button was just pressed. */
    bool RisingEdge();
    // inline bool RisingEdge() const { return updated_ ? state_ == 0x7f : false; }

    /** \return true if the button was just released */
    bool FallingEdge();
    // inline bool FallingEdge() const{ return updated_ ? state_ == 0x80 : false;}

    /** \return true if the button is held down (or if the toggle is on) */
    inline bool Pressed() const { return state_ == 0xff; }

    /** \return true if the button is held down, without debouncing */
    inline bool RawState()
    {
        return flip_ ? !dsy_gpio_read(&hw_gpio_) : dsy_gpio_read(&hw_gpio_);
    }

    /** \return the time in milliseconds that the button has been held (or toggle has been on) */
    inline float TimeHeldMs() const
    {
        return Pressed() ? System::GetNow() - rising_edge_time_ : 0;
    }

  public:
    bool holdWasTriggered = false;


    /** \return true if the button has been held for more than time */
    bool holdFor(int time);

    /** \return the time in milliseconds since rise */
    inline float sinceRiseMs() const
    {
        return System::GetNow() - rising_edge_time_;
    }


    /** Left for backwards compatability until next breaking change
     * \param update_rate Doesn't do anything
    */
    inline void SetUpdateRate(float update_rate) {}

  private:
    uint32_t last_update_;
    bool     updated_;
    Type     t_;
    dsy_gpio hw_gpio_;
    uint8_t  state_;
    bool     flip_;
    float    rising_edge_time_;
};

} // namespace daisy
#endif
