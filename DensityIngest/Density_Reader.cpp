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

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>	// sqrt, pow
#include "densityingest_error.h"

#include "Density_Reader.h"

#define ROUND(a) (a)>=0 ? floor((a)+0.5) : floor((a)-0.5) 

namespace Density {
    DensityReader::DensityReader() {
        //counter = 0;
        //currRow = -1;
    }
    
    DensityReader::DensityReader(std::string newFileName, int newSwap, int newNrecord, int newNgrid, long newSumdens, int newSnapnum, int newFileFormat) {          
        
        long ncells;
        int exponent;

        bswap = newSwap;
        snapnum = newSnapnum;
        ngrid = newNgrid;
        nrecord = newNrecord;
        sumdens = newSumdens;
        fileFormat = newFileFormat;


        if (fileFormat == 0) {
            // File format as delivered by Anatoly Klypin
            // 
            // Determine factor to scale down densities to real overdensity, 
            // rho_ov = rho/rho_back - 1.
            // Densities given in file are only particles per cell, 
            // i.e. rho = dN/dV, with dV = 1, 
            // and sum(dN/dV) = sum(dN) = sumdens (for Bolshoi: 2048^3)
            // Thus, rho_ov = (dN/dV) / (N/V) - 1 = dens / (sumdens/ngrid^3)

            densfactor =  ROUND( pow( pow(sumdens,0.3333333)/ngrid, 3) );
            printf("Scaling factor for densities: %d\n", densfactor);
        } else {
            densfactor = 1; // actually not used so far ...
        }

        ncells =  pow(ngrid,3);
        exponent = (int) ( log10(ncells)+1 );

        idfactor = pow(10, exponent); 
        printf("Factor for ids: %ld, %d\n", idfactor, snapnum);

        
        counter = 0; // counts all densities
        countInBlock = 0; // counts densities in each data block (record)
        
        numBytesPerRow = 1*sizeof(float);
        numBytesPerBlock = nrecord*sizeof(float);
       

        datablock = (char *) malloc(numBytesPerBlock);
        if (datablock == NULL) {printf("Memory error for datablock\n"); exit(2);}


        openFile(newFileName);

        if (fileFormat == 1) {
            // File format by Noam, 
            // a header is given and should be read
            readHeader(ngrid);
        }

    }
    
    
    DensityReader::~DensityReader() {
        if (datablock != NULL) free(datablock);
        closeFile();
    }
    
    void DensityReader::openFile(string newFileName) {
        if (fileStream.is_open())
            fileStream.close();

        // open binary file
        fileStream.open(newFileName.c_str(), ios::in | ios::binary);
        
        if (!(fileStream.is_open())) {
            DensityIngest_error("DensityReader: Error in opening file.\n");
        }
        
        fileName = newFileName;

    }
    
    void DensityReader::closeFile() {
        if (fileStream.is_open())
            fileStream.close();
    }
   
    int DensityReader::readHeader(int ngrid_user) {
        if (!(fileStream.is_open())) {
            DensityIngest_error("DensityReader: File is not open!\n");
        }
        int numHeaderBytes;
        int iskip, skipsize;
        long ntot;
        float boxsize;
        float mass_particle;
        int ngrid_file;

        skipsize = 4;
        numHeaderBytes = 1*4 + 1*8 + 1*4 + 1*4  +  4*2*skipsize;

        char memchunk[numHeaderBytes];


        if (!fileStream.read(memchunk,numHeaderBytes)) {
            printf("End of file reached.\n");
            return false;
        }
        assignInt(&iskip, &memchunk[0], bswap);
        assignInt(&ngrid_file, &memchunk[4], bswap);
        assignInt(&iskip, &memchunk[8], bswap);

        assignInt(&iskip, &memchunk[12], bswap);
        assignLong(&ntot, &memchunk[16], bswap);
        assignInt(&iskip, &memchunk[24], bswap);

        assignInt(&iskip, &memchunk[28], bswap);
        assignFloat(&boxsize, &memchunk[32], bswap);
        assignInt(&iskip, &memchunk[36], bswap);

        assignInt(&iskip, &memchunk[40], bswap);
        assignFloat(&mass_particle, &memchunk[44], bswap);
        assignInt(&iskip, &memchunk[48], bswap);

        printf("Header values: ngrid_file: %d ntot: %ld boxsize: %f particle mass: %f\n",
            ngrid_file, ntot, boxsize, mass_particle);

        if (ngrid_user != ngrid_file) {
            fprintf(stderr,"DensityReader: Number of grid cells from user input (%d) does not match value in file (%d)!\n",
                ngrid_user, ngrid_file);
            exit(0);
        }

        return ngrid_file;
    }

