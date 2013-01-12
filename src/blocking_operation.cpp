
#include "cluster.h"
#include "newcluster.h"
#include "engine.h"
//#include "engine.h"
// TODO: Change to include only blocking_operation.h
//#include "blocking_operation.h"


/*
 * Aim: constructors of BlockByColumns 
 * class. Look at the header file for more information.
 */
BlockByColumns::BlockByColumns (
    const vector<const StringManipulator *> & inputvsm,
    const vector<string> & columnnames,
    const vector<uint32_t> & di)
    : vsm(inputvsm), attributes_names(columnnames) {

    if (inputvsm.size() != columnnames.size())
        throw cException_Other("Critical Error in BlockByColumns: size of string manipulaters is different from size of columns");

    for (uint32_t i = 0; i < columnnames.size(); ++i) {
        indice.push_back(Record::get_index_by_name(columnnames.at(i)));
        infoless += delim;
        pdata_indice.push_back(di.at(i));
    }
}


BlockByColumns::BlockByColumns (
    const StringManipulator * const * pinputvsm,
    const string * pcolumnnames,
    const uint32_t * pdi,
    const uint32_t num_col) {

    for (uint32_t i = 0; i < num_col; ++i) {
        vsm.push_back(*pinputvsm++);
        attributes_names.push_back(*pcolumnnames);
        indice.push_back(Record::get_index_by_name(*pcolumnnames++));
        infoless += delim;
        pdata_indice.push_back(*pdi ++);
    }
}



/**
 * Aim: to extract blocking information from a record pointer and 
 * returns its blocking id string.
 *
 * Algorithm: call the polymorphic methods by StringManipulator 
 * pointers to create strings, and concatenate them.
 */
string
BlockByColumns::extract_blocking_info(const Record * p) const {

    string temp;

    for (uint32_t i = 0; i < vsm.size(); ++i) {
      // std::cout << "i " << i << std::endl;
        uint32_t index = pdata_indice.at(i);

        // std::cout << "index " << index << std::endl;
        // std::cout << "indice " << indice.size() << std::endl;
        //temp += vsm[i]->manipulate(* p->get_data_by_index(indice[i]).at(pdata_indice.at(i)));
        int temp_data = p->get_data_by_index(indice[i]).size();
        // std::cout << temp_data << std::endl;

        temp += vsm[i]->manipulate(* p->get_data_by_index(indice[i]).at(index));
        temp += delim;
     

    }
    // std::cout << temp << std::endl;
    return temp;
};


void
BlockByColumns::reset_data_indice (const vector<uint32_t> & indice) {

    if (indice.size() != this->pdata_indice.size())
        throw cException_Other("Indice size mismatch. cannot reset.");
    else
        this->pdata_indice = indice;
}


/**
 * Aim: to extract a specific blocking string. look at the header file for mor details.
 */
string
BlockByColumns::extract_column_info (const Record * p, uint32_t flag) const {

    if (flag >= indice.size()) {
        throw cException_Other("Flag index error.");
    }

    return vsm[flag]->manipulate(*p->get_data_by_index(indice[flag]).at( pdata_indice.at(flag)));
}


void
too_many_coauthors(const uint32_t num_coauthors) {

    if (num_coauthors > 4) {
        std::cout << "================ WARNING =====================" << std::endl;
        std::cout << "Number of coauthors in which cBlocking_Operation_By_Coauthors uses ";
        std::cout << "is probably too large. Number of coauthors = " << num_coauthors << std::endl;
        std::cout << "==================END OF WARNING ================" << std::endl;
    }
}


/**
 * Aim: constructors of cBlocking_Operation_By_Coauthors.
 *
 * The difference between the two constructors is that the former
 * one builds unique record id to unique inventor id binary tree,
 * whereas the latter does not, demanding external explicit call
 * of the building of the uid2uinv tree.
 */
