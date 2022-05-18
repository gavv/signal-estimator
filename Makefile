NUP_CPU ?= `nproc --all`
VARIANT ?= Release

all:
	mkdir -p build
	cd build && cmake -DCMAKE_BUILD_TYPE=$(VARIANT) ..
	cd build && make -j$(NUM_CPU) --no-print-directory

no_gui:
	mkdir -p build
	cd build && cmake -DCMAKE_BUILD_TYPE=$(VARIANT) -DBUILD_GUI=NO ..
	cd build && make -j$(NUM_CPU) --no-print-directory

install:
	cd build && make install

clean:
	rm -rf build
	rm -rf bin

fmt:
	find src -type f -name '*.[ch]pp' | while read f; do \
		grep -qF "$$f" .fmtignore || clang-format --verbose -i "$$f"; \
	done
