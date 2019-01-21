#ifndef _SDCA_H_
#define _SDCA_H_

#include "OptimizationMethod.h"
#include "SparseLRSDCAModel.h"

namespace cirrus {

    class SDCA : public OptimizationMethod {
    public:
        SDCA(double lr);

        void sdca_update(
                std::unique_ptr<SparseLRSDCAModel>& lr_model,
                const ModelGradient* gradient);

        void sgd_update(
            std::unique_ptr<cirrus::SparseLRModel> &lr_model,
            const cirrus::ModelGradient *gradient);
    };

}  // namespace cirrus

#endif  // _SDCA_H_
