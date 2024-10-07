NUM_CPU ?= $(shell nproc --all)
VARIANT ?= Release
WERROR ?= OFF
SANITIZERS ?= OFF
TESTS ?= OFF

MACHINE := $(shell uname -m)-linux-gnu

CMAKE_CMD := cmake \
	-DCMAKE_BUILD_TYPE=$(VARIANT) \
	-DBUILD_TESTING=$(TESTS) \
	-DENABLE_WERROR=$(WERROR) \
	-DENABLE_SANITIZERS=$(SANITIZERS)

MAKE_CMD := make -j$(NUM_CPU) --no-print-directory

TEST_CMD := ./bin/$(MACHINE)/signal-estimator-test

DOCKER_CMD := docker run -t --rm \
	-u "$(shell id -u)" \
	-v "$(shell pwd):$(shell pwd)" \
	-w "$(shell pwd)"

all:
	mkdir -p build/$(MACHINE)
	cd build/$(MACHINE) && $(CMAKE_CMD) ../..
	cd build/$(MACHINE) && $(MAKE_CMD)
ifeq ($(TESTS),ON)
	@echo Running tests ...
	$(TEST_CMD)
endif

no_gui:
	mkdir -p build/$(MACHINE)
	cd build/$(MACHINE) && $(CMAKE_CMD) -DBUILD_GUI=NO ../..
	cd build/$(MACHINE) && $(MAKE_CMD)
ifeq ($(TESTS),ON)
	@echo Running tests ...
	$(TEST_CMD)
endif

arm32:
	mkdir -p build/arm-linux-gnueabihf
	$(DOCKER_CMD) rocstreaming/toolchain-arm-linux-gnueabihf /bin/bash -c \
		"cd build/arm-linux-gnueabihf && \
			$(CMAKE_CMD) -DBUILD_GUI=NO -DTOOLCHAIN_PREFIX=arm-linux-gnueabihf ../.. && \
			$(MAKE_CMD)"

arm64:
	mkdir -p build/aarch64-linux-gnu
	$(DOCKER_CMD) rocstreaming/toolchain-aarch64-linux-gnu /bin/bash -c \
		"cd build/aarch64-linux-gnu && \
			$(CMAKE_CMD) -DBUILD_GUI=NO -DTOOLCHAIN_PREFIX=aarch64-linux-gnu ../.. && \
			$(MAKE_CMD)"

install:
	cd build && make install

clean:
	rm -rf build
	rm -rf bin

fmt:
	find src -type f -name '*.[ch]pp' | while read f; do \
		grep -qF "$$f" .fmtignore || clang-format --verbose -i "$$f"; \
	done
