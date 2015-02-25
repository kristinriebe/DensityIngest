/*  
 *  Copyright (c) 2012, Kristin Riebe <kriebe@aip.de>,
 *                      Adrian M. Partl <apartl@aip.de>, 
 *                      eScience team AIP Potsdam
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

#include "Density_SchemaMapper.h"
#include <SchemaItem.h>
#include <DataObjDesc.h>
#include <DType.h>
#include <DBType.h>
#include <stdlib.h>

using namespace std;
using namespace DBDataSchema;

namespace Density {
    
    DensitySchemaMapper::DensitySchemaMapper() {
        
    }

    DensitySchemaMapper::DensitySchemaMapper(DBAsserter::AsserterFactory * newAssertFac, DBConverter::ConverterFactory * newConvFac) {
        assertFac = newAssertFac;
        convFac = newConvFac;
    }

    DensitySchemaMapper::~DensitySchemaMapper() {
        
    }
    
    DBDataSchema::Schema * DensitySchemaMapper::generateSchema(string dbName, string tblName) {
        DBDataSchema::Schema * returnSchema = new Schema();

        //set database and table name
        returnSchema->setDbName(dbName);
        returnSchema->setTableName(tblName);
        
        //setup schema items and add them to the schema
        
        //first create the data object describing the input data:
        DataObjDesc * col1Obj = new DataObjDesc();
        col1Obj->setDataObjName("Col1");	// column name (data file)
        col1Obj->setDataObjDType(DT_INT8);	// data file type
        col1Obj->setIsConstItem(false, false); // not a constant
        col1Obj->setIsHeaderItem(false);	// not a header item
        
        //then describe the SchemaItem which represents the data on the server side
        SchemaItem * schemaItem1 = new SchemaItem();
        schemaItem1->setColumnName("webId"); //EMPTY_SCHEMAITEM_NAME);	// field name in Database
        schemaItem1->setColumnDBType(DBT_BIGINT);		// database field type
        schemaItem1->setDataDesc(col1Obj);		// link to data object
        
        //add schema item to the schema
        returnSchema->addItemToSchema(schemaItem1);

        
        //this is the second column
        DataObjDesc * col2Obj = new DataObjDesc();
        col2Obj->setDataObjName("Col2");
        col2Obj->setDataObjDType(DT_INT4);
        col2Obj->setIsConstItem(false, false);
        col2Obj->setIsHeaderItem(false);
        
        //then describe the SchemaItem which represents the data on the server side
        SchemaItem * schemaItem2 = new SchemaItem();
        schemaItem2->setColumnName("ix");
        schemaItem2->setColumnDBType(DBT_INTEGER);
        schemaItem2->setDataDesc(col2Obj);
        
        //add schema item to the schema
        returnSchema->addItemToSchema(schemaItem2);
       
        
        //this is the 3. column
        DataObjDesc * col3Obj = new DataObjDesc();
        col3Obj->setDataObjName("Col3");
        col3Obj->setDataObjDType(DT_INT4);
        col3Obj->setIsConstItem(false, false);
        col3Obj->setIsHeaderItem(false);
        
        //then describe the SchemaItem which represents the data on the server side
        SchemaItem * schemaItem3 = new SchemaItem();
        schemaItem3->setColumnName("iy");
        schemaItem3->setColumnDBType(DBT_INTEGER);
        schemaItem3->setDataDesc(col3Obj);
        
        //add schema item to the schema
        returnSchema->addItemToSchema(schemaItem3);
       
        
        //this is the 4. column
        DataObjDesc * col4Obj = new DataObjDesc();
        col4Obj->setDataObjName("Col4");
        col4Obj->setDataObjDType(DT_INT4);
        col4Obj->setIsConstItem(false, false);
        col4Obj->setIsHeaderItem(false);
        
        //then describe the SchemaItem which represents the data on the server side
        SchemaItem * schemaItem4 = new SchemaItem();
        schemaItem4->setColumnName("iz");
        schemaItem4->setColumnDBType(DBT_INTEGER);
        schemaItem4->setDataDesc(col4Obj);
        
        //add schema item to the schema
        returnSchema->addItemToSchema(schemaItem4);
       
        
        //this is the 5. column
        DataObjDesc * col5Obj = new DataObjDesc();
        col5Obj->setDataObjName("Col5");
        col5Obj->setDataObjDType(DT_INT4);
        col5Obj->setIsConstItem(false, false);
        col5Obj->setIsHeaderItem(false);
        
        //then describe the SchemaItem which represents the data on the server side
        SchemaItem * schemaItem5 = new SchemaItem();
        schemaItem5->setColumnName("phkey");
        schemaItem5->setColumnDBType(DBT_INTEGER);
        schemaItem5->setDataDesc(col5Obj);
        
        //add schema item to the schema
        returnSchema->addItemToSchema(schemaItem5);
       

        //this is the 6. column
        DataObjDesc * col6Obj = new DataObjDesc();
        col6Obj->setDataObjName("Col6");
        col6Obj->setDataObjDType(DT_REAL4);
        col6Obj->setIsConstItem(false, false);
        col6Obj->setIsHeaderItem(false);
        
        //then describe the SchemaItem which represents the data on the server side
        SchemaItem * schemaItem6 = new SchemaItem();
        schemaItem6->setColumnName("dens");
        schemaItem6->setColumnDBType(DBT_FLOAT);
        schemaItem6->setDataDesc(col6Obj);

        //add schema item to the schema
        returnSchema->addItemToSchema(schemaItem6);

        //this is the 7. column
        DataObjDesc * col7Obj = new DataObjDesc();
        col7Obj->setDataObjName("Col7");
        col7Obj->setDataObjDType(DT_INT4);
        col7Obj->setIsConstItem(false, false);
        col7Obj->setIsHeaderItem(false);
        
        //then describe the SchemaItem which represents the data on the server side
        SchemaItem * schemaItem7 = new SchemaItem();
        schemaItem7->setColumnName("snapnum");
        schemaItem7->setColumnDBType(DBT_INTEGER);
        schemaItem7->setDataDesc(col7Obj);
        
        //add schema item to the schema
        returnSchema->addItemToSchema(schemaItem7);
       

        
        return returnSchema;
    }
}
