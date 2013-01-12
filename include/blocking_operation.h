
#ifndef PATENT_BLOCKING_OPERATION_H
#define PATENT_BLOCKING_OPERATION_H

#include "string_manipulator.h"

/**
 * cBlocking_Operation
 *  - cBlocking_Operation_Column_Manipulate
 *  - BlockByColumns
 *
 * cBlocking_Operation reads data from a const Record pointer,
 * and extract information to create a string which represents
 * the blocking id to which the record belongs.
 *
 * Example:
 *  Record recobj: firstname = "JACK", lastname = "SMITH", patent = "12345A"
 *  if cBlocking_Operation::delim == "###", then
 *  extract_blocking_info ( &recobj ) probably returns "J###SMI###"
 *  if the blocking operation
 *  just wants the first char of firstname and 3 chars from lastname.
 *  extract_column_info ( & recobj, 1 ) probably returns "SMI" if the
 *  information of the second column (which is lastname ) is desired.
 *  num_involved_column() probably returns 2. ( firstname + lastname).
 *  get_useless_string() returns "######".
 *
 */
class cBlocking_Operation {

protected:

   /**
    * string infoless:
    *   this string represents the string that has delimiters ONLY.
    *   i.e. no useful information at all.
    *   Usually, the variable is changed by concrete subclasses.
    */
    string infoless;

public:

   /**
    * const string & get_useless_string () const: returns the infoless string.
    */
    const string & get_useless_string () const {
      return infoless;
    }

   /**
    * static const string delim: the separator between columns. Defined in
    * cpp file. Will be used by other modules.
    */
    static const string delim;

   /**
    * virtual string extract_blocking_info(const Record *) const = 0:
    * pure virtual function to extract information from a Record pointer.
    */
    virtual string extract_blocking_info(const Record *) const = 0;

   /**
    * virtual string extract_column_info ( const Record *, uint32_t flag ) const:
    * virtual function that extract information from the (flag)th column.
    */
    virtual string extract_column_info (const Record *, uint32_t UP(flag)) const {
      throw cException_Other ("Function not defined yet.");
    }

    virtual ~cBlocking_Operation() {};

   /**
    * virtual uint32_t num_involved_columns() const: return number
    * of columns that the extraction operates on.
    */
    virtual uint32_t num_involved_columns() const {
      return 1;
    }
};



/**
 * cBlocking_Operation_Column_Manipulate:
 * This is a subclass of cBlocking_Operation.
 * Usage of the class is described in its base class.
 *
 * Example:
 * const StringTruncate stobj (0, 3, true);
 * cBlocking_Operation_Column_Manipulate bocmobj (stobj, cLastname::get_class_name());
 * // set up a bocmobj instance of cBlocking_Operation_Column_Manipulate,
 * which takes a StringTruncate object as the string manipulater,
 * // and applies the string manipulation on the "Last name" column.
 * Assuming a record recobj: Firstname = "JOHN", lastname = "FLEMING",
 * patent = "1234Q", Country = "CA", ... ...
 * bocmobj.extract_blocking_info( & recobj ) returns "FLE";
 *
 *
 * However, this concrete class is not used often, because the
 * next class (BlockByColumns)
 * is more widely used.
 *
 */
//class cBlocking_Operation_Column_Manipulate: public cBlocking_Operation {
class BlockColumn: public cBlocking_Operation {

private:
    const StringManipulator & sm;
    const uint32_t column_index;

public:
    explicit BlockColumn(const StringManipulator & inputsm,
        const string & colname)
      : sm(inputsm), column_index(Record::get_index_by_name(colname)) {
        infoless = delim;
    }
    // might be problematic here for PATSTAT if blocking on NAME b/c 
    // NAME.at(0) is first word in name only; dump name splitter?
    string extract_blocking_info(const Record * p) const {
      return sm.manipulate(* p->get_data_by_index(column_index).at(0));
    }

