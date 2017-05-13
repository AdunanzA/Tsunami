/* © Pusterla Fabio 2011 */

#ifndef CHARTXYALGORITM_H
  #define CHARTXYALGORITM_H

  template<class T> class ChartXYAlgoritm
    {
    public:
      static T Interpulate(T x1,T y1,T x2,T y2,T x)
        {
        return(((((x-x2)*(y1))/(x1-x2))-(((x-x1)*(y2))/(x1-x2))));
        }
    };

#endif // CHARTXYALGORITM_H
