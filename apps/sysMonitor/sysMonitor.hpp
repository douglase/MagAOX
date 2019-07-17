/** \file sysMonitor.hpp
  * \brief The MagAO-X sysMonitor app main program which provides functions to read and report system statistics
  * \author Chris Bohlman (cbohlman@pm.me)
  *
  * To view logdump files: logdump -f sysMonitor
  *
  * To view sysMonitor with cursesIndi: 
  * 1. /opt/MagAOX/bin/xindiserver -n xindiserverMaths
  * 2. /opt/MagAOX/bin/sysMonitor -n sysMonitor
  * 3. /opt/MagAOX/bin/cursesINDI 
  *
  * \ingroup sysMonitor_files
  *
  * History:
  * - 2018-08-10 created by CJB
  */
#ifndef sysMonitor_hpp
#define sysMonitor_hpp

#include "../../libMagAOX/libMagAOX.hpp" //Note this is included on command line to trigger pch
#include "../../magaox_git_version.h"

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <sys/wait.h>


namespace MagAOX
{
namespace app
{

/** MagAO-X application to read and report system statistics
  *
  */
class sysMonitor : public MagAOXApp<> 
{

protected:
   int m_warningCoreTemp = 0;   ///< User defined warning temperature for CPU cores
   int m_criticalCoreTemp = 0;   ///< User defined critical temperature for CPU cores
   int m_warningDiskTemp = 0;   ///< User defined warning temperature for drives
   int m_criticalDiskTemp = 0;   ///< User defined critical temperature for drives

   pcf::IndiProperty core_loads;   ///< Indi variable for reporting CPU core loads
   pcf::IndiProperty core_temps;   ///< Indi variable for reporting CPU core temperature(s)
   pcf::IndiProperty drive_temps;   ///< Indi variable for reporting drive temperature(s)
   pcf::IndiProperty root_usage;   ///< Indi variable for reporting drive usage of root path
   pcf::IndiProperty boot_usage;   ///< Indi variable for reporting drive usage of /boot path
   pcf::IndiProperty data_usage;   ///< Indi variable for reporting drive usage of /data path
   pcf::IndiProperty ram_usage_indi;   ///< Indi variable for reporting ram usage

   std::vector<float> coreTemps;   ///< List of current core temperature(s)
   std::vector<float> coreLoads;   ///< List of current core load(s)
   std::vector<float> diskTemp;   ///< List of current disk temperature(s)
   float rootUsage = 0;   ///< Disk usage in root path as a value out of 100
   float dataUsage = 0;   ///< Disk usage in /data path as a value out of 100
   float bootUsage = 0;   ///< Disk usage in /boot path as a value out of 100
   float ramUsage = 0;   ///< RAM usage as a decimal value between 0 and 1

   /// Updates Indi property values of all system statistics
   /** This includes updating values for core loads, core temps, drive temps, / usage, /boot usage, /data usage, and RAM usage
     * Unsure if this method can fail in any way, as of now always returns 0
     *
     * \TODO: Check to see if any method called in here can fail
     * \returns 0 on completion
     */
   int updateVals();

public:

   /// Default c'tor.
   sysMonitor();

   /// D'tor, declared and defined for noexcept.
   ~sysMonitor() noexcept
   {
   }

   /// Setup the user-defined warning and critical values for core and drive temperatures
   virtual void setupConfig();

   /// Load the warning and critical temperature values for core and drive temperatures
   virtual void loadConfig();

   /// Registers all new Indi properties for each of the reported values to publish
   virtual int appStartup();

   /// Implementation of reading and logging each of the measured statistics
   virtual int appLogic();

   /// Do any needed shutdown tasks; currently nothing in this app
   virtual int appShutdown();


   /// Finds all CPU core temperatures
   /** Makes system call and then parses result to add temperatures to vector of values
     *
     * \returns -1 on error with system command or output reading
     * \returns 0 on successful completion otherwise
     */
   int findCPUTemperatures(
      std::vector<float>&  /**< [out] the vector of measured CPU core temperatures*/
   );


