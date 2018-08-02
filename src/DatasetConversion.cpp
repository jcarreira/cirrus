#include <DatasetConversion.h>

namespace cirrus {
SparseDataset to_sparse(const Dataset& d) {
  Matrix samples_ = d.get_samples();
  std::shared_ptr<const FEATURE_TYPE> labels_ = d.get_labels();
  const FEATURE_TYPE* m_data = reinterpret_cast<const
  FEATURE_TYPE*>(samples_.data.get());
  std::vector<std::vector<std::pair<int, FEATURE_TYPE>>> sparse_data;
  for (int i = 0; i < samples_.rows; i++) {
    std::vector<std::pair<int, FEATURE_TYPE>> row;
    for (int j = 0; j < samples_.cols; j++) {
      row.push_back(std::make_pair(j, m_data[i * samples_.cols + j]));
    }
    sparse_data.push_back(row);
  }
  std::vector<FEATURE_TYPE> labels;
  for (size_t i = 0; i < samples_.rows; i++) {
    labels.push_back(labels_.get()[i]);
  }
  return SparseDataset(std::move(sparse_data), std::move(labels));
}

Dataset to_dataset(const SparseDataset& sd, const Configuration& config) {
  std::vector<std::vector<std::pair<int, FEATURE_TYPE>>> data_ = sd.data_;
  std::vector<FEATURE_TYPE> labels_ = sd.labels_;
  std::vector<std::vector<FEATURE_TYPE>> data;
  for (int i = 0; i < data_.size(); i++) {
    std::vector<FEATURE_TYPE> row;
    for (int j = 0; j < config.get_num_features(); j++) {
      if (j == data_[i][j].first) {
        row.push_back(data_[i][j].second);
      } else {
        row.push_back(0);
      }
    }
    data.push_back(row);
  }
  return Dataset(data, labels_);
}
}
