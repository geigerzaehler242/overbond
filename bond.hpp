//
//  bond.hpp
//  overbond
//
//  Created by fernando marto on 2021-06-07.
//

#ifndef bond_hpp
#define bond_hpp

#include <stdio.h>
#include <iostream>


class Bond {

public:

//    Bond(bool hasBonded);
//    void printBondMessage();
    
    
    Bond(std::string theBond, std::string theType, double theTerm, double theYield);
    
    Bond();
    
    bool operator < (const Bond& theBond) const;
    
    std::string getBondName() const;
    
    std::string getBondType() const;
    
    double getBondTerm() const;
    
    double getBondYield() const;
    
    void setBondName(std::string theBond);
    
    void setBondType(std::string theType);
    
    void setBondTerm(double theTerm);
    
    void setBondYield(double theYield);


private:

 //   bool hasBonded = false;

    std::string bondName;
    std::string bondType;
    double bondTerm;
    double bondYield;
    
    
}; //Bond


#endif /* bond_hpp */
