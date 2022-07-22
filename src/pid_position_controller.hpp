#include <cstdint>

class PidPositionController {
   private:
    float kp;
    float ki;
    float kd;
    float max;
    float min;
    float freq;

    float e;
    float e_int;
    float e_diff;
    float u;

   public:
    void setParam(float kp, float ki, float kd, float max, float min,
                  float freq);
    void reset();

    void update(float target, float current);
    float getOutput();
};