    int DensityReader::readNextBlock() {
        // read complete block into density array
        assert(fileStream.is_open());

        int skipsize, iskip; 
        //int datasize;
        char memchunk[numBytesPerRow];

        skipsize = 4;
        //datasize = 4;
        
        // need to skip integer that starts
        // the next data block
        if (!fileStream.read(memchunk,skipsize)) {
            printf("End of file reached.\n");
            return false;
        }
        assignInt(&iskip, &memchunk[0], bswap);

        // include one more check here:
        if (iskip != (nrecord*numBytesPerRow)) {
            printf("Error: block size (%d) does not agree with nrecord*numBytesPerRow (%d). Exit.\n",
                iskip, nrecord*numBytesPerRow);
            return false;
        }

        if (!fileStream.read(datablock,numBytesPerBlock)) {
            return false;
        }

        fileStream.read(memchunk,skipsize);
        assignInt(&iskip, &memchunk[0], bswap);

        return true;

    }
    
    int DensityReader::getNextRow() {
        // read one line, from already read memchunk-block

        assert(fileStream.is_open());

        int rem;
        
        if (counter == ngrid*ngrid*ngrid) {
            // We have read all the densities already, so we are done.
            printf("End of density part is reached.\n");
            return false;
        }

        if (countInBlock == nrecord || counter == 0) {
            // end of data block/start of new one is reached!
            if (!readNextBlock()) return false;


            // reset countInBlock:
            countInBlock = 0;
        }

        // read the value from datablock
        // and assign it to the proper variables
        assignFloat(&dens, &datablock[countInBlock*numBytesPerRow], bswap);
        
        // convert to real overdensities, if these were only counts in cells,
        // otherwise use dens as is
        // dens = overdensity(dens, densfactor);


        iz = floor(counter/(ngrid*ngrid));

        rem = counter % (ngrid*ngrid);
        iy = floor( rem/ngrid );
        ix = rem % ngrid;

        //printf("webId, ix, iy, iz: %ld %d %d %d %f\n", webId, ix, iy, iz, dens);

        webId = snapnum * idfactor + counter;

        counter++;
        countInBlock++;
	
        return true;       
    }
    
    float  DensityReader::overdensity(float dens, float densfactor) {
        // determine overdensity based on counts of particles in cells
        dens = dens / densfactor - 1;
        return dens;
    }

    
    bool DensityReader::getItemInRow(DBDataSchema::DataObjDesc * thisItem, bool applyAsserters, bool applyConverters, void* result) {
        
        bool isNull;

        isNull = false;

        //reroute constant items:
        if(thisItem->getIsConstItem() == true) {
            getConstItem(thisItem, result);
            isNull = false;
        } else if (thisItem->getIsHeaderItem() == true) {
            printf("We never told you to read headers...\n");
            exit(EXIT_FAILURE);
        } else {
            isNull = getDataItem(thisItem, result);
        }
        
        // assertions and conversions could be applied here
        // but do not need them now.

        // return value: if true: just NULL is written, result is ignored.
        // if false: the value in result is used.
        return isNull;
    }
    
    bool DensityReader::getDataItem(DBDataSchema::DataObjDesc * thisItem, void* result) {

        //check if this is "Col1" etc. and if yes, assign corresponding value
        //the variables are declared already in Pmss_Reader.h 
        //and the values were read in getNextRow()
        bool isNull;

        isNull = false;
        if(thisItem->getDataObjName().compare("Col1") == 0) {           
            *(long*)(result) = webId;
        } else if (thisItem->getDataObjName().compare("Col2") == 0) {
            *(int*)(result) = ix;
        } else if (thisItem->getDataObjName().compare("Col3") == 0) {
            *(int*)(result) = iy;
        } else if (thisItem->getDataObjName().compare("Col4") == 0) {
            *(int*)(result) = iz;
        } else if (thisItem->getDataObjName().compare("Col5") == 0) {
            phkey = 0;
            *(int*)(result) = phkey;
            // better: let DBIngestor insert Null at this column
            // => need to return 1, so that Null will be written.
            isNull = true;
        } else if (thisItem->getDataObjName().compare("Col6") == 0) {
            *(float*)(result) = dens;
        } else if (thisItem->getDataObjName().compare("Col7") == 0) {
            *(int*)(result) = snapnum;
        } else {
            printf("Something went wrong...\n");
            exit(EXIT_FAILURE);
        }

        return isNull;

    }

