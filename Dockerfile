FROM ghcr.io/rootkie/fedoracmake34:main

# install dependencies
# RUN dnf -y update && dnf -y install make cmake automake gcc-c++ gcc zip gdb

WORKDIR /Team05
ADD Team05 .

WORKDIR /Team05/Code05/build

ARG CMAKE_BUILD_TYPE=Release
RUN cmake -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} ..
RUN make -j $(nproc)
RUN ln -s /Team05/Code05/build/src/autotester/autotester /usr/bin/autotester
RUN ln -s /Team05/Code05/build/src/unit_testing/unit_testing /usr/bin/unit_testing
RUN ln -s /Team05/Code05/build/src/integration_testing/integration_testing /usr/bin/integration_testing

