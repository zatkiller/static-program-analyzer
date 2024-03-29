
TEST_FOLDER=/Team05/Tests05


build:
	docker build -t cs3203t05 .

unit_tests:
	docker run --rm -it cs3203t05 unit_testing

integration_tests:
	docker run --rm -it cs3203t05 integration_testing

autotester:
	docker run --rm -it \
		-v $(shell pwd)${TEST_FOLDER}:${TEST_FOLDER} \
		cs3203t05 \
		autotester /$(source) /$(query) /$(out) 