    string extract_column_info(const Record * p, uint32_t UP(flag)) const {
      return extract_blocking_info(p);
    }
};



/**
 * BlockByColumns:
 * This is a subclass of cBlocking_Operation, which extracts information
 * from several columns and returns a block specifier string.
 */

/**
 * Example:
 *
 * StringRemoveSpace rsobj;
 * //create a space removal string manipulator
 * StringNoSpaceTruncate nstobj;
 * // create a space removal and string truncation string manipulator
 * nstobj.set_truncater(2, 4, true);
 * // set the nstobj to truncate strings in the way such
 * // that the truncation starts from the 3rd char
 * // and grabs 4 chars (if exists) from the forward direction.
 * // ie. nstobj.manipulate("EXAMPLES") returns "AMPL"
 */

/**
 * vector < const StringManipulator * > vec_strman;
 * //initialize a string manipulator pointer vector
 * vec_strman.push_back( &rsobj );
 * vec_strman.push_back( &rsobj );
 * // the vector now has 3 elements.
 * vec_strman.push_back( & nstobj );
 */

/**
 * //initialize a vector container to store column
 * //names on which the string operations take place.
 * vector<string> vec_label;
 * vec_label.push_back(cFirstname::static_get_class_name() );
 * vec_label.push_back(cAssignee::static_get_class_name() );
 * // the vector also has 3 elements. Pay attention to the sequence!
 * vec_label.push_back(cLastname::static_get_class_name() );
 */

/**
 * // initialize a data position array.
 * uint32_t data_position[] = { 1, 0, 2 };
 * // initialize a vector to used the data position array
 * vector<const uint32_t *> vec_di ;
 * vec_di.push_back( data_position );
 * vec_di.push_back(data_position + 1);
 * // another vector that has 3 elements. Also notice the sequence.
 * vec_di.push_back(data_position + 2);
 */

/**
 * //create a multiple column blocking info extractor.
 * BlockByColumns mcmobj (vec_strman, vec_label, vec_di);
 */

/**
 * Assuming there is a Record object, recobj.
 * As is described above, recobj contains a
 * vector of const Attribute pointers.
 *
 * Say within the many Attribute pointers that recobj has, 3 of them are:
 * 1. const Attribute * pfirstname: pointing to an attribute
 * whose data vector contains 3 string pointers:
 *         1.1 pfirstname->data[0] points to "ALEXANDER"
 *         1.2 pfirstname->data[1] points to "NICOLAS"
 *         1.3 pfirstname->data[2] points to "ALEXNICOLE".
 * Then the first string manipulator, rsobj, will look for
 * the string of data[1] and returns "NICOLAS".
 */

/**
 * 2. const Attribute * passignee: pointing to an attribute
 * whose data vector contains 4 string pointers:
 *         2.1 passignee->data[0] points to "MICROSOFT CORP, LTD"
 *         2.2 passignee->data[1] points to "MICROSOFT CORPORATION, LTD"
 *         2.3 passignee->data[2] points to "MICROSOFT CHINA"
 *         2.4 passignee->data[3] points to "MICROSOFT SHANGHAI, CHINA"
 * Then the second string manipulator, also rsobj, will look
 * for the string of data[0] and returns "MICROSOFTCORP,LTD"
 */

/**
 * 3. const Attribute * plastname: pointing to an attribute
 * whose data vector contains 4 string pointers:
 *         3.1 plastname->data[0] points to "JOHNSON"
 *         3.2 plastname->data[1] points to "JOHNSON JR"
 *         3.3 plastname->data[2] points to "JOHN STONE"
 *         3.3 plastname->data[3] points to "II JOHNSTON"
 * Then the 3rd string manipulator, nstobj, will look for
 * the string of data[2] and, by removing space and truncating,
 * returns "HNST";
 */

/**
 * Assuming that the cBlocking_Operation::delim is "@", then
 * mcmobj.extract_blocking_info( & recobj ) returns "NICOLAS@MICROSOFTCORP,LTD@HNST@"
 * and mcmobj.extract_column_info (& recobj, 2) returns "HNST".
 */

