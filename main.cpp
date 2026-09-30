//
//  main.cpp
//  overbond
//
// 
//

//#include <iostream>
#include <thread>

#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include <cmath>

#include "bond.hpp" //#include <iostream>


enum BondType {
    corporate,
    government
};

struct BondBenchmarkSpread {
    std::string corporateBond;
    std::string governmentBond;
    std::string spread;
    
    BondBenchmarkSpread(std::string corporateBond, std::string governmentBond, std::string spread) : corporateBond(corporateBond), governmentBond(governmentBond), spread(spread) {}
};

struct BondCurveSpread {
    std::string corporateBond;
    std::string spread;
    
    BondCurveSpread(std::string corporateBond, std::string spread) : corporateBond(corporateBond), spread(spread) {}
};

void getBondData(std::string fileName, std::vector<Bond> &vCorp, std::vector<Bond> &vGov) {
    
    std::ifstream data(fileName.c_str());
    if( !data ) {
        std::cerr << "Error opening file inputbonds.csv \n";
    }
    
    std::string line;
    
    while(std::getline(data, line, '\r')) { //parse line, might need '\n' instead of '\r' on different operating system
        
        std::regex pattern("\""); //remove extraneous "\"" string from both ends of parsed line
        std::string lineFiltered = std::regex_replace(line, pattern, "");
        
        std::stringstream lineStream(lineFiltered);
        std::string cell;
        
        std::vector<std::string> cellVector;
        
        while(std::getline(lineStream, cell, ',')) { //parse cells within a line
            std::cout << cell << " ";
            
            cellVector.push_back(cell);
        }
        std::cout << std::endl;
        
        std::regex patternPercent("%"); //remove "%" string from yield
        
        if(cellVector.at(1) == "corporate") {
            std::string yieldFiltered = std::regex_replace(cellVector.at(3), patternPercent, "");
            Bond bc(cellVector.at(0), cellVector.at(1), atof(cellVector.at(2).c_str()), atof(yieldFiltered.c_str()));
            vCorp.push_back(bc);
        }
        else if(cellVector.at(1) == "government") {
            std::string yieldFiltered = std::regex_replace(cellVector.at(3), patternPercent, "");
            Bond bg(cellVector.at(0), cellVector.at(1), atof(cellVector.at(2).c_str()), atof(yieldFiltered.c_str()));
            vGov.push_back(bg);
        }
        else {
            //title row!
        }
    }
    std::cout << std::endl;
}





void findBenchmarkSpread(const std::vector<Bond> &vCorp, const std::vector<Bond> &vGov, std::vector<BondBenchmarkSpread> &benchmarkVector) {
    
    for (auto currentCorpBond : vCorp) {
        
        double corpYield = currentCorpBond.getBondYield();
        double corpTerm = currentCorpBond.getBondTerm();
        
        double minYieldSpread = std::numeric_limits<double>::max();
        double minTermSpread = std::numeric_limits<double>::max();
        Bond benchmarkBond = Bond();
        
        for (auto currentGovBond : vGov) {
            
            double govYield = currentGovBond.getBondYield();
            double govTerm = currentGovBond.getBondTerm();
            
            double termDiff = std::abs(govTerm - corpTerm);
            
            if( termDiff <=  minTermSpread) {
                
                minTermSpread = termDiff;
                minYieldSpread = corpYield - govYield;
                
                benchmarkBond.setBondName(currentGovBond.getBondName());
                benchmarkBond.setBondType(currentGovBond.getBondType());
                benchmarkBond.setBondTerm(currentGovBond.getBondTerm());
                benchmarkBond.setBondYield(currentGovBond.getBondYield());
            }
        }
        
        BondBenchmarkSpread benchmarkSpread = BondBenchmarkSpread(      currentCorpBond.getBondName(),
            benchmarkBond.getBondName(),
            std::to_string(minYieldSpread)
        );
        
        benchmarkVector.push_back(benchmarkSpread);
    }
    std::cout << std::endl;
}

double interpolateCurvePointYield(const Bond &leftGovBond, const Bond &rightGovBond, const Bond &currentCorpBond) {
    
    double result = 0.0;
    
    double m, y, x, b, curveYieldPointY;
    
    y = rightGovBond.getBondYield() - leftGovBond.getBondYield();
    b = 0;
    x = rightGovBond.getBondTerm() - leftGovBond.getBondTerm();
    
    m = (y - b) / x;
    
    b = leftGovBond.getBondYield() - m * leftGovBond.getBondTerm();
    
    curveYieldPointY = m * currentCorpBond.getBondTerm() + b;
    
    result = floor( (currentCorpBond.getBondYield() - curveYieldPointY ) * 100 + 0.5) /100;
    
    return result;
}

void findCurveSpread(const std::vector<Bond> &vCorp, const std::vector<Bond> &vGov, std::vector<BondCurveSpread> &benchmarkCurveVector) {
    
    for (auto currentCorpBond : vCorp) {
        
        double corpYield = currentCorpBond.getBondYield();
        double corpTerm = currentCorpBond.getBondTerm();
        
        Bond leftGovBond = Bond();
        Bond rightGovBond = Bond();
        
        double minYieldSpread = std::numeric_limits<double>::max();
        
        for (auto currentGovBond : vGov) {
            
            double govYield = currentGovBond.getBondYield();
            double govTerm = currentGovBond.getBondTerm();
            
            if( govTerm < corpTerm) { //gov bonds are sorted by term so the last set leftmost bond is used for interpolation
                
                leftGovBond = currentGovBond;
            }
            else if( govTerm == corpTerm) { //gov bond term matches corporate, no need to interpolate curve
                
                minYieldSpread = corpYield - govYield;
                
                break;
            }
            else { //gov bonds are sorted by term so the first set rightmost bond is used for interpolation
             
                rightGovBond = currentGovBond;
                
                minYieldSpread = interpolateCurvePointYield(leftGovBond, rightGovBond, currentCorpBond);
                break;
            }
        }
                
        BondCurveSpread bondCurveSpread = BondCurveSpread(currentCorpBond.getBondName(), std::to_string(minYieldSpread) );
        
        benchmarkCurveVector.push_back(bondCurveSpread);
        
    }
    std::cout << std::endl;
}




