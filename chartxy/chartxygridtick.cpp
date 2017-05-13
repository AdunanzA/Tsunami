#include "chartxygridtick.h"

ChartXYGridTick::ChartXYGridTick()
  {
  m_major=4;
  m_minor=4;
  }

ChartXYGridTick::ChartXYGridTick(unsigned int tick_major,unsigned int tick_minor)
  {
  setTickMajor(tick_major);
  setTickMinor(tick_minor);
  }


unsigned int ChartXYGridTick::tickMajor(void) const
  {
  return(m_major);
  }

void ChartXYGridTick::setTickMajor(unsigned int tick)
  {
  if(tick<2) tick=2;
  m_major=tick;
  }

unsigned int ChartXYGridTick::tickMinor(void) const
  {
  return(m_minor);
  }

void ChartXYGridTick::setTickMinor(unsigned int tick)
  {
  if(tick<2) tick=2;
  m_minor=tick;
  }
