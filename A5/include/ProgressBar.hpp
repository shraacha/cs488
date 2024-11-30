#pragma once

#include <iostream>
#include <atomic>
#include <string>

class ProgressBar {
    std::atomic<unsigned int> m_current;
    unsigned int m_total;
    unsigned int m_width;
    unsigned int m_updateThreshold;
    unsigned int m_prevUpdateVal;
    double m_widthDouble;

    const char m_fillChar = '+';
    const char m_emptyChar = '-';
    const std::string m_spacer = "  ";

  public:
    ProgressBar();
    ProgressBar(unsigned int total, unsigned int width = 50);

    std::ostream& conditionalOut(std::ostream& os);

    ProgressBar& operator++();
    void initOutput();

    friend std::ostream& operator<<(std::ostream& os, const ProgressBar & progressBar);
};

