INCLUDE_FLAGS = -I$(PWD)/src
EXAMPLES = AHTX0 SCAN SSD1306

define build-target
	$(foreach example,$(EXAMPLES),arduino-cli compile --fqbn $(1) \
		--library $(PWD)/src \
		--export-binaries ./examples/$(example)/$(example).ino;)
endef

.PHONY: clean
clean:
	rm -fr ./tmp/
	$(foreach example,$(EXAMPLES),rm -fr ./examples/$(example)/build/;)
	$(foreach example,$(EXAMPLES),rm -f ./examples/$(example)/*.lst;)

.PHONY: build
build: build/uno build/leonardo build/digispark build/attiny85 build/attiny13

.PHONY: build/uno
build/uno:
	$(call build-target,arduino:avr:uno)

.PHONY: build/leonardo
build/leonardo:
	$(call build-target,arduino:avr:leonardo)

.PHONY: build/digispark
build/digispark:
	$(call build-target,ATTinyCore:avr:attinyx5micr)

.PHONY: build/attiny85
build/attiny85:
	$(call build-target,ATTinyCore:avr:attinyx5:chip=85,clock=8internal)

.PHONY: build/attiny13
build/attiny13:
	$(call build-target,MicroCore:avr:13)

.PHONY: core/update-index
core/update-index:
	arduino-cli core update-index
	arduino-cli core update-index --additional-urls https://drazzy.com/package_drazzy.com_index.json
	arduino-cli core update-index --additional-urls https://mcudude.github.io/MicroCore/package_MCUdude_MicroCore_index.json

.PHONY: core/install
core/install: core/update-index
	arduino-cli core install arduino:avr
	arduino-cli core install ATTinyCore:avr
	arduino-cli core install MicroCore:avr
