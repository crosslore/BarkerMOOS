/***********************************************************
    NAME: Jason Barker                                             
    ORGN: MIT                                             
    FILE: SegListEdit.h                                          
    DATE: November 12, 2019   
                                            
***********************************************************/
 #ifndef SEGLIST_EDIT_HEADER
 #define SEGLIST_EDIT_HEADER

#include <iterator>
#include <cmath>
#include <vector>
#include <string>
#include <cstdlib>
#include "XYObject.h"
#include "XYSegList.h"
#include "XYFormatUtilsSegl.h"
#include "SegIntercept.h"

using namespace std;

//This function works as long as there are working waypoints  

//------------------------------------------------------------
// Procedure: biteSegListRight
// This function takes in a seglist and creates a new seglist with all the points up to
// the point of intersection.  Returns the string value from .get_spec
// Example seglist pt=(10, -180: 110, -100: 40, 30)
// Example intersection point = 93.3 -113.33
// Output seglist pt=(10, -180: 93.3 -133.3)


string biteSegListRight(const XYSegList os_seglist, const double m_px, const double m_py) 
{
  XYSegList os_intercept;	
  int i;
 bool done = false;
	for(i=0; i<os_seglist.size(); i++){
		if(!done){
		double m_x1 = os_seglist.get_vx(i-1);
  	  	double m_y1 = os_seglist.get_vy(i-1);
      	double m_x2 = os_seglist.get_vx(i);
      	double m_y2 = os_seglist.get_vy(i);	

      	double slope = (m_y2 - m_y1)/(m_x2 - m_x1);
      	double intercept = m_y2 - (slope * m_x2);
      	double guess_y = (slope * m_px) + intercept;
      	if(guess_y!= m_py)
      		os_intercept.add_vertex(m_x2,m_y2);
      	if(guess_y == m_py){
      		os_intercept.add_vertex(m_px,m_py);
      		done = true;
      	}
      }
  }
      	string remaining = os_intercept.get_spec();
      	return(remaining); 
 }


//------------------------------------------------------------
// Procedure: biteSegListLeft
// Does not work 



string biteSegListLeft(XYSegList os_seglist, double m_x, double m_y) 
{
 int index = os_seglist.closest_segment(m_x, m_y);
 // os_seglist.insert_vertex(m_x, m_y);
 
 os_seglist.alter_vertex(m_x, m_y);
 // os_seglist.delete_vertex(index--);
 string remaining = os_seglist.get_spec();
 // double time = (os_remaining.length())/m_spd; 
 // string remaining = doubleToString(time) + "secs";
 return(remaining);
}






#endif 
