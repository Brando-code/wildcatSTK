#!/bin/bash

lcov --directory . --capture --no-external --output-file cov.info
genhtml --output-directory=UnitTests/Coverage cov.info

