FROM centos:7

RUN yum install -y epel-release
RUN yum install -y centos-release-scl-rh
RUN yum update -y

RUN yum install -y boost169-devel \
                   devtoolset-9-gcc-c++ \
                   make \
                   cmake3 \
                   lsof \
                   python3 \
                   python3-pip \
                   rapidjson-devel \
                   valgrind

RUN pip3 install nose pysisl