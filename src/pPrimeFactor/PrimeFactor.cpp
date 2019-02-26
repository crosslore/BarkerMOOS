/************************************************************/
/*    NAME: Jason Barker                                             
/*    ORGN: MIT                                             
/*    FILE: PrimeFactor.cpp                                        
/*    DATE: Feb 21, 2019                                                
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "PrimeFactor.h"
#include "PrimeEntry.h"
#include <sstream>
#include <math.h> 
#include <stdio.h>

using namespace std;

//---------------------------------------------------------
// Constructor

PrimeFactor::PrimeFactor()
{
}

PrimeEntry::PrimeEntry()
{
  m_start_index = 0;
  m_orig = 0;
  m_done == false;
  m_received_index = 0;
  m_calculated_index =0;
  m_current = 0;
  m_iterations = 0; // Used in prime factoring loop

}

//---------------------------------------------------------
// Destructor

PrimeFactor::~PrimeFactor()
{
}


//---------------------------------------------------------
// Procedure: OnNewMail

bool PrimeFactor::OnNewMail(MOOSMSG_LIST &NewMail)
{
  //AppCastingMOOSApp::OnNewMail(NewMail); 
  MOOSMSG_LIST::iterator p;
   
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    string key   = msg.GetKey();
    if (key == "NUM_VALUE"){//Looks through mail for variable NUM_VALUE
      string value = msg.GetString();// If NUM_VALUE is present it grabs the string 
      uint64_t m_orig =  strtoul(value.c_str(),NULL,0);//Turns the string into an integer
      int m_received_index;
      m_received_index = m_received_index + 1; // Updates received index
      PrimeEntry new_entry; //Enter PrimeEntry list
      new_entry.setReceivedIndex(m_received_index); //Retain the index recieved
      new_entry.setOriginalVal(m_orig); //Record the initial value
      new_entry.m_start_index = MOOSTime(); // Record the MOOS time 
      new_entry.m_current = m_orig; // Variable for the value to be factored
      m_list_primes.push_back(new_entry); // Push these elements into list
    }
  }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool PrimeFactor::OnConnectToServer()
{
   // register for variables here
   // possibly look at the mission file?
   // m_MissionReader.GetConfigurationParam("Name", <string>);
   // m_Comms.Register("VARNAME", 0);
	
   RegisterVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool PrimeFactor::Iterate()
{
  //AppCastingMOOSApp::Iterate(); 
  list<PrimeEntry>::iterator p;
   for(p=m_list_primes.begin(); p!=m_list_primes.end(); ) {//Works thru PrimeEntry List 
   PrimeEntry& current_calc = *p; //sets the working value to variable current_calc
   current_calc.setDone(current_calc.factor(1000)); 
   //This performs the function factor the max number of iterations
   //If successful, changes boolean to true 
   if(current_calc.m_done == true){//If current calc is done
    int m_calculated_index;
    m_calculated_index = m_calculated_index + 1; //update to next index
    current_calc.setCalculatedIndex(m_calculated_index);
    Notify("PRIME_RESULT", current_calc.getReport());//Publishes output
    p = m_list_primes.erase(p);//if done, removes from working list
   }
   else
    ++p; //Lines 110-113 from Lab 4 Sect 5.6 (pg 22)
  }
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool PrimeFactor::OnStartUp()
{
  //AppCastingMOOSApp::OnStartUp();
  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string line  = *p;
      string param = tolower(biteStringX(line, '='));
      string value = line;
      
      if(param == "foo") {
        //handled
      }
      else if(param == "bar") {
        //handled
      }
    }
  }
  
  RegisterVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void PrimeFactor::RegisterVariables()
{
  //AppCastingMOOSApp::RegisterVariables(); 
  Register("NUM_VALUE", 0);
  Register("PRIME_RESULT", 0);
}

//-------------------------------------------------------------
// Procedure:factor (from PrimeEntry.h)

bool PrimeEntry::factor(unsigned long int max_steps)
{
  //Factorization function/template found from online resource
  //https://www.geeksforgeeks.org/print-all-prime-factors-of-a-given-number/
  while(m_current % 2 == 0){
    m_factors.push_back(2); // Print the number of 2s that divide m_current
    m_current = m_current/2;
    m_iterations = m_iterations + 1; //Counts the number of iterations required
    if (m_iterations >= max_steps)
      return(false); //Exits this function if max steps reached
  }
  // m_current must be odd at this point
  for(int i = 3; i <= sqrt(m_current); i = i+2){
    // While i divides m_current, print i and divide m_current
    while (m_current%i == 0){
      m_factors.push_back(i);
      m_current = m_current/i;
      m_iterations = m_iterations +1;
      if (m_iterations >= max_steps)
      return(false); //Exits this function if max steps reached
    }
  } 
  // This condition is to handle the case when m_current  
  // is a prime number greater than 2 
    if (m_current > 2)
      m_factors.push_back(m_current);
  return(true);
}

//-------------------------------------------------------------
// Procedure:getReport (from PrimeEntry.h)
std::string PrimeEntry::getReport()
{
  stringstream ss_orig, ss_primes;
  ss_orig << m_orig;
  string orig = "orig" + ss_orig.str();//Lab 5 Section 2.3
  string received = " ,received="+intToString(m_received_index);
  string calculated = " ,calculated=" + intToString(m_calculated_index);
  string solve_time = " ,solve_time=" + doubleToString(MOOSTime() - m_start_index,3);
  copy(m_factors.begin(),m_factors.end(), ostream_iterator<uint64_t>(ss_primes, ":"));
  string primes = ss_primes.str();
  primes = " ,primes=" + primes.substr(0, primes.length()-1);
  string user = " ,username=jbbarker";
  string output = orig + received + calculated + solve_time+ primes + user;
  return(output);
}