    void DensityReader::getConstItem(DBDataSchema::DataObjDesc * thisItem, void* result) {
        memcpy(result, thisItem->getConstData(), DBDataSchema::getByteLenOfDType(thisItem->getDataObjDType()));
    }


    // write part from memoryblock to integer; byteswap, if necessary (TODO: use global 'swap' or locally submit?)
    int DensityReader::assignInt(int *n, char *memblock, int bswap) {
        
        unsigned char *cptr,tmp;

        if (sizeof(int) != 4) {
            fprintf(stderr,"assignInt: sizeof(int)=%ld and not 4. Can't handle that.\n",sizeof(int));
            return 0;
        }

        if (!memcpy(n,  memblock, sizeof(int))) {
            fprintf(stderr,"Error: Encountered end of memory block or other trouble when reading the memory block.\n");
            return 0;
        }

        if (bswap) {
            cptr = (unsigned char *) n;
            tmp     = cptr[0];
            cptr[0] = cptr[3];    
            cptr[3] = tmp;
            tmp     = cptr[1];
            cptr[1] = cptr[2];
            cptr[2] = tmp;
        }

        return 1;
    }


    // write part from memoryblock to long; byteswap, if necessary (TODO: use global 'swap' or locally submit?)
    int DensityReader::assignLong(long *n, char *memblock, int bswap) {
        
        unsigned char *cptr,tmp;

        if (sizeof(long) != 8) {
            fprintf(stderr,"assignLong: sizeof(long)=%ld and not 8. Can't handle that.\n",sizeof(long));
            return 0;
        }

        if (!memcpy(n,  memblock, sizeof(long))) {
            fprintf(stderr,"Error: Encountered end of memory block or other trouble when reading the memory block.\n");
            return 0;
        }

        if (bswap) {
            cptr = (unsigned char *)n;
            tmp     = cptr[0];
            cptr[0] = cptr[7];
            cptr[7] = tmp;
            tmp     = cptr[1];
            cptr[1] = cptr[6];
            cptr[6] = tmp;
            tmp     = cptr[2];
            cptr[2] = cptr[5];
            cptr[5] = tmp;
            tmp     = cptr[3];
            cptr[3] = cptr[4];
            cptr[4] = tmp;
        }

        return 1;
    }


    // write part from memoryblock to float; byteswap, if necessary
    int DensityReader::assignFloat(float *n, char *memblock, int bswap) {
        
        unsigned char *cptr,tmp;
        
        if (sizeof(float) != 4) {
            fprintf(stderr,"assignFloat: sizeof(float)=%ld and not 4. Can't handle that.\n",sizeof(float));
            return 0;
        }
        
        if (!memcpy(n, memblock, sizeof(float))) {
            printf("Error: Encountered end of memory block or other trouble when reading the memory block.\n");
            return 0;
        }
        
        if (bswap) {
            cptr = (unsigned char *) n;
            tmp     = cptr[0];
            cptr[0] = cptr[3];    
            cptr[3] = tmp;
            tmp     = cptr[1];
            cptr[1] = cptr[2];
            cptr[2] = tmp;
        }
        return 1;
    }

    int DensityReader::swapInt(int i, int bswap) {
        unsigned char *cptr,tmp;
        
        if ( (int)sizeof(int) != 4 ) {
            fprintf( stderr,"Swap int: sizeof(int)=%d and not 4\n", (int)sizeof(int) );
            exit(0);
        }
        
        if (bswap) {
            cptr = (unsigned char *) &i;
            tmp     = cptr[0];
            cptr[0] = cptr[3];
            cptr[3] = tmp;
            tmp     = cptr[1];
            cptr[1] = cptr[2];
            cptr[2] = tmp;
        }

        return i;
    }

    float DensityReader::swapFloat(float f, int bswap) {
        unsigned char *cptr,tmp;
        
        if (sizeof(float) != 4) {
        fprintf(stderr,"Swap float: sizeof(float)=%d and not 4\n",(int)sizeof(float));
        exit(0);
        }
         
        if (bswap) {
            cptr = (unsigned char *)&f;
            tmp     = cptr[0];
            cptr[0] = cptr[3];
            cptr[3] = tmp;
            tmp     = cptr[1];
            cptr[1] = cptr[2];
            cptr[2] = tmp;
        }

        return f;
    }

}
