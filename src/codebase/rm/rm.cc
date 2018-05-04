
#include "rm.h"
#include <iostream>

RelationManager* RelationManager::_rm = 0;

RelationManager* RelationManager::instance()
{
    if(!_rm)
        _rm = new RelationManager();

    return _rm;
}

RelationManager::RelationManager() : tableCounter(1)
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
    return -1;
}

RC RelationManager::deleteCatalog()
{
    RecordBasedFileManager *rbfm = RecordBasedFileManager::instance();
    RC returnCode;
    returnCode = rbfm->destroyFile("Tables.t");
    rbfm->destroyFile("Columns.t");
    return 0;
}

RC RelationManager::createTable(const string &tableName, const vector<Attribute> &attrs)
{


/*
    RC returnCode;
    RC thisReturnCode;
    FileHandle fileHandle;
    RID thisRID;

    //Initialized this in the constructor, not sure if I need it again?
    //RecordBasedFileManager *rbfm = RecordBasedFileManager::instance();

    returnCode = rbfm->createFile(tableName + ".t");
    if(returnCode)
    {
      return returnCode;
    }

//Find the largest tableID, so we can create a table at that value + 1
    thisReturnCode = rbfm->openFile("Tables.t", fileHandle);
    if(thisReturnCode)
    {
      return thisReturnCode;
    }

    vector<string> attributes;
    attributes.push_back("table-id");
    RBFM_ScanIterator RBFMscanIterator;
    thisReturnCode = rbfm->scan(fileHandle, tableDescriptor, "table-id", NO_OP, NULL, attributes, RBFMscanIterator);
    if(thisReturnCode)
    {
      return thisReturnCode;
    }

    void *thisData = malloc(5); //size of an int, + 1
    int tableID = 0;
    char nullChar;
*/








    return -1;
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


