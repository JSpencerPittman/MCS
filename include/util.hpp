#ifndef UTIL_H
#define UTIL_H

#include <vector>

namespace util {

    typedef std::vector<unsigned char> ByteArray;
    typedef std::vector<bool> BinArray;

    template<typename T>
    ByteArray SerializeInteger(T tNum, bool bBigEndian = false) {
        unsigned int unNumOfBytes = sizeof(tNum);
        ByteArray vResults;

        vResults.reserve(unNumOfBytes);

        for(unsigned int unIdx = 0; unIdx < unNumOfBytes; ++unIdx) {
            vResults.emplace_back(tNum & 0xFF);
            tNum = tNum>>8;
        }

        if(bBigEndian)
            vResults = ByteArray(vResults.rbegin(), vResults.rend());

        return vResults;
    }

    ByteArray Bytify(const BinArray& binArr) {
        if(binArr.size() % 8 != 0)
            throw std::invalid_argument("Provide a binary array that has a size divisible by eight.");

        unsigned int unTotalBytes = binArr.size() / 8;
        ByteArray byteArr.reserve(unTotalBytes);

        for(unsigned int unIdx = 0; unIdx < unTotalBytes; ++unIdx) {
            unsigned char unByte = 0;
            for(unsigned int unByteIdx = 0; unByteIdx < 8; ++unByteIdx) {
                bool bit = binArr[(unIdx*8) + unByteIdx];
                if(bit) unByte += std::pow(2, 7-unByteIdx);
            }
            byteArr.push_back(unByte);
        }

        return byteArr;
    }

    template<typename T>
    BinArray Binarize(T tNum, unsigned int unFillTo = 0) {
        BinArray vBinary;
        if(unFillTo) vBinary.reserve(unFillTo);

        while(tNum) {
            vBinary.emplace_back(tNum & 1);
            tNum = tNum >> 1;
        }

        if(vBinary.size() < unFillTo)
            vBinary.resize(unFillTo, false);

        vBinary = BinArray(vBinary.rbegin(), vBinary.rend());

        return vBinary;        
    } 

};

#endif