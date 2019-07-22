#!/bin/bash

lcov --directory . --capture --no-external --output-file cov.info
genhtml --output-directory=BOOST.TESTS/Coverage cov.info

