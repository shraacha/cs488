#include "ProgressBar.hpp"

#include <cmath>

ProgressBar::ProgressBar() {}
// ~~~~~~ constructors ~~~~~~
ProgressBar::ProgressBar(unsigned int total, unsigned int width)
    : m_current{0},
      m_total{total},
      m_width{width},
      m_widthDouble{(double)width},
      m_updateThreshold{100},
      m_prevUpdateVal{0}
{}

// ~~~~~~ member functions ~~~~~~
ProgressBar& ProgressBar::operator++()
{
    ++m_current;

    return *this;
}

void ProgressBar::initOutput() {
    std::cout << '\r';
}

std::ostream& ProgressBar::conditionalOut(std::ostream& os)
{
    if (m_current - m_prevUpdateVal >= m_updateThreshold) {
        os << *this;
        m_prevUpdateVal = m_current;
    }

    return os;
}

// ~~~~~~ other functions ~~~~~~
std::ostream& operator<<(std::ostream& os, const ProgressBar & progressBar)
{
    double percentDone = (double)progressBar.m_current / (double)progressBar.m_total;

    os << '\r';
    os << std::string((unsigned int)((double)progressBar.m_width * percentDone), progressBar.m_fillChar)
       << std::string((unsigned int)((double)progressBar.m_width * (1.0 -percentDone)), progressBar.m_emptyChar)
       << progressBar.m_spacer
       << std::round(percentDone * 100) << "%";

    return os;
}
