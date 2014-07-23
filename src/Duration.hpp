#ifndef DURATION_H
#define DURATION_H

class Duration
{
public:
    Duration();

    Duration(const Duration& duration);

    Duration& operator = (const Duration& duration);

    unsigned long elapsed() const;

private:
    unsigned long _start;
};

#endif // DURATION_H
