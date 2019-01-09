#include "SDCA.h"
#include "SparseLRSDCAModel.h"

namespace cirrus {

    SDCA::SDCA(double lr)
            : OptimizationMethod(lr) {}

    void SDCA::sdca_update(
            std::unique_ptr<SparseLRSDCAModel>& lr_model,
            const ModelGradient* gradient) {
        // TODO: fix

        const LRSparseGradient* grad =
                dynamic_cast<const LRSparseGradient*>(gradient);
        if (grad == nullptr) {
            throw std::runtime_error("Error in dynamic cast");
        }

        for (const auto& w : grad->weights) {
            int index = w.first;
            FEATURE_TYPE value = w.second;
            lr_model->weights_[index] += learning_rate * value;
        }
    }

}  // namespace cirrus
