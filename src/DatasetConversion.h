#ifndef _DC_H
#define _DC_H

#include <Dataset.h>
#include <SparseDataset.h>

namespace cirrus {
  SparseDataset to_sparse(Dataset& d);
  Dataset to_dataset(SparseDataset& sd);
}
