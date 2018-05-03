
#include "rm.h"

RelationManager* RelationManager::_rm = 0;

RelationManager* RelationManager::instance()
{
    if(!_rm)
        _rm = new RelationManager();

    return _rm;
}

RelationManager::RelationManager()
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
    table.push_back(attr);
    
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
    return -1;
}

RC RelationManager::createTable(const string &tableName, const vector<Attribute> &attrs)
{

    RC returnCode;
    RC thisReturnCode;
    FileHandle fileHandle;
    //Initialized this in the constructor, not sure if I need it again?
    //RecordBasedFileManager *rbfm = RecordBasedFileManager::instance();

    returnCode = rbfm->createFile(getFileName(tableName));
    if(returnCode)
    {
      return returnCode;
    }


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



