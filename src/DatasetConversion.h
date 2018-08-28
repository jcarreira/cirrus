#ifndef _DATASETCONVERSION_H
#define _DATASETCONVERSION_H

#include <Dataset.h>
#include <SparseDataset.h>

namespace cirrus {
SparseDataset to_sparse(const Dataset& d);
Dataset to_dataset(const SparseDataset& sd, const Configuration& config);
}  // namespace cirrus

#endif
