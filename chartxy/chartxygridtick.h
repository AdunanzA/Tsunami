/* © Pusterla Fabio 2011 */

#ifndef CHARTXYGRIDTICK_H
  #define CHARTXYGRIDTICK_H

  class ChartXYGridTick
    {
    public:
      ChartXYGridTick();
      ChartXYGridTick(unsigned int tick_major,unsigned int tick_minor);

      unsigned int tickMajor(void) const;
      void setTickMajor(unsigned int tick);

      unsigned int tickMinor(void) const;
      void setTickMinor(unsigned int tick);
    private:
      unsigned int m_major;
      unsigned int m_minor;
    };

#endif // CHARTXYGRIDTICK_H