void testChalenge1(const std::vector<Bond> &vCorp, const std::vector<Bond> &vGov, std::vector<BondBenchmarkSpread> &benchmarkVector) {
    
    double corpYield = vCorp[0].getBondYield();
    double corpTerm = vCorp[0].getBondTerm();
    
    double minYieldSpread = std::numeric_limits<double>::max();
    double minTermSpread = std::numeric_limits<double>::max();
    Bond benchmarkBond = Bond();
    
    for (auto currentGovBond : vGov) {
        
        double govYield = currentGovBond.getBondYield();
        double govTerm = currentGovBond.getBondTerm();
        
        double termDiff = std::abs(govTerm - corpTerm);
        
        if( termDiff <=  minTermSpread) {
            
            minTermSpread = termDiff;
            minYieldSpread = corpYield - govYield;
            
            benchmarkBond.setBondName(currentGovBond.getBondName());
            benchmarkBond.setBondType(currentGovBond.getBondType());
            benchmarkBond.setBondTerm(currentGovBond.getBondTerm());
            benchmarkBond.setBondYield(currentGovBond.getBondYield());
        }
    }
    
    if(std::to_string(minYieldSpread) == "1.600000") {
        std::cout << "findBenchmarkSpread PASSED" << std::endl;
    }
    else {
        std::cout << "findBenchmarkSpread FAILED" << std::endl;
    }
    
    
}

void testChalenge2(const std::vector<Bond> &vCorp, const std::vector<Bond> &vGov, std::vector<BondCurveSpread> &benchmarkCurveVector) {
    
        double corpYield = vCorp[0].getBondYield();
        double corpTerm = vCorp[0].getBondTerm();
        
        Bond leftGovBond = Bond();
        Bond rightGovBond = Bond();
        
        double minYieldSpread = std::numeric_limits<double>::max();
        
        for (auto currentGovBond : vGov) {
            
            double govYield = currentGovBond.getBondYield();
            double govTerm = currentGovBond.getBondTerm();
            
            if( govTerm < corpTerm) { //gov bonds are sorted by term so the last set leftmost bond is used for interpolation
                
                leftGovBond = currentGovBond;
            }
            else if( govTerm == corpTerm) { //gov bond term matches corporate, no need to interpolate curve
                
                minYieldSpread = corpYield - govYield;
                
                break;
            }
            else { //gov bonds are sorted by term so the first set rightmost bond is used for interpolation
             
                rightGovBond = currentGovBond;
                
                minYieldSpread = interpolateCurvePointYield(leftGovBond, rightGovBond, vCorp[0]);
                break;
            }
        }
        
       
    if(std::to_string(minYieldSpread) == "1.430000") {
        std::cout << "findCurveSpread PASSED" << std::endl;
    }
    else {
        std::cout << "findCurveSpread FAILED" << std::endl;
    }
        
    std::cout << std::endl;
}




int main(int argc, const char * argv[]) {
    
    std::cout << std::endl;
    std::cout << "Overbond Test \n";
    
//    std::shared_ptr<Bond> pFork = std::make_shared<Bond>(true);
//    pFork->printBondMessage();
    
    std::string fileName;
    //std::string outputFile;
    //std::string outputString;
    
    if(argc == 2) {
        fileName = argv[1];
   //     outputFile = argv[2];
    }
    else {
        std::cout << "input filename is required!" << std::endl;
        fileName = "/Users/fernando/Developer/Xcode/overbond/overbond/inputbonds.csv";
    }
    
    std::vector<Bond> vCorp;
    std::vector<Bond> vGov;
    
    //const std::string fileName = "/Users/fernando/Developer/Xcode/overbond/overbond/inputbonds.csv";
    
    getBondData(fileName, vCorp, vGov);
    std::sort(vGov.begin(), vGov.end(), std::less<Bond>()); //sort government bonds in case they are not sorted by increasing term

    
    std::vector<BondBenchmarkSpread> benchmarkSpreadVector;
    std::vector<BondCurveSpread> benchmarkCurveVector;
    
    testChalenge1(vCorp, vGov, benchmarkSpreadVector);
    testChalenge2(vCorp, vGov, benchmarkCurveVector);
    
    
    std::cout << "challenge #1" << std::endl;
    
    findBenchmarkSpread(vCorp, vGov, benchmarkSpreadVector);
    
    std::cout << "bond, benchmark, spread_to_benchmark\n";
    for(auto bondSpread : benchmarkSpreadVector) {
    
        std::cout << std::endl;
        std::cout << bondSpread.corporateBond << ", " << bondSpread.governmentBond << ", " << bondSpread.spread << "%" << std::endl;
    }
    std::cout << std::endl;
    
    
   
    
    
    std::cout << "challenge #2" << std::endl;
    
    findCurveSpread(vCorp, vGov, benchmarkCurveVector);
    
    std::cout << "bond, spread_to_curve\n";
    for(auto bondSpread : benchmarkCurveVector) {
    
        std::cout << std::endl;
        std::cout << bondSpread.corporateBond << ", " << bondSpread.spread << "%" << std::endl;
    }
    

    return 0;
}
