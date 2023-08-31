//
// Created by erik on 8/23/23.
//

#ifndef E21010_RUNTABULATIONS_H
#define E21010_RUNTABULATIONS_H

#include <map>
#include <string>
#define IS_NOT_POSSIBLE "C'EST PAS POSSIBLE!"

std::string get2PDaughter(unsigned int runNumber) {
  switch (runNumber) {
    // 22Al $( seq 30 35 ; seq 40 40 ; seq 45 52 ; seq 55 55 ; seq 57 59 ; seq 61 61 ; seq 63 74 )
    case 30 ... 35:   // < 30 mins low rate with different molecules, then 21Mg overnight
      return "20Ne";
    case 40:          // 7 min., then beam tuning
      return "20Ne";
    case 45 ... 52:   // 6-7 hours, then cavity trip
      return "20Ne";
    case 55:          // 1.25 hours
      return "20Ne";
    case 57 ... 59:   // 6.5 hours
      return "20Ne";
    case 60:          // attempt to measure daughters following beam loss
      return "20Ne";
    case 61:          // 45 min, then beam loss
      return "20Ne";
    case 63 ... 74:   // 15.8 hours
      return "20Ne";

    // 26P $( seq 79 82 ; seq 84 97 ; seq 103 118 )
    case 79 ... 82:   // ~30 mins with different molecules
      return "24Mg";
    case 84 ... 97:   // 15 hours, then 25Si contamination/background runs
      return "24Mg";
    case 103 ... 118: // 15.5 hours, then end of beam time
      return "24Mg";

    default:
      return IS_NOT_POSSIBLE;
  }
}


#endif //E21010_RUNTABULATIONS_H
