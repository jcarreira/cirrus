#ifndef _LDAMODEL_H_
#define _LDAMODEL_H_

#include <vector>
#include <map>
#include <set>
#include <utility>
#include <Model.h>
#include <LDADataset.h>
#include <ModelGradient.h>

namespace cirrus{

    /**
      * Latent Dirichlet Allocation model
      */
    class LDAModel{
      public:
        LDAModel() {}
        /**
          * LDA Model constructor from serialized memory
          * @param buffer Serialized memory containing global statistics
          *               (nvt, nt)
          * @param info Serialized memory containing local statistics
          *               (ndt, topics assignments, vocab slice)
          */
        LDAModel(const char* buffer, const char* info);
        /**
          * LDA sampling function (currently using Gibbs Sampler)
          * Sampling is based on all the statistics stored in the object
          */
        std::unique_ptr<LDAUpdates> sample_model();

        void get_ndt(std::vector<std::vector<int>>& ndt_) {ndt_ = ndt;}
        void get_nt(std::vector<int>& nt_) {nt_ = nt;}

        /**
          * LDA loglikelihood method
          * TODO: would be moved to the server side
          */
        // double loglikelihood();

        /**
          * Update the global variables
          * TODO: would be moved to the server side
          */
        // void update_globals(const LDAUpdates* update);

        /**
          * Given one vocabulary slice,
          * return only partial global variables needed for sampling the slice
          *
          * @param slice Serialized memory containing vocabulary slice
          * TODO: would be moved to the server side
          */
        // char* get_partial_model(const char* slice);

      // private:
        std::unique_ptr<LDAUpdates> sample_thread(std::vector<int>& t,
                                    std::vector<int>& d,
                                    std::vector<int>& w,
                                    std::vector<int>& nt,
                                    std::vector<std::vector<int>>& nvt,
                                    std::vector<std::vector<int>>& ndt,
                                    std::vector<int>& slice
                                    );
        /**
          * K_: number of topics
          * V_: number of words
          */
        int K_, V_;
        /**
          * Set to fixed constants:
          * alpha = .1, eta = .01
          */
        double alpha = .1, eta = .01;

        /**
          * topic assignments
          */
        std::vector<int> t, d, w;
        /**
          * statistics needed for sampling
          */
        std::vector<std::vector<int>> ndt, nvt;
        std::vector<int> nt;
        /**
          * vocabulary slice containing word indices
          * that are being considered currently
          */
        std::vector<int> slice;
    };
}
#endif
