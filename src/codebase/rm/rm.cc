#include <cstdlib>
#include<string.h>
#include "rm.h"
#include <iostream>

//tableEntry includes 2 ints(8): TID & sysFlag
//2 varchars(108): fName, tName,  & 1 nullByte(1)
#define tableEntrySize 1 + 108 + 8
//1 nullByte, 1 varchar (cName)
// 4 ints: len, pos, type, TID
#define columnEntrySize 1 + 54 + 16
RelationManager* RelationManager::_rm = 0;

RelationManager* RelationManager::instance()
{
    if(!_rm)
        _rm = new RelationManager();

    return _rm;
}

RelationManager::RelationManager() : nextTid(1)
{
    pfm = PagedFileManager::instance();
    rbfm = RecordBasedFileManager::instance();
    

    //Table initialized
    Attribute attr;
    attr.name = "table-id";
    attr.type = TypeInt;
    attr.length = 4;
    table.push_back(attr);

    attr.name = "table-name";
    attr.type = TypeVarChar;
    attr.length = 50;
    table.push_back(attr);

    attr.name = "file-name";
    attr.type = TypeVarChar;
    attr.length = 50;
    table.push_back(attr);


    //Either 1 or 0, 1 means can access, 0 means no access.
    attr.name = "system"; 
    attr.type = TypeInt;
    attr.length = 4;
    table.push_back(attr);


    //Column initialized
    attr.name = "table-id";
    attr.type = TypeInt;
    attr.length = 4;
    column.push_back(attr);
    
    attr.name = "column-name";
    attr.length = 50;
    attr.type = TypeVarChar;
    column.push_back(attr);
    
    attr.name = "column-type";
    attr.length = 4;
    attr.type = TypeInt;
    column.push_back(attr);
    
    attr.name = "column-length";
    attr.length = 4;
    attr.type = TypeInt;
    column.push_back(attr);
    
    attr.name = "column-position";
    attr.length = 4;
    attr.type = TypeInt;
    column.push_back(attr);

    if (doesExist("Tables.tbl")) 
    {
        load();
    }
    else 
    {
      createTable("Tables", table, "System");
      createTable("Columns", column, "System");
    }

    
    
}

RelationManager::~RelationManager()
{

}

RC RelationManager::createCatalog()
{
    RC rc;
    string tableName = "Tables";
    string columnsName = "Columns";
    rc = rbfm->createFile("Tables.t");
    if (rc) return rc;
    rc = rbfm->createFile("Columns.t");
    if (rc) return rc;

    rc = saveTable(1, tableName, 1);
    if (rc) return rc;
    rc = saveTable(2, columnsName, 1); 
    if (rc) return rc;

    rc = saveTableColumns(1, table);
    if (rc) return rc;
    rc = saveTableColumns(1, column);
    if (rc) return rc;
    return 0;
}

RC RelationManager::deleteCatalog()
{
    RecordBasedFileManager *rbfm = RecordBasedFileManager::instance();
    RC rc;
    rc  = rbfm->destroyFile("Tables.t");
    if (rc) return rc;
    rbfm->destroyFile("Columns.t");
    return 0;
}

RC RelationManager::createTable(const string &tableName, const vector<Attribute> &attrs)
{
    RC rc;
    FileHandle fileHandle;

    rc = rbfm->createFile(tableName + ".t");
    if(rc)
    {
      return rc;
    }
    int tableId = this ->nextTid;
    rc = saveTable(tableId, tableName, 0);
    rc = saveTableColumns(tableId, attrs);

    this->nextTid++;
    return 0;
}

RC RelationManager::deleteTable(const string &tableName)
{
    return -1;
}

RC RelationManager::getAttributes(const string &tableName, vector<Attribute> &attrs)
{
    return -1;
}

RC RelationManager::insertTuple(const string &tableName, const void *data, RID &rid)
{
    return -1;
}

RC RelationManager::deleteTuple(const string &tableName, const RID &rid)
{
    return -1;
}

RC RelationManager::updateTuple(const string &tableName, const void *data, const RID &rid)
{
    return -1;
}

RC RelationManager::readTuple(const string &tableName, const RID &rid, void *data)
{
    return -1;
}

RC RelationManager::printTuple(const vector<Attribute> &attrs, const void *data)
{
  RecordBasedFileManager *rbfm = RecordBasedFileManager::instance();

  return rbfm->printRecord(attrs, data);
}

RC RelationManager::readAttribute(const string &tableName, const RID &rid, const string &attributeName, void *data)
{
    return -1;
}

RC RelationManager::scan(const string &tableName,
      const string &conditionAttribute,
      const CompOp compOp,                  
      const void *value,                    
      const vector<string> &attributeNames,
      RM_ScanIterator &rm_ScanIterator)
{
    return -1;
}
RC RM_ScanIterator::getNextTuple(RID &rid, void *data)
{
    return rbfm_scanIterator.getNextRecord(rid, data);
}

