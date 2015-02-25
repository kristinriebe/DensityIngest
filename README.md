DensityIngest
=============

This code uses the DBIngestor library (https://github.com/adrpar/DBIngestor) 
to ingest binary density fields into a database.

It assumes that these catalogues come in the Fortran binary format used by 
Anatoly Klypin. Adjust the routines in the reader, if you have a different 
format.

For any questions, please contact me at
Kristin Riebe, kriebe@aip.de


Data files
-----------
The files were written with Fortran, option "unformatted", big endian, 
with 4-byte-reals. The record length is Ngrid^2, where Ngrid is the 
number of grid cells per dimension. Each record is wrapped with a 
leading and trailing 4-byte integer. 

They data files only contains density values, sorted row by row. 
Spatial grid indizes are derived from the position in the array in
DensityReader::getNextRow().

The density values in the file give the number of particles per cell,
dN/dV. With dV = 1, we get sum(dens) = npart^3 = total number of particles
in the simulation.


Features
----------
* Read given datafile and ingest the density values into a database table.
(see DensitySchemaMapper.cpp). Convert them to overdensities beforehand.
* Determine spatial grid indizes ix, iy, iz.

* Columns written to the database:

    webId	unique id for database entry, = snapnum* (some factor of 10) + i	
    ix		spatial grid index, ranging from 0 to 255
    iy
    iz
    phkey 	Peano-Hilbert key for the grid cell in which the particle is located, 
		is just filled with nulls. Values can be updated via the database 
        	server using e.g. libhilbert (https://github.com/adrpar/libhilbert)
    dens	overdensity
    snapnum 	snapshot number

* If swap=1 is given, values will be byteswapped
* The reader reads complete records to accelerate ingestion. Please take care that 
your machine has enough memory to fit Ngrid^2 floats into memory!


Installation
--------------
see INSTALL


Example
--------
An example data file and create-table statement are given in the 
Example directory.
Data can be ingested with a command line like this:

DensityIngest/build/FofIngest.x -s mysql -D TestDB -T Density -U myusername -P mypassword -H 127.0.0.1 -O 3306 -r 65536 -g 256 -M 426 -e 8589934592 -w 1 -d Density-0256-0416.dat

Replace myusername and mypassword with your own credentials. 
-s: type of database (e.g. mysql, unix_sqlsrv_odbc)
-D: database name
-T: table name
-H: host
-O: port
-d: data file 
-M: snapshot number (snapnum)
-r: record length
-g: grid cells for ix,iy,iz, per dimension
-e: sum of densities (sum of total number of particles)

NOTE: One can also use -R 1. This would try to resume the connection, 
if something fails. But then be careful and check later on if all rows were 
ingested and contain meaningful values. Erroneous rows must be deleted manually. 

NOTE: The example data file contains the density field at redshift z=0 for the Bolshoi simulation, 
created by Anatoly Klypin. Also 
see the [CosmoSim database](http://www.cosmosim.org/), for which this code was used to ingest
density fields. 
