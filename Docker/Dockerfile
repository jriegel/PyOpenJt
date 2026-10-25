# Linux wheel build environment (manylinux_2_28) for PyOpenJt.
#
#   docker build -f Docker/Dockerfile --target release -t pyopenjt-wheel .
#   docker run --rm -v "${PWD}/dist:/out" pyopenjt-wheel
#
# Target "dev" is used by .devcontainer; target "release" builds the wheel.
FROM quay.io/pypa/manylinux_2_28_x86_64 AS dev

RUN yum install -y zip unzip tar curl perl-IPC-Cmd git \
    && yum clean all

ENV VCPKG_ROOT=/opt/vcpkg \
    VCPKG_FORCE_SYSTEM_BINARIES=1 \
    VCPKG_DISABLE_METRICS=1 \
    VCPKG_DEFAULT_TRIPLET=x64-linux-dynamic \
    CMAKE_TOOLCHAIN_FILE=/opt/vcpkg/scripts/buildsystems/vcpkg.cmake \
    PYBIN=/opt/python/cp39-cp39/bin

RUN git clone --depth 1 https://github.com/microsoft/vcpkg ${VCPKG_ROOT} \
    && ${VCPKG_ROOT}/bootstrap-vcpkg.sh -disableMetrics

RUN ${PYBIN}/pip install --no-cache-dir build scikit-build-core cmake ninja auditwheel

ENV PATH=${PYBIN}:${PATH}
WORKDIR /project

# ---------------------------------------------------------------------------
FROM dev AS release

COPY . /project
ENV LD_LIBRARY_PATH=/project/build/wheel/vcpkg_installed/x64-linux-dynamic/lib

# Build the abi3 wheel, bundle shared libs, then smoke test the repaired wheel.
CMD ["bash", "-c", "set -e; rm -rf /tmp/raw; \
    python -m pip wheel . --no-deps --no-build-isolation -w /tmp/raw && \
    auditwheel repair -w /out /tmp/raw/*.whl && \
    python -m pip install --force-reinstall /out/*.whl && \
    python tests/test_smoke.py Data/ExampleFiles/example_block_jt9.5.jt"]