class BlockByColumns : public cBlocking_Operation {

private:

   /**
    *   vector < const StringManipulator * > vsm: the vector
    *   of string manipulator pointers.
    */
    vector<const StringManipulator *> vsm;

   /**
    *   vector < uint32_t > indice: the vector of column indice
    *   from which the manipulators extract information, respectively.
    */
    vector<uint32_t> indice;

    vector<string> attributes_names;

   /**
    *   vector < const uint32_t * > pdata_indice: the vector of
    *   the const interger pointers indicating the positions of data in
    *   the data vector in Attribute on which the extractions take place.
    */
    vector<uint32_t> pdata_indice;

   /**
    *  BlockByColumns (
    *  const vector < const StringManipulator * > & inputvsm,
    *  const vector<string> & columnnames, const vector < uint32_t > & di )
    *     : This is a constructor of the class.
    *   inputvsm = the vector of string manipulator pointers
    *   columnnames = the vector of strings which reprensents
    *   the name of columns that the extractions will be applied on,
    *   respectively.
    *   di = the vector of indice in the Attribute::data,
    *   which will serve as the source string pointers.
    *   NOTE: di SHOULD NOT BE DESTRUCTED BEFORE THE
    *   BlockByColumns object is discarded.
    */
public:

   /**
    * BlockByColumns (
    * const StringManipulator * const* pinputvsm,
    * const string * pcolumnnames,
    * const uint32_t  * pdi,
    */
    BlockByColumns (
        const vector<const StringManipulator *> & inputvsm,
        const vector<string> & columnnames,
        const vector<uint32_t> & di);

   /**
    * This is another constructor of the class object.
    * pinputvsm = pointer of the string manipulator pointers array
    * pcolumnnames = pointer of the strings array.
    * pdi = pointer of the Attribute::data indice array.
    * num_col = number of involved columns.
    * NOTE: pdi SHOULD NOT BE DESTRUCTED BEFORE THE
    * BlockByColumns object is discarded.
    */
    BlockByColumns (
        const StringManipulator * const* pinputvsm,
        const string * pcolumnnames,
        const uint32_t * pdi,
        const uint32_t num_col);

   /**
    * string extract_blocking_info(const Record * p) const:
    * extracts information and returns a string.
    */
    string extract_blocking_info(const Record * p) const;

   /**
    * string extract_column_info ( const Record * p, uint32_t flag ) const:
    * extracts information from specified column and returns a string
    */
    string extract_column_info (const Record * p, uint32_t flag) const;

   /**
    * uint32_t num_involved_columns() const :
    * return number of involved columns.
    */
    uint32_t num_involved_columns() const {
      return vsm.size();
    }

    void reset_data_indice(const vector < uint32_t > & indice);

    const vector<string> & get_blocking_attribute_names() const {
      return attributes_names;
    }

    const vector<const StringManipulator *> & get_blocking_string_manipulators() const {
      return vsm;
    }
};



class ClusterInfo;

/**
 * cBlocking_Operation_By_Coauthor:
 * This is a subclass of cBlocking_Operation. It was used
 * to find the top N unique inventors' names of a given record.
 *
 * Although it is not used any more for blocking, it does,
 * however, provide other functionalities such as :
 * 1. sort records by patent through a binary tree. Ideal for fast search
 * 2. map unique record id to unique inventor id. This can
 * be regarded as an expression of result of disambiguation.
 * 3. map unique inventor id to number of records (patents) that the inventor holds.
 *
 * Therefore, this class is actually working beyond its
 * original design purpose. And one should be familiar
 * with the class members.
 *
 * Example:
 *
 * list < const Record * > complete_list;    //initialize a list
 * Do something here to add elements into the list.
 * cBlocking_Operation_By_Coauthors bobcobj ( complete_list, 2 );
 * //create an instance of cBlocking_Operation_By_Coauthors
 * // and set the maximum most prolific coauthors to 2,
 * for blocking information extraction.
 * bobcobj.build_uid2uinv_tree ( Good_Cluster );
 * // assuming Good_Cluster is a ClusterInfo object.
 * This builds an internal uid2uinv tree in bobcobj.
 * Now bobcobj's patent_tree and uid2uinv tree are ready to use.
 * bobcobj.extract_blocking_info(prec) returns the string of
 * the full name concatenation of prec's two most prolific coauthors.
 * for example: "KIA.SILVERBROOK##GEORGE.SPECTOR##", assuming the delimiter is "##";
 *
 */