   /// Parses string from system call to find CPU temperatures
   /** When a valid string is read in, the value from that string is stored
     * 
     * \returns -1 on invalid string being read in
     * \returns 0 on completion and storing of value
     */
   int parseCPUTemperatures(
      std::string,  /**< [in] the string to be parsed*/
      float&   /**< [out] the return value from the string*/
   );


   /// Checks if any core temperatures are warning or critical levels
   /** Warning and critical temperatures are either user-defined or generated based on initial core temperature values
     *
     * \returns 1 if a temperature value is at the warning level
     * \returns 2 if a temperature value is at critical level
     * \returns 0 otherwise (all temperatures are considered normal)
     */
   int criticalCoreTemperature(
      std::vector<float>&   /**< [in] the vector of temperature values to be checked*/
   );


   /// Finds all CPU core usage loads
   /** Makes system call and then parses result to add usage loads to vector of values
     *
     * \returns -1 on error with system command or output file reading
     * \returns 0 on completion
     */
   int findCPULoads(
      std::vector<float>&   /**< [out] the vector of measured CPU usages*/
   );


   /// Parses string from system call to find CPU usage loads
   /** When a valid string is read in, the value from that string is stored
     * 
     * \returns -1 on invalid string being read in
     * \returns 0 on completion and storing of value
     */
   int parseCPULoads(
      std::string,   /**< [in] the string to be parsed*/
      float&   /**< [out] the return value from the string*/
   );


   /// Finds all drive temperatures
   /** Makes system call and then parses result to add temperatures to vector of values
     * For hard drive temp utility:
     * `wget http://dl.fedoraproject.org/pub/epel/7/x86_64/Packages/h/hddtemp-0.3-0.31.beta15.el7.x86_64.rpm`
     * `su`
     * `rpm -Uvh hddtemp-0.3-0.31.beta15.el7.x86_64.rpm`
     * Check install with rpm -q -a | grep -i hddtemp
     *
     * \returns -1 on error with system command or output reading
     * \returns 0 on successful completion otherwise
     */
   int findDiskTemperature(
      std::vector<float>&   /**< [out] the vector of measured drive temperatures*/
   );


   /// Parses string from system call to find drive temperatures
   /** When a valid string is read in, the value from that string is stored
     * 
     * \returns -1 on invalid string being read in
     * \returns 0 on completion and storing of value
     */
   int parseDiskTemperature(
      std::string,  /**< [in] the string to be parsed*/
      float&   /**< [out] the return value from the string*/
   );


   /// Checks if any drive temperatures are warning or critical levels
   /** Warning and critical temperatures are either user-defined or generated based on initial drive temperature values
     *
     * \returns 1 if a temperature value is at the warning level
     * \returns 2 if a temperature value is at critical level
     * \returns 0 otherwise (all temperatures are considered normal)
     */
   int criticalDiskTemperature(
      std::vector<float>&   /**< [in] the vector of temperature values to be checked*/
   );


   /// Finds usages of space for following directory paths: /; /data; /boot
   /** These usage values are stored as integer values between 0 and 100 (e.g. value of 39 means directory is 39% full)
     * If directory is not found, space usage value will remain 0
     * TODO: What about multiple drives? What does this do?
     *
     * \returns -1 on error with system command or output reading
     * \returns 0 if at least one of the return values is found
     */
   int findDiskUsage(
      float&,   /**< [out] the return value for usage in root path*/
      float&,   /**< [out] the return value for usage in /data path*/
      float&    /**< [out] the return value for usage in /boot path*/
   );


   /// Parses string from system call to find drive usage space
   /** When a valid string is read in, the value from that string is stored
     * 
     * \returns -1 on invalid string being read in
     * \returns 0 on completion and storing of value
     */
   int parseDiskUsage(
      std::string,   /**< [in] the string to be parsed*/
      float&,   /**< [out] the return value for usage in root path*/
      float&,   /**< [out] the return value for usage in /data path*/
      float&    /**< [out] the return value for usage in /boot path*/
   );