RC RM_ScanIterator::close()
{
    RecordBasedFileManager *rbfm = RecordBasedFileManager::instance();
    rbfm->closeFile(fileHandle);
    return SUCCESS;
}
RC RelationManager::load() 
{

  RM_ScanIterator rmScanIterator;
  vector <string> attributeName;
  RID thisRID;

  //tableID
  attributeName.push_back(table[0].name);
  //tableName
  attributeName.push_back(table[1].name);

  scan("Tables", table[0].name, NO_OP, NULL, attributeNames, rmScanIterator);

  //Max table record size.
  char *data = (char*) malloc(768);
  char *BOF = data;

  int maxTable = 0;
  int tableID = 0;

  while(rmScanIterator.getNextTuple(thisRID, data) != RM_EOF) 
  {
    memcpy(&tableID, data, sizeof(int));

    int tableNameLength;
    data = data + sizeof(int); //jump the length
    memcpy(&tableNameLength, data, sizeof(int));

    data = data + sizeof(int);
    string tableName = string(data, tableNameLength);

    if(tableId > maxTable)
      maxTable = tableId;

    map<int, RID> * tableIDToRidMap = new map<int, RID> ();
    (*tableIDToRidMap)[tableId] = rid;

    tablesMap[tableName] = tableIDToRidMap;
    data = BOF;
  }

  tableCounter = maxTableID + 1; //get the max and add 1
  rmScanIterator.close();

  attributeName.clear();
  attributeName.push_back(column[0].name); //table id
  attributeName.push_back(column[4].name); //column position
  scan("Columns", column[0].name, NO_OP, NULL, attributeName, rmScanIterator);

  int columnPos;
  while (rmScanIterator.getNextTuple(thisRID, data) != RM_EOF) 
  {
    memcpy(&tableId, data, sizeof(int));

    data = data + sizeof(int);
    memcpy(&columnPos, data, sizeof(int));

    if (columnsMap.find(tableId) != columnsMap.end()) {
      (*columnsMap[tableId])[columnPosition] = rid;
    } else {
      map<int, RID> * colEntryMap = new map<int, RID> ();
      (*colEntryMap)[columnPosition] = rid;
      (columnsMap[tableId]) = colEntryMap;
    }

    data = beginOfData;
  }
  rmsi.close();

  attributeNames.clear();

  return 0;
}

//returns true if the file exists, else returns false.
bool RelationManager::doesExist(string fileName)
{
  struct stat buf;
  return (stat(fileName.c_str(), &buf) == 0);
}

RC RelationManager::saveTable(int tableId,const string &name, int sysFlag)
{
   FileHandle fh;
   RC rc;
   RID rid;
   int nameLen = name.length();
   rc = rbfm -> openFile("Tables.t", fh);
   if (rc) return rc;

   void *entryData = malloc(tableEntrySize);
   char nullByte = 0;
   memcpy((char*) entryData, &nullByte, 1);
   memcpy((char*) entryData + 1, &tableId, 4);
   memcpy((char*) entryData + 5, &nameLen, 4);
   memcpy((char*) entryData + 9, &name, 50);
   memcpy((char*) entryData + 59, &name, 50);
   memcpy((char*) entryData + 109, &sysFlag, 4);

   rc = rbfm-> insertRecord(fh, table, entryData, rid);
   if (rc) return rc;

   rbfm -> closeFile(fh);
   free(entryData);

   return 0;
}

RC RelationManager::saveTableColumns(int tableId, const vector<Attribute> &recordDescriptor)
{
   FileHandle fh;
   RC rc;
   RID rid;
   int nameLen, type, len, pos;
   rc = rbfm -> openFile("Tables.t", fh);
   if (rc) return rc;

   void *entryData = malloc(columnEntrySize);
   char nullByte = 0;
   for (unsigned i = 0; i < recordDescriptor.size(); i++)
{
   Attribute attr = recordDescriptor[i];
   nameLen = attr.name.length();
   type = attr.type;
   len = attr.length;
   pos = i+1;

   memcpy((char*) entryData, &nullByte, 1);
   memcpy((char*) entryData + 1, &tableId, 4);
   memcpy((char*) entryData + 5, &nameLen, 4);
   memcpy((char*) entryData + 9, attr.name.c_str(), 50);
   memcpy((char*) entryData + 59, &type, 4);
   memcpy((char*) entryData + 63, &len, 4);
   memcpy((char*) entryData + 67, &pos, 4);

   rc = rbfm-> insertRecord(fh, column, entryData, rid);
   if (rc) return rc;
}

   rbfm -> closeFile(fh);
   free(entryData);

   return 0;
}