class cBlocking_Operation_By_Coauthors : public cBlocking_Operation {


private:

   /**
    *  map<const Record *, RecordPList, cSort_by_attrib> patent_tree:
    *  a binary tree to sort records by patent information.
    *  key = const Record pointer, value = list < const Record * >,
    *  comparison function = an object of cSort_by_attrib, which
    *  includes the column information.
    */
    PatentTree patent_tree;

   /**
    *  map < const Record *, const Record * > uid2uinv_tree:
    *  a binary tree that maps a unique record pointer to its
    *  unique inventor record pointer.
    */
    Uid2UinvTree uid2uinv_tree;

   /**
    *  map < const Record *, uint32_t > uinv2count_tree:
    *  a binary tree that maps the unique inventor record pointer
    *  to the number of its associated patents.
    */
    map <const Record *, uint32_t> uinv2count_tree;

   /**
    *  const uint32_t num_coauthors: number of coauthors to
    *  extract for blocking. Usually it should not be too large (like 10).
    */
    const uint32_t num_coauthors;

   /**
    *  void build_patent_tree(const list < const Record * > & allrec):
    *  reads a list of const Record pointers and build patent tree.
    *  NOTE THAT the comparison function for the tree is determined
    *  in the constructor.
    */
    void build_patent_tree(const RecordPList & allrec);


public:

   /**
    * cBlocking_Operation_By_Coauthors(const list < const Record * > & allrec,
    * const ClusterInfo& cluster, const uint32_t coauthors):
    * initialize a class object. allrec = a complete list of const Record pointers.
    * cluster = a ClusterInfo object, which contains all the information
    * of clustering from the previous disambiguation.
    * coauthors = number of coauthors for blocking information extraction.
    */
    cBlocking_Operation_By_Coauthors(const RecordPList & allrec,
        const ClusterInfo& cluster, const uint32_t coauthors);

   /**
    * cBlocking_Operation_By_Coauthors(const list < const Record * > & allrec,
    * const uint32_t coauthors): the second constructor.
    */
    cBlocking_Operation_By_Coauthors(const RecordPList & allrec,
        const uint32_t num_coauthors);

   /**
    *  RecordPList get_topN_coauthors( const Record * pRec, const uint32_t topN) const:
    *  find the top N most prolific coauthors of the input pRec record
    *  pointer, and return a list of const Record pointers.
    */
    // TODO: Move this back to private, or protected/friend or something.
    RecordPList get_topN_coauthors(const Record *, const uint32_t topN) const;

   /**
    * string extract_blocking_info(const Record * prec) const;
    * extract blocking string for the Record to which prec points,
    * and returns a string.
    */
    string extract_blocking_info(const Record *) const;

   /**
    * void build_uid2uinv_tree( const ClusterInfo & source):
    * build an internal unique record id to unique inventor
    * id map from a ClusterInfo object.
    * TODO: Document side effects if produced
    */
    void build_uid2uinv_tree(const ClusterInfo &);

    const PatentTree & get_patent_tree() const {
      return patent_tree;
    }

    Uid2UinvTree & get_uid2uinv_tree() {
      return uid2uinv_tree;
    }

};


#endif /* PATENT_BLOCKING_OPERATION_H */
