#include <Checksum.h>
#include <MlUtils.h>
#include <SparseLRSDCAModel.h>
#include <Utils.h>
#include <Eigen/Dense>
#include <algorithm>
#include <map>
#include <unordered_map>

//#define DEBUG

namespace cirrus {

SparseLRSDCAModel::SparseLRSDCAModel(uint64_t d, uint64_t n) {
  weights_.resize(d);
  coord_weights_.resize(n);
}

SparseLRSDCAModel::SparseLRSDCAModel(const FEATURE_TYPE* w,
                                     uint64_t d,
                                     const FEATURE_TYPE* a,
                                     uint64_t n) {
  weights_.resize(d);
  std::copy(w, w + d, weights_.begin());

  coord_weights_.resize(n);
  std::copy(a, a + n, coord_weights_.begin());
}

uint64_t SparseLRSDCAModel::primal_size() const {
  return weights_.size();
}

uint64_t SparseLRSDCAModel::dual_size() const {
  return coord_weights_.size();
}

/**
 * Serialization / deserialization routines
 */

/** FORMAT
 * weights
 */
std::unique_ptr<CirrusModel> SparseLRSDCAModel::deserialize(
    void* data,
    uint64_t size) const {
  throw std::runtime_error("not supported");
}

std::pair<std::unique_ptr<char[]>, uint64_t> SparseLRSDCAModel::serialize()
    const {
  throw std::runtime_error("Fix. Not implemented1");
}

void SparseLRSDCAModel::serializeTo(void* mem) const {
#ifdef DEBUG
// std::cout << "Num weights size: " << weights_.size() << std::endl;
#endif
  store_value<int>(mem, weights_.size());
  store_value<int>(mem, coord_weights_.size());
  std::copy(weights_.data(), weights_.data() + weights_.size(),
            reinterpret_cast<FEATURE_TYPE*>(mem));
  std::copy(coord_weights_.data(),
            coord_weights_.data() + coord_weights_.size(),
            reinterpret_cast<FEATURE_TYPE*>(mem));
}

uint64_t SparseLRSDCAModel::getSerializedSize() const {
  auto ret =
      (primal_size() + dual_size()) * sizeof(FEATURE_TYPE) + 2 * sizeof(int);
  return ret;
}

/** FORMAT
 * number of weights (int)
 * list of weights: weight1 (FEATURE_TYPE) | weight2 (FEATURE_TYPE) | ..
 */
void SparseLRSDCAModel::loadSerialized(const void* data) {
  int num_weights = load_value<int>(data);
  int num_coords = load_value<int>(data);
#ifdef DEBUG
  std::cout << "num_weights: " << num_weights << std::endl;
  std::cout << "num_coordinates: " << num_coords << std::endl;
#endif

  char* data_begin = (char*) data;

  weights_.resize(num_weights);
  std::copy(reinterpret_cast<FEATURE_TYPE*>(data_begin),
            (reinterpret_cast<FEATURE_TYPE*>(data_begin)) + num_weights,
            weights_.data());

  advance_ptr(data, sizeof(FEATURE_TYPE) * num_weights);

  char* coord_begin = (char*) data;

  coord_weights_.resize(num_coords);
  std::copy(reinterpret_cast<FEATURE_TYPE*>(coord_begin),
            (reinterpret_cast<FEATURE_TYPE*>(coord_begin)) + num_coords,
            coord_weights_.data());
}

void SparseLRSDCAModel::randomize() {
  // Xavier initialization
  for (auto& w : weights_) {
    w = 0;
  }

  for (auto& a : coord_weights_) {
    a = 0;
  }
}

std::unique_ptr<CirrusModel> SparseLRSDCAModel::copy() const {
  std::unique_ptr<SparseLRSDCAModel> new_model =
      std::make_unique<SparseLRSDCAModel>(weights_.data(), primal_size(),
                                          coord_weights_.data(), dual_size());
  return new_model;
}

void SparseLRSDCAModel::sgd_update(double learning_rate,
                                   const ModelGradient* gradient) {
  const LRSDCASparseGradient* grad =
      dynamic_cast<const LRSDCASparseGradient*>(gradient);

  if (grad == nullptr) {
    throw std::runtime_error("Error in dynamic cast");
  }

  for (const auto& w : grad->w) {
    int index = w.first;
    FEATURE_TYPE value = w.second;
    weights_[index] += value / (learning_rate * dual_size());
  }

  for (const auto& a : grad->a) {
    int index = a.first;
    FEATURE_TYPE value = a.second;
    coord_weights_[index] += value;
  }
}

double SparseLRSDCAModel::dot_product(
    const std::vector<std::pair<int, FEATURE_TYPE>>& v1,
    const std::vector<FEATURE_TYPE>& weights_) const {
  double res = 0;
  for (const auto& feat : v1) {
    int index = feat.first;
    FEATURE_TYPE value = feat.second;
    if ((uint64_t) index >= weights_.size()) {
      std::cerr << "index: " << index << " weights.size: " << weights_.size()
                << std::endl;
      throw std::runtime_error("Index too high");
    }
    assert(index >= 0 && (uint64_t) index < weights_.size());
    res += value * weights_[index];
#ifdef DEBUG
    if (std::isnan(res) || std::isinf(res)) {
      std::cout << "res: " << res << std::endl;
      std::cout << "i: " << i << std::endl;
      std::cout << "index: " << index << " value: " << value << std::endl;
      std::cout << "weights_[index]: " << weights_[index] << std::endl;
      exit(-1);
    }
#endif
  }
  return res;
}

double SparseLRSDCAModel::norm_squared(
    const std::vector<std::pair<int, FEATURE_TYPE>>& v1) const {
  auto total = 0.0;

  for (std::pair<int, float> entry : v1) {
    total += entry.second * entry.second;
  }

  return total;
}

std::unique_ptr<ModelGradient> SparseLRSDCAModel::minibatch_grad_indexed(
    const uint64_t starting_index,
    const double learning_rate,
    const SparseDataset& dataset,
    const Configuration& config) const {
  if (is_sparse_) {
    throw std::runtime_error("This model is sparse");
  }
#ifdef DEBUG
  std::cout << "<Minibatch grad" << std::endl;
  dataset.check();
  // print();
  auto start = get_time_us();
#endif

  std::vector<std::pair<int, FEATURE_TYPE>> a_grad;
  a_grad.resize(0);

  std::unordered_map<int, FEATURE_TYPE> w_grad_map;

  for (int i = 0; i < dataset.num_samples(); i += 1) {
    const std::vector<std::pair<int, FEATURE_TYPE>>& x = dataset.get_row(i);
    FEATURE_TYPE numerator =
        1.0 + std::exp(dot_product(x, weights_) * dataset.labels_[i]);
    numerator =
        (dataset.labels_[i] / numerator) - coord_weights_[i + starting_index];

    FEATURE_TYPE denominator =
        std::max(1.0, 0.25 + (norm_squared(x) / (learning_rate * dual_size())));

    FEATURE_TYPE grad = numerator / denominator;

    a_grad.push_back(std::make_pair(i + starting_index, grad));

    for (int j = 0; j < x.size(); j += 1) {
      w_grad_map[x[j].first] += grad * x[j].second;
    }
  }

  std::vector<std::pair<int, FEATURE_TYPE>> w_grad;
  w_grad.resize(w_grad_map.size());
  int i = 0;
  for (const auto& pair : w_grad_map) {
    w_grad[i] = pair;
    i++;
  }

  std::unique_ptr<LRSDCASparseGradient> ret =
      std::make_unique<LRSDCASparseGradient>(std::move(w_grad),
                                             std::move(a_grad));
#ifdef DEBUG
  auto after_4 = get_time_us();
#endif
  // std::unique_ptr<LRGradient> ret = std::make_unique<LRGradient>(res);

#ifdef DEBUG
  ret->check_values();
  std::cout << " Elapsed1: " << (after_1 - start)
            << " Elapsed2: " << (after_2 - after_1)
            << " Elapsed3: " << (after_3 - after_2)
            << " Elapsed4: " << (after_4 - after_3) << std::endl;
#endif
  return ret;
}

std::pair<double, double> SparseLRSDCAModel::calc_loss(SparseDataset& dataset,
                                                       uint32_t) const {
  double total_loss = 0;
  auto w = weights_;

#ifdef DEBUG
  dataset.check();
#endif

  // count how many samples are wrongly classified
  uint64_t wrong_count = 0;
  for (uint64_t i = 0; i < dataset.num_samples(); ++i) {
    // get labeled class for the ith sample
    FEATURE_TYPE class_i = dataset.labels_[i];

    const auto& sample = dataset.get_row(i);
    double r1 = 0;
    for (const auto& feat : sample) {
      int index = feat.first;
      FEATURE_TYPE value = feat.second;
      r1 += weights_[index] * value;
    }

    double s1 = s_1(r1);
    FEATURE_TYPE predicted_class = 0;
    if (s1 > 0.5) {
      predicted_class = 1.0;
    }
    if (predicted_class != class_i) {
      wrong_count++;
    }

#define CROSS_ENTROPY_LOSS
#ifdef CROSS_ENTROPY_LOSS

    double value = class_i * log_aux(s1) + (1 - class_i) * log_aux(1 - s1);

    if (value > 0) {
      throw std::runtime_error("Error: logistic loss is > 0");
    }

    total_loss -= value;
#endif
  }

  if (total_loss < 0) {
    throw std::runtime_error("total_loss < 0");
  }

  FEATURE_TYPE accuracy = (1.0 - (1.0 * wrong_count / dataset.num_samples()));
  if (std::isnan(total_loss) || std::isinf(total_loss))
    throw std::runtime_error("calc_log_loss generated nan/inf");

  return std::make_pair(total_loss, accuracy);
}

uint64_t SparseLRSDCAModel::getSerializedGradientSize() const {
  return (primal_size() + dual_size()) * sizeof(FEATURE_TYPE);
}

std::unique_ptr<ModelGradient> SparseLRSDCAModel::loadGradient(
    void* mem) const {
  throw std::runtime_error("Not supported");
}

bool SparseLRSDCAModel::is_integer(FEATURE_TYPE n) const {
  return floor(n) == n;
}

double SparseLRSDCAModel::checksum() const {
  return crc32(weights_.data(), weights_.size() * sizeof(FEATURE_TYPE));
}

void SparseLRSDCAModel::print() const {
  std::cout << "MODEL PRIMAL: ";
  for (const auto& w : weights_) {
    std::cout << " " << w;
  }
  std::cout << std::endl;

  std::cout << "MODEL DUAL: ";
  for (const auto& a : coord_weights_) {
    std::cout << " " << a;
  }
  std::cout << std::endl;
}

void SparseLRSDCAModel::check() const {
  for (const auto& w : weights_) {
    if (std::isnan(w) || std::isinf(w)) {
      std::cout << "Wrong model primal weight" << std::endl;
      exit(-1);
    }
  }

  for (const auto& a : coord_weights_) {
    if (std::isnan(a) || std::isinf(a)) {
      std::cout << "Wrong model dual weight" << std::endl;
      exit(-1);
    }
  }
}

void SparseLRSDCAModel::ensure_preallocated_vectors(
    const Configuration& config) const {
  if (unique_indices.capacity() == 0) {
    unique_indices.reserve(500);
  } else {
    unique_indices.clear();
    unique_indices.reserve(500);
  }

  if (part3.capacity() == 0) {
    part3.resize(1 << config.get_model_bits());
  }

  // value needs to be less than number of samples in minibatch
  if (part2.capacity() == 0) {
    part2.resize(config.get_minibatch_size());
  }
}

}  // namespace cirrus
