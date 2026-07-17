ifeq ($(OS),Windows_NT)
PYTHON ?= py -3
else
PYTHON ?= python3
endif

CODEGEN := generate_nanopb.py

.PHONY: help protobuf generate-stm32 check-stm32

help:
	@echo protobuf       Regenerate the STM32 nanopb C bindings
	@echo check-stm32   Fail if the checked-in STM32 bindings are stale

protobuf: generate-stm32

generate-stm32:
	$(PYTHON) "$(CODEGEN)" generate

check-stm32:
	$(PYTHON) "$(CODEGEN)" check
