#include <DatasetConversion.h>

namespace cirrus {
SparseDataset to_sparse(const Dataset& d) {
  const FEATURE_TYPE* m_data =
      reinterpret_cast<const FEATURE_TYPE*>(d.samples_.data.get());
  std::vector<std::vector<std::pair<int, FEATURE_TYPE>>> sparse_data(
      d.samples_.rows);
  for (int i = 0; i < d.samples_.rows; i++) {
    std::vector<std::pair<int, FEATURE_TYPE>> row(d.samples_.cols);
    for (int j = 0; j < d.samples_.cols; j++) {
      row[j] = std::make_pair(j, m_data[i * d.samples_.cols + j]);
    }
    sparse_data[i] = row;
  }
  std::vector<FEATURE_TYPE> labels(d.samples_.rows);
  for (int i = 0; i < d.samples_.rows; i++) {
    labels[i] = d.labels_.get()[i];
  }
  return SparseDataset(std::move(sparse_data), std::move(labels));
}

Dataset to_dataset(const SparseDataset& sd, const Configuration& config) {
  std::vector<std::vector<FEATURE_TYPE>> data(sd.data_.size());
  for (int i = 0; i < sd.data_.size(); i++) {
    std::vector<FEATURE_TYPE> row(config.get_num_features());
    for (int j = 0; j < config.get_num_features(); j++) {
      // Adds zeroes into the places that aren't specified by SparseDataset
      if (j == sd.data_[i][j].first) {
        row[j] = sd.data_[i][j].second;
      } else {
        row[j] = 0;
      }
    }
    data[i] = row;
  }
  return Dataset(data, sd.labels_);
}
}  // namespace cirrus
