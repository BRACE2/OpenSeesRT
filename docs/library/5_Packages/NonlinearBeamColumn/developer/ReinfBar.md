# ReinfBar

```cpp
// $Revision: 1.2 $
// $Date: 2003-02-14 23:01:37 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/repres/reinfBar/ReinfBar.h,v $
                                                                        
                                                                        
// File: ReinfBar.h
//
// Written by Remo M. de Souza
// November 1998

#include <OPS_Globals.h>

class Vector;

class ReinfBar
{
  public:

    ReinfBar();
    ReinfBar(double barArea, int materialID, const Vector &position);
        
    virtual ~ReinfBar();
    
    // edition functions

    void setDiameter (double barDiameter);
    void setArea     (double barArea);
    void setMaterial (int materialID);
    void setPosition (const Vector &position);

    // reinforcing bar inquiring functions
    
    double getDiameter (void) const;
    double getArea     (void) const;
    int    getMaterial (void) const; 

    const Vector & getPosition (void) const;

    virtual void Print(OPS_Stream &s, int flag =0) const;   
    friend OPS_Stream &operator<<(OPS_Stream &s, const ReinfBar &reinfBar);    
    
  protected:
    
  private:
    int    matID;
    double diameter;
    double area;
    Vector posit;
};


#endif

```
