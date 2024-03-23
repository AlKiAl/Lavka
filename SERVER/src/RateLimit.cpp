#include "RateLimit.h"


RateController::RateController(unsigned limit, unsigned long milliseconds)
    : n_(limit), milliseconds_(milliseconds), q(n_, time_point_t()) {}

bool RateController::check() {
    std::lock_guard<std::mutex> lock(mutex_);
    time_point_t t = clk_t::now();
    bool allowed = t - q[i_] >= std::chrono::milliseconds(milliseconds_);
    if (allowed)
        push(t);
    return allowed;
}

void RateController::push(time_point_t t) {
    q[i_] = t;
    i_ = (i_ + 1) % n_;
}
