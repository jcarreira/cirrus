docker run --memory=5G docker/base \
  sh jenkins_tests/test_lr.sh

docker run --memory=5G docker/base \
  sh jenkins_tests/test_jester.sh

docker run --memory=5G docker/base \
  sh jenkins_tests/test_mf.sh
