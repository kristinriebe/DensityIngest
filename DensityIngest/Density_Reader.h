/*  
 *  Copyright (c) 2016, Kristin Riebe <kriebe@aip.de>,
 *                      E-Science team AIP Potsdam
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  See the NOTICE file distributed with this work for additional
 *  information regarding copyright ownership. You may obtain a copy
 *  of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include <Reader.h>
#include <string>
#include <fstream>
#include <stdio.h>
#include <assert.h>

#ifndef Density_Density_Reader_h
#define Density_Density_Reader_h

using namespace DBReader;
using namespace DBDataSchema;
using namespace std;

namespace Density {
    
    class DensityReader : public Reader {
    private:
        std::string fileName;
        
        std::ifstream fileStream;
        
        //int currRow;
        unsigned long numFieldPerRow;
        
        std::string buffer;
        std::string oneLine;
        
        std::string endLineFlag;

        //this is here for performance reasons. used to be in getItemInRow, but is allocated statically here to get rid
        //of many frees
        std::string tmpStr;
              
        string densityString;
        int counter;
        int countInBlock;   // counter for particles in each data block

        int numBytesPerRow;	
        long  numBytesPerBlock;

        int densfactor;   // for scaling densities
        long idfactor;       // for id in table, derived from ngrid

        // items from file
        char *datablock;

        float dens;

        //fields to be generated/converted/...
        long int webId;
        int ix;
        int iy;
        int iz;
        int phkey;



        // to be passed on from main
        int bswap;	// byte swap or not
        int ngrid;	// number of cells per dimension for ix,iy,iz
        int nrecord;   // lines per data block (record)
        int snapnum;   // snapshot number
        long sumdens;    // sum of all densities; needed for scaling to overdensity, usually = tot. num. of particles


          
    public:
        DensityReader();
        DensityReader(std::string newFileName, int swap, int nrecord, int ngrid, long sumdens, int snapnum);
        ~DensityReader();

        void openFile(std::string newFileName);

        void closeFile();

        int readNextBlock();
        
        int getNextRow();
        
        int assignInt(int *n, char *memblock, int bswap);
        int assignFloat(float *n, char *memblock, int bswap);   
        int swapInt(int i, int bswap);
        float swapFloat(float f, int bswap);
        
        bool getItemInRow(DBDataSchema::DataObjDesc * thisItem, bool applyAsserters, bool applyConverters, void* result);

        bool getDataItem(DBDataSchema::DataObjDesc * thisItem, void* result);

        void getConstItem(DBDataSchema::DataObjDesc * thisItem, void* result);
    };
    
}

#endif
