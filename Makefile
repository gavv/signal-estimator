NUP_CPU ?= `nproc --all`
VARIANT ?= Release
SANITIZERS ?= OFF

all:
	mkdir -p build
	cd build && cmake -DCMAKE_BUILD_TYPE=$(VARIANT) -DENABLE_SANITIZERS=$(SANITIZERS) ..
	cd build && make -j$(NUM_CPU) --no-print-directory VERBOSE=1

no_gui:
	mkdir -p build
	cd build && cmake -DCMAKE_BUILD_TYPE=$(VARIANT) -DENABLE_SANITIZERS=$(SANITIZERS) -DBUILD_GUI=NO ..
	cd build && make -j$(NUM_CPU) --no-print-directory VERBOSE=1

install:
	cd build && make install

clean:
	rm -rf build
	rm -rf bin

fmt:
	find src -type f -name '*.[ch]pp' | while read f; do \
		grep -qF "$$f" .fmtignore || clang-format --verbose -i "$$f"; \
	done