cBlocking_Operation_By_Coauthors::cBlocking_Operation_By_Coauthors(
    const RecordPList & all_rec_pointers,
    const ClusterInfo & cluster,
    const uint32_t coauthors)
    : patent_tree(cSort_by_attrib(cPatent::static_get_class_name())),
      num_coauthors(coauthors) {

    too_many_coauthors(num_coauthors);

    build_patent_tree(all_rec_pointers);
    build_uid2uinv_tree(cluster);

    for (uint32_t i = 0; i < num_coauthors; ++i) {
        infoless += cBlocking_Operation::delim;
        infoless += cBlocking_Operation::delim;
    }
}



cBlocking_Operation_By_Coauthors::cBlocking_Operation_By_Coauthors(
    const RecordPList & all_rec_pointers,
    const uint32_t coauthors)
    : patent_tree(cSort_by_attrib(cPatent::static_get_class_name())),
      num_coauthors(coauthors) {

    too_many_coauthors(num_coauthors);
    build_patent_tree(all_rec_pointers);

    for (uint32_t i = 0; i < num_coauthors; ++i) {
        infoless += cBlocking_Operation::delim + cBlocking_Operation::delim;
        infoless += cBlocking_Operation::delim + cBlocking_Operation::delim;
    }
}


/**
 * Aim: to create a binary tree of patent -> patent holders
 * to allow fast search.
 *
 * Algorithm: create a patent->patent holder map (std::map),
 * and for any given const Record pointer p, look for the patent
 * attribute of p in * the map. If the patent attribute is not found,
 * insert p into the map as a key, and insert a list of const Record
 * pointers which includes p as * a value of the key; if the patent
 * attribute is found, find the corresponding value ( which is a list ),
 * and append p into the list.
 */
void
cBlocking_Operation_By_Coauthors::build_patent_tree(
    const RecordPList & all_rec_pointers) {

    map<const Record *, RecordPList, cSort_by_attrib>::iterator ppatentmap;

    RecordPList::const_iterator p = all_rec_pointers.begin();
    for (; p != all_rec_pointers.end(); ++p) {
        ppatentmap = patent_tree.find(*p);

        if (ppatentmap == patent_tree.end()) {
            RecordPList temp (1, *p);
            patent_tree.insert(std::pair<const Record *, RecordPList> (*p, temp));
        }
        else {
            ppatentmap->second.push_back(*p);
        }

    }
}


/*
 * Aim: to create binary tree of unique record id -> unique inventor id,
 * also to allow fast search and insertion/deletion.
 * the unique inventor id is also a const Record pointer,
 * meaning that different unique record ids may be associated with a same
 * const Record pointer that represents them.
 *
 * Algorithm: clean the uinv2count and uid2uinv tree first.
 *   For any cluster in the cCluser_Info object:
 *     For any const Record pointer p in the cluster member list:
 *       create a std::pair of (p, d), where d is the delegate
 *         (or representative) of the cluster
 *       insert the pair into uid2uinv map.
 *     End for
 *   End for
 *
 *   uinv2count is updated in the same way.
 */
void
cBlocking_Operation_By_Coauthors::build_uid2uinv_tree(const ClusterInfo & cluster) {

    uinv2count_tree.clear();
    uid2uinv_tree.clear();
    uint32_t count = 0;
    //typedef list<Cluster> cRecGroup;
    // Maybe should be RecordGroup
    typedef list<Cluster> ClusterList;

    std::cout << "Building trees: 1. Unique Record ID to Unique Inventer ID. ";
    std::cout << "2 Unique Inventer ID to Number of holding patents ........";
    std::cout << std::endl;

    map<string, ClusterList>::const_iterator p = cluster.get_cluster_map().begin();
    for (; p != cluster.get_cluster_map().end(); ++p) {

        ClusterList::const_iterator q = p->second.begin();
        for (; q != p->second.end(); ++q) {

            const Record * value = q->get_cluster_head().m_delegate;
            map<const Record *, uint32_t>::iterator pcount = uinv2count_tree.find(value);
            if (pcount == uinv2count_tree.end())
                pcount = uinv2count_tree.insert(std::pair<const Record *, uint32_t>(value, 0)).first;

            for (RecordPList::const_iterator r = q->get_fellows().begin(); r != q->get_fellows().end(); ++r) {
                const Record * key = *r;
                uid2uinv_tree.insert(std::pair<const Record * , const Record *>(key, value ));
                ++(pcount->second);
                ++count;
            }
        }
    }
    std::cout << count << " nodes has been created inside the tree." << std::endl;
}


