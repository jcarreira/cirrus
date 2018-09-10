#ifndef _DATASETCONVERSION_H
#define _DATASETCONVERSION_H

#include <Dataset.h>
#include <SparseDataset.h>

namespace cirrus {
/**
 * Converts an object of type Dataset to type SparseDataset
 * @param d Dataset to convert
 * @return SparseDataset that contains the same data as d
 */
SparseDataset to_sparse(const Dataset& d);

/**
 * Converts an object of type SparseDataset to type Dataset
 * @param sd SparseDataset to convert
 * @param config Configuration object
 * @return Dataset that contains the same data as sd
 */
Dataset to_dataset(const SparseDataset& sd, const Configuration& config);
}  // namespace cirrus

#endif