   /// Finds current RAM usage
   /** This usage value is stored as a decimal value between 0 and 1 (e.g. value of 0.39 means RAM usage is 39%)
     *
     * \returns -1 on error with system command or output reading
     * \returns 0 on completion
     */
   int findRamUsage(
      float&    /**< [out] the return value for current RAM usage*/
   );


   /// Parses string from system call to find RAM usage
   /** When a valid string is read in, the value from that string is stored
    * 
    * \returns -1 on invalid string being read in
    * \returns 0 on completion and storing of value
    */
   int parseRamUsage(
      std::string,   /**< [in] the string to be parsed*/
      float&    /**< [out] the return value for current RAM usage*/
   );

   /// Runs a command (with parameters) passed in using fork/exec
   /** New process is made with fork(), and child runs execvp with command provided
    * 
    * \returns output of command, contained in a vector of strings
    * If an error occurs during the process, an empty vector of strings is returned.
    */
   std::vector<std::string> runCommand(
    std::vector<std::string>    /**< [in] command to be run, with any subsequent parameters stored after*/
   );

};

inline sysMonitor::sysMonitor() : MagAOXApp(MAGAOX_CURRENT_SHA1, MAGAOX_REPO_MODIFIED)
{
   return;
}

void sysMonitor::setupConfig()
{
   config.add("warningCoreTemp", "", "warningCoreTemp", argType::Required, "", "warningCoreTemp", false, "int", "The warning temperature for CPU cores.");
   config.add("criticalCoreTemp", "", "criticalCoreTemp", argType::Required, "", "criticalCoreTemp", false, "int", "The critical temperature for CPU cores.");
   config.add("warningDiskTemp", "", "warningDiskTemp", argType::Required, "", "warningDiskTemp", false, "int", "The warning temperature for the disk.");
   config.add("criticalDiskTemp", "", "criticalDiskTemp", argType::Required, "", "criticalDiskTemp", false, "int", "The critical temperature for disk.");
}

void sysMonitor::loadConfig()
{
   config(m_warningCoreTemp, "warningCoreTemp");
   config(m_criticalCoreTemp, "criticalCoreTemp");
   config(m_warningDiskTemp, "warningDiskTemp");
   config(m_criticalDiskTemp, "criticalDiskTemp");
}

int sysMonitor::appStartup()
{
   REG_INDI_NEWPROP_NOCB(core_loads, "core_loads", pcf::IndiProperty::Number);
   REG_INDI_NEWPROP_NOCB(core_temps, "core_temps", pcf::IndiProperty::Number);
   REG_INDI_NEWPROP_NOCB(drive_temps, "drive_temps", pcf::IndiProperty::Number);
   REG_INDI_NEWPROP_NOCB(root_usage, "root_usage", pcf::IndiProperty::Number);
   REG_INDI_NEWPROP_NOCB(boot_usage, "boot_usage", pcf::IndiProperty::Number);
   REG_INDI_NEWPROP_NOCB(data_usage, "data_usage", pcf::IndiProperty::Number);
   REG_INDI_NEWPROP_NOCB(ram_usage_indi, "ram_usage_indi", pcf::IndiProperty::Number);

   unsigned int i;
   std::string coreStr = {"core"};

   findCPULoads(coreLoads);
   for (i = 0; i < coreLoads.size(); i++) 
   {
      coreStr.append(std::to_string(i));
      core_loads.add (pcf::IndiElement(coreStr));
      core_loads[coreStr].set<double>(0.0);
      coreStr.pop_back();
   }

   findCPUTemperatures(coreTemps);
   for (i = 0; i < coreTemps.size(); i++) 
   {
      coreStr.append(std::to_string(i));
      core_temps.add (pcf::IndiElement(coreStr));
      core_temps[coreStr].set<double>(0.0);
      coreStr.pop_back();
   }

   std::string driveStr = {"drive"};
   findDiskTemperature(diskTemp);
   for (i = 0; i < diskTemp.size(); i++) 
   {
      driveStr.append(std::to_string(i));
      drive_temps.add (pcf::IndiElement(driveStr));
      drive_temps[driveStr].set<double>(0.0);
      driveStr.pop_back();
   }

   root_usage.add(pcf::IndiElement("root_usage"));
   root_usage["root_usage"].set<double>(0.0);

   boot_usage.add(pcf::IndiElement("boot_usage"));
   boot_usage["boot_usage"].set<double>(0.0);

   data_usage.add(pcf::IndiElement("data_usage"));
   data_usage["data_usage"].set<double>(0.0);

   ram_usage_indi.add(pcf::IndiElement("ram_usage"));
   ram_usage_indi["ram_usage"].set<double>(0.0);

   return 0;
}

int sysMonitor::appLogic()
{
   coreTemps.clear();
   int rvCPUTemp = findCPUTemperatures(coreTemps);
   if (rvCPUTemp >= 0) 
   {
      for (auto i: coreTemps)
      {
         std::cout << "Core temp: " << i << ' ';
      }   
      std::cout << std::endl;
      rvCPUTemp = criticalCoreTemperature(coreTemps);
   }
   
   coreLoads.clear();
   int rvCPULoad = findCPULoads(coreLoads);
   if (rvCPULoad >= 0) 
   {
      for (auto i: coreLoads)
      {
         std::cout << "CPU load: " << i << ' ';
      }
      std::cout << std::endl;
   }
   
   if (rvCPUTemp >= 0 && rvCPULoad >= 0)
   {
      if (rvCPUTemp == 1)
      {
         log<core_mon>({coreTemps, coreLoads}, logPrio::LOG_WARNING);
      } 
      else if (rvCPUTemp == 2) 
      {
         log<core_mon>({coreTemps, coreLoads}, logPrio::LOG_ALERT);
      } 
      else 
      {
         log<core_mon>({coreTemps, coreLoads}, logPrio::LOG_INFO);
      }
   }
   else 
   {
      log<software_error>({__FILE__, __LINE__,"Could not log values for CPU core temperatures and usages."});
   }

   diskTemp.clear();
   int rvDiskTemp = findDiskTemperature(diskTemp);
   if (rvDiskTemp >= 0)
   {
      for (auto i: diskTemp)
      {
         std::cout << "Disk temp: " << i << ' ';
      }
      std::cout << std::endl;
      rvDiskTemp = criticalDiskTemperature(diskTemp);
   }  


   int rvDiskUsage = findDiskUsage(rootUsage, dataUsage, bootUsage);

   if (rvDiskUsage >= 0)
   {
      std::cout << "/ usage: " << rootUsage << std::endl;
      std::cout << "/data usage: " << dataUsage << std::endl; 
      std::cout << "/boot usage: " << bootUsage << std::endl;
   }
   
   if (rvDiskTemp >= 0 && rvDiskUsage >= 0)
   {
      if (rvDiskTemp == 1) 
      {
         log<drive_mon>({diskTemp, rootUsage, dataUsage, bootUsage}, logPrio::LOG_WARNING);
      } 
      else if (rvDiskTemp == 2) 
      {
         log<drive_mon>({diskTemp, rootUsage, dataUsage, bootUsage}, logPrio::LOG_ALERT);
      } 
      else 
      {
         log<drive_mon>({diskTemp, rootUsage, dataUsage, bootUsage}, logPrio::LOG_INFO);
      }
   }
   else 
   {
      log<software_error>({__FILE__, __LINE__,"Could not log values for drive temperatures and usages."});
   }
   

   int rvRamUsage = findRamUsage(ramUsage);
   if (rvRamUsage >= 0)
   {
      std::cout << "Ram usage: " << ramUsage << std::endl;
      log<ram_usage>({ramUsage}, logPrio::LOG_INFO);
   }
   else 
   {
      log<software_error>({__FILE__, __LINE__,"Could not log values for RAM usage."});
   }

   updateVals();

   return 0;
}

int sysMonitor::appShutdown()
{
    return 0;
}

int sysMonitor::findCPUTemperatures(std::vector<float>& temps) 
{
   std::vector<std::string> commandList{"sensors"};
   std::vector<std::string> commandOutput = runCommand(commandList);
   int rv = -1;
   for (auto line: commandOutput) 
   {  
      float tempVal;
      if (parseCPUTemperatures(line, tempVal) == 0)
      {
         temps.push_back(tempVal);
         rv = 0;
      }
   }
   return rv;
}

int sysMonitor::parseCPUTemperatures(std::string line, float& temps) 
{
   if (line.length() <= 1)
   {
      return -1;
   }

   std::string str = line.substr(0, 5);
   if (str.compare("Core ") == 0) 
   {
      std::string temp_str = line.substr(17, 4);
      float temp;
      try
      {
         temp = std::stof (temp_str);
      }
      catch (const std::invalid_argument& e) 
      {
         log<software_error>({__FILE__, __LINE__,"Invalid read occured when parsing CPU temperatures."});
         return -1;
      }

      temps = temp;

      if (m_warningCoreTemp == 0)
      {
         std::istringstream iss(line);
         std::vector<std::string> tokens{std::istream_iterator<std::string>{iss},std::istream_iterator<std::string>{}};
         try
         {
            tokens.at(5).pop_back();
            tokens.at(5).pop_back();
            tokens.at(5).pop_back();
            tokens.at(5).pop_back();
            tokens.at(5).erase(0,1);
            m_warningCoreTemp = std::stof(tokens.at(5));
         }
         catch (const std::invalid_argument& e) 
         {
            log<software_error>({__FILE__, __LINE__,"Invalid read occured when parsing warning CPU temperatures."});
            return -1;
         }
      }
      if (m_criticalCoreTemp == 0) 
      {
         std::istringstream iss(line);
         std::vector<std::string> tokens{std::istream_iterator<std::string>{iss},std::istream_iterator<std::string>{}};
         try
         {
            tokens.at(8).pop_back();
            tokens.at(8).pop_back();
            tokens.at(8).pop_back();
            tokens.at(8).pop_back();
            tokens.at(8).erase(0,1);
            m_criticalCoreTemp = std::stof(tokens.at(8));
         }
         catch (const std::invalid_argument& e) 
         {
            log<software_error>({__FILE__, __LINE__,"Invalid read occured when parsing critical CPU temperatures."});
            return -1;
         }
      }
      return 0;
   }
   else 
   {
      return -1;
   }

}

int sysMonitor::criticalCoreTemperature(std::vector<float>& v)
{
   int coreNum = 0, rv = 0;
   for(auto it: v)
   {
      float temp = it;
      if (temp >= m_warningCoreTemp && temp < m_criticalCoreTemp ) 
      {
         std::cout << "Warning temperature for Core " << coreNum << std::endl;
         if (rv < 2) 
         {
            rv = 1;
         }
      }
      else if (temp >= m_criticalCoreTemp) 
      {   
         std::cout << "Critical temperature for Core " << coreNum << std::endl;
         rv = 2;
      }
      ++coreNum;
   }
   return rv;
}

int sysMonitor::findCPULoads(std::vector<float>& loads) 
{
   std::vector<std::string> commandList{"mpstat", "-P", "ALL"};
   std::vector<std::string> commandOutput = runCommand(commandList);
   int rv = -1;
   // If output lines are less than 5 (with one CPU, guarenteed output is 5)
   if (commandOutput.size() < 5) 
   {
      return rv;
   }
   //start iterating at fourth line
   for (auto line = commandOutput.begin()+4; line != commandOutput.end(); line++) 
   {
      float loadVal;
      if (parseCPULoads(*line, loadVal) == 0) 
      {
         loads.push_back(loadVal);
         rv = 0;
      }
   }
   return rv;
}

int sysMonitor::parseCPULoads(std::string line, float& loadVal)
{
   if (line.length() <= 1)
   {
      return -1;
   }
   std::istringstream iss(line);
   std::vector<std::string> tokens{std::istream_iterator<std::string>{iss},std::istream_iterator<std::string>{}};
   float cpu_load;
   try
   {
      cpu_load = 100.0 - std::stof(tokens.at(12));
   }
   catch (const std::invalid_argument& e)
   {
      log<software_error>({__FILE__, __LINE__,"Invalid read occured when parsing CPU core usage."});
      return -1;
   }
   catch (const std::out_of_range& e) {
      return -1;
   }
   cpu_load /= 100;
   loadVal = cpu_load;
   return 0;
}

int sysMonitor::findDiskTemperature(std::vector<float>& hdd_temp) 
{
   std::vector<std::string> commandList{"hddtemp"};
   std::vector<std::string> commandOutput = runCommand(commandList);
   int rv = -1;
   for (auto line: commandOutput) 
   {  
      float tempVal;
      if (parseDiskTemperature(line, tempVal) == 0)
      {
         hdd_temp.push_back(tempVal);
         rv = 0;
      }
   }
   return rv;
}

int sysMonitor::parseDiskTemperature(std::string line, float& hdd_temp) 
{
   float tempValue;
   if (line.length() <= 1) 
   {
      return -1;
   }
   std::istringstream iss(line);
   std::vector<std::string> tokens{std::istream_iterator<std::string>{iss},std::istream_iterator<std::string>{}};

   for(auto temp_s: tokens) 
   {
      try {
         if (isdigit(temp_s.at(0)) && temp_s.substr(temp_s.length() - 1, 1) == "C") 
         {
            temp_s.pop_back();
            temp_s.pop_back();
            try
            {
               tempValue = std::stof (temp_s);
            }
            catch (const std::invalid_argument& e) 
            {
               log<software_error>({__FILE__, __LINE__,"Invalid read occured when parsing drive temperatures."});
               return -1;
            }
            hdd_temp = tempValue;
            if (m_warningDiskTemp == 0)
            {
               m_warningDiskTemp = tempValue + (.1*tempValue);
            }
            if (m_criticalDiskTemp == 0) 
            {
               m_criticalDiskTemp = tempValue + (.2*tempValue);
            }
            return 0;
         }
      }
      catch (const std::out_of_range& e) {
         return -1;
      }
   }
   return -1;
}

int sysMonitor::criticalDiskTemperature(std::vector<float>& v)
{
   int rv = 0;
   for(auto it: v)
   {
      float temp = it;
      if (temp >= m_warningDiskTemp && temp < m_criticalDiskTemp )
      {
         std::cout << "Warning temperature for Disk" << std::endl;
         if (rv < 2)
         {
            rv = 1;
         }
      }  
      else if (temp >= m_criticalDiskTemp) 
      {   
         std::cout << "Critical temperature for Disk " << std::endl;
         rv = 2;
      }
   }
   return rv;
}

int sysMonitor::findDiskUsage(float &rootUsage, float &dataUsage, float &bootUsage) 
{
   std::vector<std::string> commandList{"df"};
   std::vector<std::string> commandOutput = runCommand(commandList);
   int rv = -1;
   for (auto line: commandOutput) 
   {  
      int rvDiskUsage = parseDiskUsage(line, rootUsage, dataUsage, bootUsage);
      if (rvDiskUsage == 0) 
      {
         rv = 0;
      }
   }
   return rv;
}

int sysMonitor::parseDiskUsage(std::string line, float& rootUsage, float& dataUsage, float& bootUsage) 
{
   if (line.length() <= 1)
   {
      return -1;
   }

   std::istringstream iss(line);
   std::vector<std::string> tokens{std::istream_iterator<std::string>{iss},std::istream_iterator<std::string>{}};

   try {
      if (tokens.at(5).compare("/") == 0)
      {
         tokens.at(4).pop_back();
         try
         {
            rootUsage = std::stof (tokens.at(4))/100;
            return 0;
         }
         catch (const std::invalid_argument& e) 
         {
            log<software_error>({__FILE__, __LINE__,"Invalid read occured when parsing drive usage."});
            return -1;
         }
      } 
      else if (tokens.at(5).compare("/data") == 0)
      {
         tokens.at(4).pop_back();
         try
         {
            dataUsage = std::stof (tokens.at(4))/100;
            return 0;
         }
         catch (const std::invalid_argument& e) 
         {
            log<software_error>({__FILE__, __LINE__,"Invalid read occured when parsing drive usage."});
            return -1;
         }
      } 
      else if (tokens.at(5).compare("/boot") == 0)
      {
         tokens.at(4).pop_back();
         try
         {
            bootUsage = std::stof (tokens.at(4))/100;
            return 0;
         }
         catch (const std::invalid_argument& e) 
         {
            log<software_error>({__FILE__, __LINE__,"Invalid read occured when parsing drive usage."});
            return -1;
         }
      }
   }
   catch (const std::out_of_range& e) {
      return -1;
   }
   return -1;
}

int sysMonitor::findRamUsage(float& ramUsage) 
{
   std::vector<std::string> commandList{"free", "-m"};
   std::vector<std::string> commandOutput = runCommand(commandList);
   for (auto line: commandOutput) 
   {  
      if (parseRamUsage(line, ramUsage) == 0)
      {
        return 0;
      }
   }
   return -1;
}

int sysMonitor::parseRamUsage(std::string line, float& ramUsage) 
{
   if (line.length() <= 1)
   {
      return -1;
   }
   std::istringstream iss(line);
   std::vector<std::string> tokens{std::istream_iterator<std::string>{iss},std::istream_iterator<std::string>{}};
   try
   {
      if (tokens.at(0).compare("Mem:") != 0)
      {
        return -1;
      }
      ramUsage = std::stof(tokens.at(2))/std::stof(tokens.at(1));
      if (ramUsage > 1 || ramUsage == 0)
      {
         ramUsage = -1;  
         return -1;
      }
      return 0;
   }
   catch (const std::invalid_argument& e) 
   {
      log<software_error>({__FILE__, __LINE__,"Invalid read occured when parsing RAM usage."});
      return -1;
   }
   catch (const std::out_of_range& e) {
      return -1;
   }
}

int sysMonitor::updateVals()
{
   MagAOXApp::updateIfChanged(core_loads, "core", coreLoads);

   MagAOXApp::updateIfChanged(core_temps, "core", coreTemps);

   MagAOXApp::updateIfChanged(drive_temps, "drive", diskTemp);

   MagAOXApp::updateIfChanged(
      root_usage,
      "root_usage",
      rootUsage
   );

   MagAOXApp::updateIfChanged(
      boot_usage,
      "boot_usage",
      bootUsage
   );

   MagAOXApp::updateIfChanged(
      data_usage,
      "data_usage",
      dataUsage
   );

   MagAOXApp::updateIfChanged(
      ram_usage_indi,
      "ram_usage",
      ramUsage
   );

   return 0;
}

std::vector<std::string> sysMonitor::runCommand( std::vector<std::string> commandList) 
{
   int link[2];
   pid_t pid;
   char commandOutput_c[4096];
   std::vector<std::string> commandOutput;

   if (pipe(link)==-1) 
   {
      perror("Pipe error");
      return commandOutput;
   }

   if ((pid = fork()) == -1) 
   {
      perror("Fork error");
      return commandOutput;
   }

   if(pid == 0) 
   {
      dup2 (link[1], STDOUT_FILENO);
      close(link[0]);
      close(link[1]);
      std::vector<const char *>charCommandList( commandList.size()+1, NULL);
      for(int index = 0; index < (int) commandList.size(); ++index)
      {
         charCommandList[index]=commandList[index].c_str();
      }
      execvp( charCommandList[0], const_cast<char**>(charCommandList.data()));
      perror("exec");
      return commandOutput;
   }
   else 
   {
      wait(NULL);
      close(link[1]);
      if (read(link[0], commandOutput_c, sizeof(commandOutput_c)) < 0) 
      {
         perror("Read error");
         return commandOutput;
      }
      std::string line{};
      std::string commandOutputString(commandOutput_c);
      std::istringstream iss(commandOutputString);
      while (getline(iss, line)) 
      {
         commandOutput.push_back(line);
      }
      wait(NULL);
      return commandOutput;
   }
}


} //namespace app
} //namespace MagAOX

#endif //sysMonitor_hpp