/**
 * Aim: to get a list of top N coauthors
 * (as represented by a const Record pointer)
 * of an inventor to whom prec (a unique
 * record id) belongs.
 *
 * Algorithm:
 *
 *  1. create a binary tree T(std::map)
 *     Key = number of holding patents,
 *     value = const Record pointer to the unique inventor.
 *
 *  2. For any associated record r:
 *   find r in the uinv2count tree.
 *   if number of nodes in T < N, insert (count(r), r) into T;
 *   else
 *       if count(r) > front of T:
 *           delete front of T from T
 *           insert ( count(r), r );
 *
 *  3. return values in T.
 */
RecordPList cBlocking_Operation_By_Coauthors::get_topN_coauthors(
    const Record * prec, const uint32_t topN) const {

    const RecordPList & list_alias = patent_tree.find(prec)->second;
    map<uint32_t, RecordPList> occurrence_map;
    uint32_t cnt = 0;

    RecordPList::const_iterator p = list_alias.begin();
    for (; p != list_alias.end(); ++p) {

        if (*p == prec) continue;

        map<const Record *, const Record *>::const_iterator puid2uiv = uid2uinv_tree.find(*p);
        if (puid2uiv == uid2uinv_tree.end())
            throw cException_Other("Critical Error: unique record id to unique inventer id tree is incomplete!!");

        const Record * coauthor_pointer = puid2uiv->second;

        map<const Record *, uint32_t>::const_iterator puinv2count = uinv2count_tree.find(coauthor_pointer);

        if ( puinv2count == uinv2count_tree.end())
            throw cException_Other("Critical Error: unique inventer id to number of holding patents tree is incomplete!!");

        const uint32_t coauthor_count = puinv2count->second;

        if (cnt <= topN || coauthor_count > occurrence_map.begin()->first) {

            map < uint32_t, RecordPList >::iterator poccur = occurrence_map.find (coauthor_count);
            if (poccur == occurrence_map.end()) {
                RecordPList temp (1, coauthor_pointer);
                occurrence_map.insert(std::pair<uint32_t, RecordPList>(coauthor_count, temp));
            }
            else {
                poccur->second.push_back(coauthor_pointer);
            }

            if (cnt < topN) {
                ++cnt;
            } else {
                map < uint32_t, RecordPList >::iterator pbegin = occurrence_map.begin();
                pbegin->second.pop_back();
                if (pbegin->second.empty()) {
                    occurrence_map.erase(pbegin);
                }
            }

        }
    }

    //output
    RecordPList ans;

    map<uint32_t, RecordPList>::const_reverse_iterator rp = occurrence_map.rbegin();
    for (; rp != occurrence_map.rend(); ++rp) {
        ans.insert(ans.end(), rp->second.begin(), rp->second.end());
    }

    return ans;
}


/**
 * Aim: to get the blocking string id for prec.
 * Algorithm: see get_topN_coauthor
 */
string
cBlocking_Operation_By_Coauthors::extract_blocking_info(const Record * prec) const {

    const RecordPList top_coauthor_list = get_topN_coauthors(prec, num_coauthors);
    // now make string
    const uint32_t nameindex = Record::get_index_by_name(cFirstname::static_get_class_name());

    // const uint32_t firstnameindex = Record::get_index_by_name(cFirstname::static_get_class_name());
    // const uint32_t lastnameindex = Record::get_index_by_name(cLastname::static_get_class_name());

    string answer;

    RecordPList::const_iterator p = top_coauthor_list.begin();
    for (; p != top_coauthor_list.end(); ++p) {
      answer += *(*p)->get_data_by_index(nameindex).at(0);
      // For PATSTAT, need to read entire name; at(0) only has first word in name
      // answer += *(*p)->get_data_by_index(nameindex).at(1);
        answer += cBlocking_Operation::delim;
        // answer += *(*p)->get_data_by_index(lastnameindex).at(0);
        // answer += cBlocking_Operation::delim;
    }
    if (answer.empty()) answer = infoless;
    return answer;
}

