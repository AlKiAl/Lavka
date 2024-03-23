#include <chrono>
#include <vector>
#include <mutex>

class RateController {
public:
    RateController(unsigned limit, unsigned long milliseconds);
    bool check();

private:
    using clk_t = std::chrono::system_clock;
    using time_point_t = std::chrono::time_point<clk_t>;

    unsigned n_;
    unsigned long milliseconds_;
    unsigned i_ = 0;
    std::vector<time_point_t> q;
    std::mutex mutex_;

    void push(time_point_t t);
};