TOP_DIR = $(top_srcdir)/

AUTOMAKE_OPTIONS = foreign

CXX = g++

#: gcc -D_U_="__attribute__((unused))"  -DHAVE_CONFIG_H -static  -I. -Infs -Iinclude/  -Iinclude/nfsc/ -Imount examples/nfsclient-write.c lib/libnfs-sync.c nfs4/nfs4.c nfs/nfs.c nfs/libnfs-raw-nfs.c mount/mount.c  mount/libnfs-raw-mount.c nfs4/libnfs-raw-nfs4.c portmap/portmap.c portmap/libnfs-raw-portmap.c  -L/home/ec2-user/joao/libnfs/lib/ -lnfs -o nfsclient-writ

bin_PROGRAMS = parameter_server ps_test csv_to_libsvm

LIBS         =  -laws-cpp-sdk-s3 -laws-cpp-sdk-core \
		-lcurl -lssl -lcrypto -lz -ldl -lkrb5 -lk5crypto \
		-lall -lkeyutils -lresolv -lgflags -lnfs

LINCLUDES    = -L$(TOP_DIR)/third_party/aws-sdk-cpp/build/aws-cpp-sdk-core/ \
	       -L$(TOP_DIR)/third_party/aws-sdk-cpp/build/aws-cpp-sdk-s3 \
	       -L$(TOP_DIR)/third_party/kerberos/src/lib \
	       -L$(TOP_DIR)/third_party/keyutils/ \
	       -L$(TOP_DIR)/third_party/gflags/lib/ \
	       -L$(TOP_DIR)/third_party/curl/curl/lib/.libs/ \
	       -L/home/ec2-user/joao/libnfs/lib/

LDADD    = $(LINCLUDES) $(LIBS)
LDFLAGS  = -static-libgcc  -static  -Wl,--whole-archive \
	   -lpthread -Wl,--no-whole-archive

CPPFLAGS = -static -static-libgcc -static-libstdc++ \
	   -DHAVE_CONFIG_H \
	   -D_U_="__attribute__((unused))" \
           -I$(TOP_DIR) \
           -I$(TOP_DIR)/third_party/flatbuffers/include \
           -I$(TOP_DIR)/third_party/eigen_source \
	   -I$(TOP_DIR)/third_party/aws-sdk-cpp/aws-cpp-sdk-s3/include/ \
	   -I$(TOP_DIR)/third_party/aws-sdk-cpp/aws-cpp-sdk-core/include/  \
	   -I$(TOP_DIR)/third_party/gflags/include/ \
	   -I/home/ec2-user/joao/libnfs/include/nfsc \
	   -I/home/ec2-user/joao/libnfs/              -I/home/ec2-user/joao/libnfs/nfs -I/home/ec2-user/joao/libnfs/include/  \
	   -I/home/ec2-user/joao/libnfs/include/nfsc/ -I/home/ec2-user/joao/libnfs/mount \
	   -I/home/ec2-user/joao/libnfs/infs/ \
	   -ggdb -frecord-gcc-switches -O3

CPP_SOURCES = InputReader.cpp Utils.cpp \
              Dataset.cpp Matrix.cpp \
	      Model.cpp LRModel.cpp SoftmaxModel.cpp SparseLRModel.cpp \
	      SparseMFModel.cpp MFModel.cpp\
              ModelGradient.cpp MlUtils.cpp Configuration.cpp \
              Checksum.cpp \
              ErrorSparseTask.cpp \
	      LogisticSparseTaskS3.cpp \
	      MFNetflixTask.cpp \
	      LoadingSparseTaskS3.cpp LoadingNetflixTask.cpp \
              TasksWait.cpp MurmurHash3.cpp \
	      S3Iterator.cpp S3SparseIterator.cpp \
	      PerformanceLambdaTask.cpp \
	      S3.cpp SparseDataset.cpp \
	      PSSparseServerInterface.cpp \
	      PSSparseServerTaskEFS.cpp \
	      NFSFile.cpp \
	      /home/ec2-user/joao/libnfs/lib/libnfs-sync.c /home/ec2-user/joao/libnfs/nfs4/nfs4.c /home/ec2-user/joao/libnfs/nfs/nfs.c \
	      /home/ec2-user/joao/libnfs/nfs/libnfs-raw-nfs.c   /home/ec2-user/joao/libnfs/mount/mount.c     /home/ec2-user/joao/libnfs/mount/libnfs-raw-mount.c \
	      /home/ec2-user/joao/libnfs/nfs4/libnfs-raw-nfs4.c /home/ec2-user/joao/libnfs/portmap/portmap.c /home/ec2-user/joao/libnfs/portmap/libnfs-raw-portmap.c


CPP_SOURCES_SOFTMAX = InputReader.cpp Utils.cpp \
              Dataset.cpp Matrix.cpp Model.cpp LRModel.cpp \
              ModelGradient.cpp MlUtils.cpp Configuration.cpp \
              Checksum.cpp SoftmaxModel.cpp \
              Serializers.cpp  \
              SparseDataset.cpp MurmurHash3.cpp

parameter_server_SOURCES  = parameter_server.cpp $(CPP_SOURCES)

ps_test_SOURCES = PSSparseServerTaskTest.cpp $(CPP_SOURCES)

csv_to_libsvm_SOURCES = CriteoCsvToLibSVM.cpp $(CPP_SOURCES)
