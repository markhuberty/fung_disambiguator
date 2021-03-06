#ifndef PATENT_WORKER_H
#define PATENT_WORKER_H



/**
 * Worker is a threading subclass to achieve multithreading in Linux systems.
 * It is used in ClusterInfo::disambiguate function.
 * It is unnecessary to understanding the detail. The only thing necessary to know is the constructor.
 */
class Worker : public Thread {

/**
 * Private:
 *        map < string, ClusterInfo::ClusterList >::iterator * ppdisambiged:
 *        the pointer to an iterator that is a cursor of progress of disambiguation.
 *
 *        const cRatios * pratios: the pointer to a cRatio object.
 *        ClusterInfo & cluster_ref: the reference of a ClusterInfo object that is actually the source.
 *        static pthread_mutex_t iter_lock: a mutex to synchronize the cursor.
 *        static unsigned int count: a static member to count the number of disambiguated blocks.
 *        void run(): the overriding function of base class, implementing details of disambiguation in each thread.
 */
private:
    map < string, ClusterInfo::ClusterList >::iterator * ppdisambiged;
    const cRatios * pratios;
    ClusterInfo & cluster_ref;

    static pthread_mutex_t iter_lock;
    static unsigned int count;
    void run();

/**
 * Public:
 *         explicit Worker( map < string, ClusterInfo::ClusterList >::iterator & input_pdisambiged,
                                            const cRatios & ratiosmap, ClusterInfo & inputcluster): constructor
 *        ~Worker(): destructor
 *        static void zero_count(): clear the variable "count" to zero
 *        static unsigned int get_count(): return the variable "count"
 *
 */
public:
    explicit Worker( map < string, ClusterInfo::ClusterList >::iterator & input_pdisambiged,
            const cRatios & ratiosmap,
            ClusterInfo & inputcluster
    ) : ppdisambiged(&input_pdisambiged), pratios(&ratiosmap), cluster_ref(inputcluster) {}

    ~Worker() {}
    static void zero_count() { count = 0; }
    static unsigned int get_count() { return count;}
};


#endif /* PATENT_WORKER_H */
