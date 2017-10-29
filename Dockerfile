FROM ubuntu:16.10

ADD . /hprof-browser/

RUN apt-get update
RUN apt-get -y install cmake clang bison flex google-mock libgtest-dev lcov libgtkmm-3.0-dev
RUN cd /usr/src/gtest/ && cmake . && make && cp *.a /usr/lib
RUN cd /usr/src/gmock/ && cmake . && make && cp *.a /usr/lib
RUN export CTEST_OUTPUT_ON_FAILURE=1

CMD cd /hprof-browser/ && cmake . && cmake --build . --target all && cmake --build . --target test