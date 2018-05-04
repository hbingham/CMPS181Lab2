#ifndef _rm_h_
#define _rm_h_

#include <string>
#include <vector>
#include <list>
#include <map>
#include <string>
#include <cstring>

#include "../rbf/rbfm.h"

#define CATELOG_FILE_NAME "Tables"
#define ATTR_FILE_NAME "Columns"

using namespace std;

# define RM_EOF (-1)  // end of a scan operator

class Column{
public:
	int tableId;
	string name;
	AttrType columnType;
	int length;
	int position;
	int deleteFLag;
	RID rid;

	Column(){}
	Column(int tableId, const char* name, AttrType columnType, int length, int position, int deleteFlag){
		this->tableId = tableId;
		this->name = name;
		this->columnType = columnType;
		this->length = length;
		this->position = position;
		this->deleteFLag = deleteFlag;
	}

	RC toAPIRecordFormat(void* data);
};


class MyTable{
public:
	int id;
	string name;
	string filaname;
	std::list<Column*> column_list;
	RID rid;

public:
	MyTable(){};
	MyTable(int id, const string& name, const string& filename){
		this->id = id;
		this->name = name;
		this->filaname = filename;
	}

	RC toAPIRecordFormat(void* data);
};


// RM_ScanIterator is an iteratr to go through tuples
class RM_ScanIterator {
public:
  RM_ScanIterator() {};
  ~RM_ScanIterator() {};

  // "data" follows the same format as RelationManager::insertTuple()
  RC getNextTuple(RID &rid, void *data);
  RC close();

  /*  My Methods */
  bool hasNextTuple();

  RC createNewScanner(FileHandle fileHandler,vector<Attribute> attrs, const string &conditionAttribute, const CompOp compOp,
			const void *value, const vector<string> &attributeNames);

public:
  RBFM_ScanIterator rbfm_scanner;
};

// Relation Manager
class RelationManager
{
public:
  static RelationManager* instance();

  RC createCatalog();

  RC deleteCatalog();

  RC createTable(const string &tableName, const vector<Attribute> &attrs);

  RC deleteTable(const string &tableName);

  RC getAttributes(const string &tableName, vector<Attribute> &attrs);

  RC insertTuple(const string &tableName, const void *data, RID &rid);

  RC deleteTuple(const string &tableName, const RID &rid);

  RC updateTuple(const string &tableName, const void *data, const RID &rid);

  RC readTuple(const string &tableName, const RID &rid, void *data);

  // Print a tuple that is passed to this utility method.
  // The format is the same as printRecord().
  RC printTuple(const vector<Attribute> &attrs, const void *data);

  RC readAttribute(const string &tableName, const RID &rid, const string &attributeName, void *data);

  // Scan returns an iterator to allow the caller to go through the results one by one.
  // Do not store entire results in the scan iterator.
  RC scan(const string &tableName,
      const string &conditionAttribute,
      const CompOp compOp,                  // comparison type such as "<" and "="
      const void *value,                    // used in the comparison
      const vector<string> &attributeNames, // a list of projected attributes
      RM_ScanIterator &rm_ScanIterator);

// Extra credit work (10 points)
public:
  RC addAttribute(const string &tableName, const Attribute &attr);

  RC dropAttribute(const string &tableName, const string &attributeName);


  /************************************************My Methods******************************************************************/
public:
//  RC importCatalog();
//  RC updateCatelogMetaData();
//  RC exportCatalog();

  RC createSysTableFiles();

  RC convertColumnIntoAttribute(Column* col, vector<Attribute> &attrs, bool showDeleted);

  RC dataComparator();

  RC importTableMapper();

  void loadTableDescriptors(vector<Attribute>& tablesAttrs);
  void loadColumnsDescriptors(vector<Attribute>& colsAttrs);




//  RC exportTableMapper();

  RC getAllAttributes(const string &tableName, vector<Attribute> &attrs);
  /************************************************My Attributes******************************************************************/
public:
  std::map<string, MyTable*> table_mapper;

  RecordBasedFileManager* rbfm;

protected:
  RelationManager();
  ~RelationManager();

private:
  static RelationManager *_rm;

  void constructTableFromAPIData(MyTable* table, void* data);
  void constructColumnFromAPIData(Column* column, void* data);
};




#endif