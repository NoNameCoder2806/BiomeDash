#pragma once

class Timer
{
    float length;
    float time;
    bool timeout;

public:
    Timer(float length) : length(length), time(0), timeout(false)
    {

    }

    bool step(float deltaTime)   // Step function
    {
        time += deltaTime;

        if (time >= length)
        {
            time -= length;      // Subtract the length so timer can loop
            timeout = true;      // Mark as timed out
            return true;         // Signal that it reached its duration
        }

        return false;
    }

    bool isTimeout() const       // Getter
    {
        return timeout;
    }

    float getTime() const        // Getter
    {
        return time;
    }

    float getLength() const      // Getter
    {
        return length;
    }

    void reset()                 // Reset function
    {
        time = 0;
        timeout = false;
    }

    void addTime(float extra)   // Extend timer duration by subtracting from elapsed time
    {
        time -= extra;
        if (time < 0)
            time = 0;
        timeout = false;
    }
};
