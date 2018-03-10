#include <Arduino.h>
#include <L298MD.h>

class tracker {

public: 
    tracker(uint8_t tk1, uint8_t tk2, uint8_t tk3,uint8_t tk4);
    uint16_t tracking();
    void initSensorTracker();
    void initMotorTracker(L298MD& md);
    void readSesnor();
    bool _is_wbbw();
    bool _is_wwbw();
    bool _is_wwwb();
    bool _is_wbww();
    bool _is_bwww();
    bool _is_wwww();
    bool _is_bbbb();
    bool _is_wbbb();
    bool _is_wwbb();
    bool _is_bwwb();
    bool _is_bbbw();
    bool _is_bbww();
    uint8_t getpwm(uint16_t pwmpersen) {
        return (uint8_t)((pwmpersen * 255) / 100);
    }

private:
    uint8_t _pin_tk1;
    uint8_t _pin_tk2;
    uint8_t _pin_tk3;
    uint8_t _pin_tk4;

    uint8_t _val_tk1;
    uint8_t _val_tk2;
    uint8_t _val_tk3;
    uint8_t _val_tk4;
    L298MD _md;

};