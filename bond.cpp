//
//  bond.cpp
//  overbond
//
//  
//

#include "bond.hpp"


//Bond::Bond(bool hasBonded) : hasBonded(hasBonded) {};

//void Bond::printBondMessage() {

//    if(this->hasBonded == true) {
//
//        std::cout << "bonded!" << std::endl;
//    }
//    else {
//        std::cout << "not bonded!" << std::endl;
//    }
//}
    
    
    Bond::Bond(std::string theBond, std::string theType, double theTerm, double theYield)
    : bondName(theBond), bondType(theType), bondTerm(theTerm), bondYield(theYield) {}
    
    Bond::Bond()
    : bondName(""), bondType(""), bondTerm(0), bondYield(0) {}
    
    bool Bond::operator < (const Bond& theBond) const
    {
        return (this->bondTerm < theBond.bondTerm);
    }
    
    std::string Bond::getBondName() const {
        return this->bondName;
    }
    
    std::string Bond::getBondType() const {
        return this->bondType;
    }
    
    double Bond::getBondTerm() const {
        return this->bondTerm;
    }
    
    double Bond::getBondYield() const {
        return this->bondYield;
    }
    
    void Bond::setBondName(std::string theBond) {
        this->bondName = theBond;
    }
    
    void Bond::setBondType(std::string theType) {
        this->bondType = theType;
    }
    
    void Bond::setBondTerm(double theTerm) {
        this->bondTerm = theTerm;
    }
    
    void Bond::setBondYield(double theYield) {
        this->bondYield = theYield;
    }
    
